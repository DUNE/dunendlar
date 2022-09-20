#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "NDLArModule0RawInput.h"
#include "dunecore/DuneObj/DUNEHDF5FileInfo.h"
#include "dunendlar/DUNENDLArObj/RawPixel.h"
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
  fNMessagesPerEvent = ps.get<int>("NMessagesPerEvent",1000);  // number of messages per event
  rh.reconstitutes<std::vector<raw::RawPixel>, art::InEvent>(pretend_module_name);
}

void dune::NDLArModule0RawInputDetail::readFile(
                                                std::string const & filename, art::FileBlock*& fb) {
  fHDFFile = dune::HDF5Utils::openFile(filename);
  fHDFFile->runNumber = fCurRun;
  fCurEvent = 0; 

  // read all the io_groups into memory.  Only reason to do this is because we need to read all the
  // messages into memory at the same time too.  The i/o groups take less space.

  hid_t headerdsid = H5Dopen(fHDFFile->filePtr,"msg_headers",H5P_DEFAULT);
  hid_t headerspaceid = H5Dget_space(headerdsid);
  hsize_t headerndims = H5Sget_simple_extent_ndims(headerspaceid);
  //std::cout << " msg_headers dataspace ndims: " << headerndims << std::endl;
  std::vector<hsize_t> headerdims(headerndims);
  std::vector<hsize_t> headermaxdims(headerndims);
  H5Sget_simple_extent_dims(headerspaceid,headerdims.data(),headermaxdims.data());
  //std::cout << " msg_headers dims: " << headerdims[0] << std::endl;
  //std::cout << " msg_headers maxdims: " << headermaxdims[0] << std::endl;

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
  fNEvents = fNMessages/fNMessagesPerEvent;
  H5Dclose(datasetid);

  if (fIogBuff.size() != dims[0])
    {
      throw cet::exception("NDLArModule0RawInput") << "Different numbers of io_groups and messages " <<
        fIogBuff.size() << " != " << dims[0]; 
    }

  MF_LOG_INFO("NDLArModule0RawInput")
    << "HDF5 file " << filename << " with run number: " <<
    fHDFFile->runNumber  << " and " <<
    fNEvents << " events of length " << fNMessagesPerEvent << " messages.";
  size_t rdr = fNMessages % fNMessagesPerEvent; 
  if ( rdr != 0)
    {
      MF_LOG_INFO("NDLArModule0RawInput") << "And one event with " << rdr << " messages.";
    }

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

  if (fCurEvent * fNMessagesPerEvent >= fNMessages) 
    {
      return false;
    }

  size_t run_id = fCurRun;

  //Where to get event number?  Event number just starts counting at 1.

  fCurEvent++;

  // decode messages and make RawPixel data products

  // trigTimeStamp is NOT time but Clock-tick since the epoch.
  uint64_t trigTimeStamp = 0;   // get time stamp from the first message being decoded

  dunedaq::detdataformats::pacman::PACMANFrame pmf;
  std::unique_ptr<std::vector<raw::RawPixel>> oPixels( new std::vector<raw::RawPixel> );
  size_t imlow = fNMessagesPerEvent * (fCurEvent - 1);
  size_t imhighp = fNMessagesPerEvent * (fCurEvent);  // high index plus 1
  if (imhighp > fNMessages)
    {
      imhighp = fNMessages;   // last event is short, just pick up the remainder.
    }
  for (size_t imessage = imlow; imessage < imhighp; ++imessage)
    {
      void *messageptr = fRData[imessage].p;
      dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageHeader *hdr = pmf.get_msg_header(messageptr);
      
      dunedaq::detdataformats::pacman::PACMANFrame::msg_type mtype = hdr->type;
      //std::cout << "PACMAN message[" << imessage << "] nwords: " << hdr->words << " message type: " << mtype << std::endl;

      if (mtype == dunedaq::detdataformats::pacman::PACMANFrame::msg_type::DATA_MSG)
        {
          //std::cout << "unpacking data words" << std::endl;
          for (size_t iword = 0; iword<hdr->words; ++iword)
            {
              dunedaq::detdataformats::pacman::PACMANFrame::PACMANMessageWord* mwp = pmf.get_msg_word(messageptr,iword);
              //std::cout << "word[" << iword << "] type: " 
              //          << mwp->data_word.type 
              //          << " uart chan: " << (int) mwp->data_word.channel_id 
              //          << " chipid: " << mwp->data_word.larpix_word.data_packet.chipid 
              //          << " channelid: " << mwp->data_word.larpix_word.data_packet.channelid 
              //          << " adc: " << mwp->data_word.larpix_word.data_packet.dataword 
              //          << " timestamp: " << mwp->data_word.larpix_word.data_packet.timestamp 
              //          << " trigger_type: " << mwp->data_word.larpix_word.data_packet.trigger_type 
              //          << " io_group: " << (int) fIogBuff.at(imessage).iog
              //          << std::endl;
              raw::ChannelID_t chan = fIogBuff.at(imessage).iog +       // to do:  map channels
                (int) mwp->data_word.channel_id + 
                mwp->data_word.larpix_word.data_packet.chipid + 
                mwp->data_word.larpix_word.data_packet.channelid;
              int adc = mwp->data_word.larpix_word.data_packet.dataword;
              unsigned int ts = mwp->data_word.larpix_word.data_packet.timestamp;
              if (trigTimeStamp == 0) 
                {
                  trigTimeStamp = ts;
                }
              uint32_t ts32 = ts & 0xFFFFFFFF;
              oPixels->emplace_back(chan, adc, ts32 );
            }
        }
    }

  // this format of the timestamp is almost certainly the wrong thing to do for now.

  uint64_t getTrigTime = formatTrigTimeStamp (trigTimeStamp);
  // std::cout << "getTrigTime :" << getTrigTime << std::endl;
  
  art::Timestamp artTrigStamp (getTrigTime);
  // std::cout << "artTrigStamp :" << artTrigStamp.value() << std::endl;

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
  //std::cout << "Event Time Stamp :" << event.time() << std::endl;
 

  put_product_in_principal(std::move(oPixels), *outE, pretend_module_name,"");

  return true;
}

//typedef for shorthand
namespace dune {
  using NDLArModule0RawInputSource = art::Source<NDLArModule0RawInputDetail>;
}


DEFINE_ART_INPUT_SOURCE(dune::NDLArModule0RawInputSource)
