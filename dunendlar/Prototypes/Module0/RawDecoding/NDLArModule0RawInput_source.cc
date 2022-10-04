#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art_root_io/TFileService.h"
#include "NDLArModule0RawInput.h"
#include "dunecore/DuneObj/DUNEHDF5FileInfo.h"
#include "dunendlar/DUNENDLArObj/RawPixel.h"
#include "dunendlar/DUNENDLArObj/Module0Trigger.h"
#include "detdataformats/pacman/PACMANFrame.hpp"
#include "dunendlar/Prototypes/Module0/ChannelMap/NDLArModule0ChannelMapService.h"

dune::NDLArModule0RawInputDetail::NDLArModule0RawInputDetail(
                                                             fhicl::ParameterSet const & ps,
                                                             art::ProductRegistryHelper & rh,
                                                             art::SourceHelper const & sh) :
  pmaker(sh) {
  pretend_module_name = ps.get<std::string>("raw_data_label", "daq");
  fLogLevel = ps.get<int>("LogLevel", 0);
  fConfigRunNumber = ps.get<size_t>("RunNumber",1);  
  fCurRun = fConfigRunNumber;
  fConfigSubRunNumber = ps.get<size_t>("SubRunNumber",1);
  fConfigNTickTriggerReadout = ps.get<size_t>("NTickTriggerReadout",2000);  
  fConfigNTickNextTriggerGap = ps.get<size_t>("NTickNextTriggerGap",100);    

  // trigger words come in
  fConfigMaxMessageLookBack = ps.get<size_t>("MaxMessageLookBack",600);  // look 600 messages before the current trigger for
  // data that may be in the timestamp window.  Also use this to look after the last message to contribute in the 
  // window for possibly more out-of sequence data.  Cover the 512-word bug

  rh.reconstitutes<std::vector<raw::RawPixel>, art::InEvent>(pretend_module_name);
  rh.reconstitutes<std::vector<raw::Module0Trigger>, art::InEvent>(pretend_module_name);

  art::ServiceHandle<art::TFileService> tfs;
  fHist_tts = tfs->make<TH1F>("tts","Trigger Timestamps",200,0,2E7);
  fHist_dts = tfs->make<TH1F>("dts","LArPix Data Timestamps",200,0,2E7);
  fHist_deltats = tfs->make<TH1F>("deltats","LArPix - Trigger Timestamps",200,-2000.0,4000.0);
}

void dune::NDLArModule0RawInputDetail::readFile(
                                                std::string const & filename, art::FileBlock*& fb) {
  fHDFFile = dune::HDF5Utils::openFile(filename);
  fHDFFile->runNumber = fCurRun;
  fCurEvent = 0;
  fCurMessage = 0;
  fCurTrigTS = 0;

  // read all the io_groups into memory.  Only reason to do this is because we need to read all the
  // messages into memory at the same time too.  The i/o groups take less space.

  hid_t headerdsid = H5Dopen(fHDFFile->filePtr,"msg_headers",H5P_DEFAULT);
  hid_t headerspaceid = H5Dget_space(headerdsid);
  hsize_t headerndims = H5Sget_simple_extent_ndims(headerspaceid);
  if (fLogLevel > 1)
    {
      std::cout << "NDLArModule0Source: msg_headers dataspace ndims: " << headerndims << std::endl;
    }
  std::vector<hsize_t> headerdims(headerndims);
  std::vector<hsize_t> headermaxdims(headerndims);
  H5Sget_simple_extent_dims(headerspaceid,headerdims.data(),headermaxdims.data());
  if (fLogLevel > 1)
    {
      std::cout << "NDLArModule0Source: msg_headers dims: " << headerdims[0] << std::endl;
      std::cout << "NDLArModule0Source: msg_headers maxdims: " << headermaxdims[0] << std::endl;
    }

  fIogBuff.resize(headerdims[0]);

  hid_t iogroup_tid = H5Tcreate(H5T_COMPOUND, sizeof(ig_t));
  H5Tinsert(iogroup_tid, "io_groups", HOFFSET(ig_t, iog), H5T_STD_U8LE);
  H5Dread(headerdsid, iogroup_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, fIogBuff.data());
  //for (size_t i=0;i<headerdims[0]; ++i)
  //    {
  //  std::cout << "io_group: " << i << "  " << (int) iogbuff[i].iog << std::endl;
  //}

  H5Sclose(headerspaceid);
  H5Tclose(iogroup_tid);
  H5Dclose(headerdsid);

  // currently read all the messages into memory until we can figure out
  // how to read a subset of them

  hid_t datasetid = H5Dopen(fHDFFile->filePtr,"msgs",H5P_DEFAULT);
  fRDataspaceid = H5Dget_space(datasetid);
  hsize_t     dims[1] = {2};
  H5Sget_simple_extent_dims (fRDataspaceid, dims, NULL);
  fVlt = H5Tvlen_create(H5T_STD_U8LE);
  fRData = (hvl_t *) malloc (dims[0] * sizeof (hvl_t));
  H5Dread(datasetid, fVlt, H5S_ALL, H5S_ALL, H5P_DEFAULT, fRData);
  fNMessages = dims[0];
  H5Dclose(datasetid);

  if (fIogBuff.size() != dims[0])
    {
      throw cet::exception("NDLArModule0RawInput") << "Different numbers of io_groups and messages " <<
        fIogBuff.size() << " != " << dims[0]; 
    }

  MF_LOG_INFO("NDLArModule0RawInput")
    << "HDF5 file " << filename << " with run number: " <<
    fHDFFile->runNumber << std::endl;

  fb = new art::FileBlock(art::FileFormatVersion(1, "RawEvent2011"),
                          filename);
}

bool dune::NDLArModule0RawInputDetail::readNext(art::RunPrincipal const* const inR,
                                                art::SubRunPrincipal const* const inSR,
                                                art::RunPrincipal*& outR,
                                                art::SubRunPrincipal*& outSR,
                                                art::EventPrincipal*& outE) 
{
  using namespace dune::HDF5Utils;

  art::ServiceHandle<dune::NDLArModule0ChannelMapService> cmap;

  // Establish default 'results'
  outR = 0;
  outSR = 0;
  outE = 0;

  size_t run_id = fCurRun;

  //Where to get event number?  Event number just starts counting at 1 for each input file,
  // and the run number increments when the file is closed.  That way the config run number
  // is used for the first file.

  fCurEvent++;

  // decode messages and make RawPixel data products

  dunedaq::detdataformats::pacman::PACMANFrame pmf;
  std::unique_ptr<std::vector<raw::RawPixel>> oPixels( new std::vector<raw::RawPixel> );
  std::unique_ptr<std::vector<raw::Module0Trigger>> oTriggers( new std::vector<raw::Module0Trigger> );

  // assume the current message is left over from the previous event and is the message number of its trigger
  // starting from the current message, find the first trigger word that is at least fConfigNTickTriggerReadout from the last trigger time

  size_t foundtrigmessage = 0;

  // ltsc's purpose is to allow for the rollover of the timestamp counter.  Want to look for rollbacks of the timestamp that are
  // bigger than a triggered event.

  uint32_t ltsc = 0;
  if (fCurTrigTS >= fConfigNTickTriggerReadout)
    {
      ltsc = fCurTrigTS - fConfigNTickTriggerReadout;
    }
  for (size_t iMessage = fCurMessage + 1; iMessage < fNMessages; ++iMessage)
    {
      void *messageptr = fRData[iMessage].p;
      dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageHeader *hdr = pmf.get_msg_header(messageptr);
      dunedaq::detdataformats::pacman::PACMANFrame::msg_type mtype = hdr->type;
      if (fLogLevel > 1)
        {
          std::cout << "PACMAN message[" << iMessage << "] nwords: " << hdr->words << " message type: " << mtype << std::endl;
        }

      if (mtype == dunedaq::detdataformats::pacman::PACMANFrame::msg_type::DATA_MSG)
        {
          //std::cout << "unpacking data words" << std::endl;
          for (size_t iword = 0; iword < hdr->words; ++iword)
            {
              dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageWord* mwp = pmf.get_msg_word(messageptr,iword);
              if ( mwp->word.type == dunedaq::detdataformats::pacman::PACMANFrame::word_type::TRIG_WORD )
                {
                  uint32_t TrigBits =  mwp->word._null[0];
                  uint32_t TrigTS = (mwp->word._null[6] << 24) + (mwp->word._null[5] << 16) + (mwp->word._null[4] << 8) + mwp->word._null[3];
		  if (TrigTS == 0 || TrigTS > 10200000) continue; // discard buggy timestamps
                  uint8_t  IO_Group = fIogBuff.at(iMessage).iog;

		  // start a new event if:  the new trigger is long enough after the previous one,
		  // or if the clock has rolled around, or if we haven't seen a trigger yet

		  if (TrigTS > fCurTrigTS + fConfigNTickNextTriggerGap || TrigTS < ltsc || fCurTrigTS == 0)
		    {
		      foundtrigmessage = iMessage;
		      fCurTrigTS = TrigTS;
                      if (fLogLevel > 0)
                        {
                          std::cout << "NDLArModule0Source: Found an event trigger: " << TrigTS << " Trig Bits: " << TrigBits << " IO_Group: " << (int) IO_Group << " " << iMessage << " " << fCurMessage << std::endl;
                        }
		      break;
		    }
                }
	      if (foundtrigmessage) break;
	    }
	}
      if (foundtrigmessage) break;
    }

  if (!foundtrigmessage)  // didn't find any more triggers 
    {
      return false;
    }
  fCurMessage = foundtrigmessage;

  size_t lowmessage = 0;
  if (fConfigMaxMessageLookBack <= foundtrigmessage)
    {
      lowmessage = foundtrigmessage - fConfigMaxMessageLookBack; 
    }

  // loop through range of messages and put everything in the event that is between fCurTrigTS and fCurTrigTS + fConfigNTickTriggerReadout
  // discard zero timestamps and timestamps > 1.02E7

  size_t lastmessageinevent = foundtrigmessage;

  for (size_t iMessage = lowmessage; iMessage < fNMessages; ++iMessage)
    {
      //std::cout << "check if done: " << iMessage << " " << lastmessageinevent << " " << fConfigMaxMessageLookBack << std::endl;
      if (iMessage > lastmessageinevent + fConfigMaxMessageLookBack) break;   // look past the last message in the event for some more out-of-order words

      void *messageptr = fRData[iMessage].p;
      dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageHeader *hdr = pmf.get_msg_header(messageptr);
      
      dunedaq::detdataformats::pacman::PACMANFrame::msg_type mtype = hdr->type;
      if (fLogLevel > 1)
	{
	  std::cout << "PACMAN message[" << iMessage << "] nwords: " << hdr->words << " message type: " << mtype << std::endl;
	}

      if (mtype == dunedaq::detdataformats::pacman::PACMANFrame::msg_type::DATA_MSG)
	{
	  for (size_t iword = 0; iword < hdr->words; ++iword)
	    {
	      dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageWord* mwp = pmf.get_msg_word(messageptr,iword);
	      if ( mwp->word.type == dunedaq::detdataformats::pacman::PACMANFrame::word_type::TRIG_WORD )
		{
		  uint32_t TrigBits =  mwp->word._null[0];
		  uint32_t TrigTS = (mwp->word._null[6] << 24) + (mwp->word._null[5] << 16) + (mwp->word._null[4] << 8) + mwp->word._null[3];
		  uint8_t  IO_Group = fIogBuff.at(iMessage).iog;
		  fHist_tts->Fill(TrigTS);
		  if (TrigTS == 0 || TrigTS > 10200000) continue; // discard buggy timestamps

		  if (fLogLevel > 1)
		    {
		      std::cout << "NDLArModule0Source: Found a trigger: " << TrigTS << " Trigger bits: " << (int) TrigBits << " IO_Group: " << (int) IO_Group << std::endl;
		    }

		  if (TrigTS >= fCurTrigTS && TrigTS < fCurTrigTS + fConfigNTickTriggerReadout)
		    {
		      oTriggers->emplace_back(IO_Group, TrigBits, TrigTS);
		      lastmessageinevent = iMessage;
		    }
		}


	      if (mwp->word.type == dunedaq::detdataformats::pacman::PACMANFrame::word_type::DATA_WORD)
		{
		  if (fLogLevel > 1)
		    {
		      std::cout << "word[" << iword << "] type: " 
				<< mwp->data_word.type 
				<< " uart chan: " << (int) mwp->data_word.channel_id 
				<< " chipid: " << mwp->data_word.larpix_word.data_packet.chipid 
				<< " channelid: " << mwp->data_word.larpix_word.data_packet.channelid 
				<< " adc: " << mwp->data_word.larpix_word.data_packet.dataword 
				<< " timestamp: " << mwp->data_word.larpix_word.data_packet.timestamp 
				<< " trigger_type: " << mwp->data_word.larpix_word.data_packet.trigger_type 
				<< " io_group: " << (int) fIogBuff.at(iMessage).iog
				<< std::endl;
		    }

		  uint32_t dataTS = mwp->data_word.larpix_word.data_packet.timestamp;
		  fHist_dts->Fill(dataTS);
		  if (dataTS == 0 || dataTS > 10200000) continue;

		  double deltaTS = (double) dataTS - (double) fCurTrigTS;
		  fHist_deltats->Fill(deltaTS);

		  if (dataTS >= fCurTrigTS && dataTS < fCurTrigTS + fConfigNTickTriggerReadout)
		    {
		      lastmessageinevent = iMessage;
		      auto cinfo = cmap->GetChanInfoFromElectronics(
								    fIogBuff.at(iMessage).iog,
								    mwp->data_word.channel_id,
								    mwp->data_word.larpix_word.data_packet.chipid,
								    mwp->data_word.larpix_word.data_packet.channelid);
		      raw::ChannelID_t chan = 0;
		      if (cinfo.valid)
			{
			  chan = cinfo.offlinechan;
			}
		      if (fLogLevel > 1)
			{
			  std::cout << "Channel map check: " << (int) fIogBuff.at(iMessage).iog << " " <<
			    (int) mwp->data_word.channel_id << " " << mwp->data_word.larpix_word.data_packet.chipid << " " 
				    << mwp->data_word.larpix_word.data_packet.channelid << " " << chan << std::endl;
			}

		      int adc = mwp->data_word.larpix_word.data_packet.dataword;  // it's a uint16_t in the message, and a short in rawpixel
		      uint32_t ts = mwp->data_word.larpix_word.data_packet.timestamp;
		      oPixels->emplace_back(chan, adc, ts );
		    }
		}
	    }    
	}
    }      

  uint64_t getTrigTime = formatTrigTimeStamp (fCurTrigTS);
  if (fLogLevel > 0)
    {
      std::cout << "NDLArModule0Source: getTrigTime :" << getTrigTime << std::endl;
    }

  art::Timestamp artTrigStamp (getTrigTime);
  if (fLogLevel > 0)
    {
      std::cout << "artTrigStamp :" << artTrigStamp.value() << std::endl;
    }

  // make new run if inR is 0 or if the run has changed
  if (inR == 0 || inR->run() != run_id) {
    outR = pmaker.makeRunPrincipal(run_id,artTrigStamp);
  }

  // make new subrun if inSR is 0 or if the subrun has changed
  art::SubRunID subrun_check(run_id, 1);
  if (inSR == 0 || subrun_check != inSR->subRunID()) {
    outSR = pmaker.makeSubRunPrincipal(run_id, 1, artTrigStamp);
  }
  outE = pmaker.makeEventPrincipal(run_id, 1, fCurEvent, artTrigStamp);

  put_product_in_principal(std::move(oPixels), *outE, pretend_module_name,"");
  put_product_in_principal(std::move(oTriggers), *outE, pretend_module_name,"");

  return true;
}

//typedef for shorthand
namespace dune {
  using NDLArModule0RawInputSource = art::Source<NDLArModule0RawInputDetail>;
}

DEFINE_ART_INPUT_SOURCE(dune::NDLArModule0RawInputSource)
