///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       NDLArModule0ChannelMapService
// Module type: service
// File:        NDLArModule0ChannelMapService_service.cc
// Author:      Tom Junk, August 2022
//
// Implementation of hardware-offline channel mapping for the ND-LAr Module 0 prototype.  Read a 
// de-yamlified version of module0_pixel_layout_2.2.16.yaml.  Use it to create an in-memory representation
// of the channel positions and electronics numbers.

// This service calls methods in NDLArModule0ChannelMapSP.cxx, the service provider.  This service is meant to
// satisfy the framework interface, while the service provider does the actual work 
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "NDLArModule0ChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

dune::NDLArModule0ChannelMapService::NDLArModule0ChannelMapService(fhicl::ParameterSet const& pset) {

  std::string channelMapFile = pset.get<std::string>("ChannelMapFile","module0_pixel_layout_2.2.16.dat");
  std::vector<double> dv;
  dv.push_back(0.0); // in cm
  dv.push_back(0.0);
  fAnodeXOffset = pset.get<std::vector<double>>("AnodeXOffset",dv);  // X location of anode planes in cm, indexed by io_group-1
  fAnodeYOffset = pset.get<std::vector<double>>("AnodeYOffset",dv); // Y offsets of anode planes in cm, indexed by io_group-1
  fAnodeZOffset = pset.get<std::vector<double>>("AnodeZOffset",dv); // Y offsets of anode planes in cm, indexed by io_group-1

  std::string chanmapfullname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(channelMapFile, chanmapfullname);

  if (chanmapfullname.empty()) {
    throw cet::exception("NDLArModule0ChannelMapService") << "Input channel map file " << channelMapFile << " not found" << std::endl;
  }
  
  MF_LOG_INFO("NDLArModule0ChannelMapService") << "Building NDLArModule0 pixel map from file " << channelMapFile << std::endl;

  fModule0ChanMap.ReadMapFromFile(chanmapfullname, fAnodeXOffset, fAnodeYOffset, fAnodeZOffset);
}

dune::NDLArModule0ChannelMapService::NDLArModule0ChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) : NDLArModule0ChannelMapService(pset) {
}

dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t dune::NDLArModule0ChannelMapService::GetChanInfoFromElectronics(
    unsigned int io_group,
    unsigned int io_channel,
    unsigned int chip,
    unsigned int chipchannel ) const {

  return fModule0ChanMap.GetChanInfoFromElectronics(io_group, io_channel, chip, chipchannel);
}


dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t dune::NDLArModule0ChannelMapService::GetChanInfoFromOfflChan(unsigned int offlineChannel) const {

  return fModule0ChanMap.GetChanInfoFromOfflChan(offlineChannel);

}

dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t dune::NDLArModule0ChannelMapService::GetChanInfoFromXYZ(double x, double y, double z) const {
  return GetChanInfoFromXYZ(x, y, z);
}

std::vector<dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t> dune::NDLArModule0ChannelMapService::GetChanInfoFromXYZWithNeighbors(double x, double y, double z, double r) const {
  return GetChanInfoFromXYZWithNeighbors(x, y, z, r);
}

DEFINE_ART_SERVICE(dune::NDLArModule0ChannelMapService)
