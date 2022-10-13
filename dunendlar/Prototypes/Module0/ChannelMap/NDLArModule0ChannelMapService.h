///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       NDLArModule0ChannelMapService
// Module type: service
// File:        NDLArModule0ChannelMapService.h
// Author:      Tom Junk, September 26, 2022
//
// Implementation of hardware-offline channel mapping for the ND-LAr Module 0 prototype
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NDLArModule0ChannelMapService_H
#define NDLArModule0ChannelMapService_H

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "NDLArModule0ChannelMapSP.h"

namespace dune {
  class NDLArModule0ChannelMapService;
}

class dune::NDLArModule0ChannelMapService {

public:

  NDLArModule0ChannelMapService(fhicl::ParameterSet const& pset);
  NDLArModule0ChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&);

  dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t GetChanInfoFromElectronics(
   unsigned int io_group,
   unsigned int io_channel,
   unsigned int chip,
   unsigned int chipchannel) const;

  dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t GetChanInfoFromOfflChan(unsigned int offlchan) const;

  dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t GetChanInfoFromXYZ(double x, double y, double z) const;

  std::vector<dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t> GetChanInfoFromXYZWithNeighbors(double x, double y, double z, double r) const;

private:

  dune::NDLArModule0ChannelMapSP fModule0ChanMap;

  std::vector<double> fAnodeXOffset;
  std::vector<double> fAnodeYOffset;
  std::vector<double> fAnodeZOffset;

};

DECLARE_ART_SERVICE(dune::NDLArModule0ChannelMapService, LEGACY)

#endif
