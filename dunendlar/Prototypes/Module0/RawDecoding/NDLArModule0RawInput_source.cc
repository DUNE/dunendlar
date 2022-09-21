#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "NDLArModule0RawInput.h"
#include "dunecore/DuneObj/DUNEHDF5FileInfo.h"
#include "dunendlar/DUNENDLArObj/RawPixel.h"
#include "dunendlar/DUNENDLArObj/Module0Trigger.h"
#include "detdataformats/pacman/PACMANFrame.hpp"

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
  rh.reconstitutes<std::vector<raw::RawPixel>, art::InEvent>(pretend_module_name);
  rh.reconstitutes<std::vector<raw::Module0Trigger>, art::InEvent>(pretend_module_name);
}

void dune::NDLArModule0RawInputDetail::readFile(
                                                std::string const & filename, art::FileBlock*& fb) {
  fHDFFile = dune::HDF5Utils::openFile(filename);
  fHDFFile->runNumber = fCurRun;
  fCurEvent = 0;
  fCurMessage = 0;
  fCurTrigTS = 0;
  fLastTrigTS = 0;

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
  
  // Establish default 'results'
  outR = 0;
  outSR = 0;
  outE = 0;

  // this also covers the case where fNMessages is zero. 

  if (fCurMessage >= fNMessages) 
    {
      return false;
    }

  size_t run_id = fCurRun;

  //Where to get event number?  Event number just starts counting at 1 for each input file,
  // and the run number increments when the file is closed.  That way the config run number
  // is used for the first file.

  fCurEvent++;

  // decode messages and make RawPixel data products

  dunedaq::detdataformats::pacman::PACMANFrame pmf;
  std::unique_ptr<std::vector<raw::RawPixel>> oPixels( new std::vector<raw::RawPixel> );
  std::unique_ptr<std::vector<raw::Module0Trigger>> oTriggers( new std::vector<raw::Module0Trigger> );

  oTriggers->emplace_back(fLastTrigBits, fLastTrigTS);

  while (true)
    {
      void *messageptr = fRData[fCurMessage].p;
      dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageHeader *hdr = pmf.get_msg_header(messageptr);
      
      dunedaq::detdataformats::pacman::PACMANFrame::msg_type mtype = hdr->type;
      if (fLogLevel > 1)
        {
          std::cout << "PACMAN message[" << fCurMessage << "] nwords: " << hdr->words << " message type: " << mtype << std::endl;
        }
      if (mtype == dunedaq::detdataformats::pacman::PACMANFrame::msg_type::DATA_MSG)
        {
          //std::cout << "unpacking data words" << std::endl;
          for (size_t iword = 0; iword < hdr->words; ++iword)
            {
              dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageWord* mwp = pmf.get_msg_word(messageptr,iword);

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
                                << " io_group: " << (int) fIogBuff.at(fCurMessage).iog
                                << std::endl;
                    }

                  raw::ChannelID_t chan = fIogBuff.at(fCurMessage).iog +       // to do:  map channels
                    (int) mwp->data_word.channel_id + 
                    mwp->data_word.larpix_word.data_packet.chipid + 
                    mwp->data_word.larpix_word.data_packet.channelid;

                  int adc = mwp->data_word.larpix_word.data_packet.dataword;  // it's a uint16_t in the message, and a short in rawpixel
                  uint32_t ts = mwp->data_word.larpix_word.data_packet.timestamp;
                  oPixels->emplace_back(chan, adc, ts );
                }

              if ( mwp->word.type == dunedaq::detdataformats::pacman::PACMANFrame::word_type::TRIG_WORD )
                {
		  fLastTrigBits = fCurTrigBits;
                  fCurTrigBits =  mwp->word._null[0];
                  fLastTrigTS = fCurTrigTS;
                  fCurTrigTS = (mwp->word._null[6] << 24) + (mwp->word._null[5] << 16) + (mwp->word._null[4] << 8) + mwp->word._null[3];
                  if (fLogLevel > 0)
                    {
                      std::cout << "NDLArModule0Source: Found a trigger: " << fCurTrigTS << " Trigger bits: " << (int) fCurTrigBits << std::endl;
                    }

		  // If we see multiple trigger words, write them all to the output stream.  If there is pixel data between trigger words,
		  // then start a new event.

		  if (oPixels->size() == 0)
		    {
                       oTriggers->emplace_back(fCurTrigBits, fCurTrigTS);
		    }
                }
                  
              // determine whether to finish up the event.  Either we have a new trigger timestamp or we ran out of data

              if ( (fCurTrigTS != fLastTrigTS || fCurMessage + 1 >= fNMessages) && oPixels->size() > 0 )
                {
                  // this format of the timestamp is almost certainly the wrong thing to do for now.

                  uint64_t getTrigTime = formatTrigTimeStamp (fLastTrigTS);
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

                  fLastTrigTS = fCurTrigTS;
                  fCurMessage++;

                  return true;
                }

              // get the trigger timestamp if we are reading a trigger word.

              // to do -- do we subtract the trigger time from the data timestamp?  What about data in the file that
              // come before the first trigger data word?
            }
        }
      fCurMessage++;
    }
}

//typedef for shorthand
namespace dune {
  using NDLArModule0RawInputSource = art::Source<NDLArModule0RawInputDetail>;
}

DEFINE_ART_INPUT_SOURCE(dune::NDLArModule0RawInputSource)
