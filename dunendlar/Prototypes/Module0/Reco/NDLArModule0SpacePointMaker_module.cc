////////////////////////////////////////////////////////////////////////
// Class:       NDLArModule0SpacePointMaker
// Plugin Type: producer (Unknown Unknown)
// File:        NDLArModule0SpacePointMaker_module.cc
//
// Generated at Tue Sep 27 17:41:12 2022 by Thomas Junk using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

#include "dunendlar/DUNENDLArObj/RawPixel.h"
#include "dunendlar/DUNENDLArObj/Module0Trigger.h"
#include "dunendlar/Prototypes/Module0/ChannelMap/NDLArModule0ChannelMapService.h"
#include "lardataobj/RecoBase/SpacePoint.h"

class NDLArModule0SpacePointMaker;


class NDLArModule0SpacePointMaker : public art::EDProducer {
public:
  explicit NDLArModule0SpacePointMaker(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  NDLArModule0SpacePointMaker(NDLArModule0SpacePointMaker const&) = delete;
  NDLArModule0SpacePointMaker(NDLArModule0SpacePointMaker&&) = delete;
  NDLArModule0SpacePointMaker& operator=(NDLArModule0SpacePointMaker const&) = delete;
  NDLArModule0SpacePointMaker& operator=(NDLArModule0SpacePointMaker&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;

private:

  std::string fRawPixelLabel;
  double fDriftVel; // drift velocity -- cm per tick

};


NDLArModule0SpacePointMaker::NDLArModule0SpacePointMaker(fhicl::ParameterSet const& p)
  : EDProducer{p}  // ,
  // More initializers here.
{
  fRawPixelLabel = p.get<std::string>("RawPixelLabel","daq");
  fDriftVel = p.get<double>("DriftVel",0.0081);  // cm per tick.  1 tick = 0.1 microsecond
  consumes<std::vector<raw::RawPixel>>(fRawPixelLabel);
  consumes<std::vector<raw::Module0Trigger>>(fRawPixelLabel);
  produces<std::vector<recob::SpacePoint>>();
}

void NDLArModule0SpacePointMaker::produce(art::Event& e)
{
  art::ServiceHandle<dune::NDLArModule0ChannelMapService> cmap;

  std::unique_ptr<std::vector<recob::SpacePoint>> spacepoints(new std::vector<recob::SpacePoint>);

  auto rawpixhandle = e.getValidHandle<std::vector<raw::RawPixel>>(fRawPixelLabel);
  auto rawtrighandle = e.getValidHandle<std::vector<raw::Module0Trigger>>(fRawPixelLabel);

  if (rawtrighandle->size() > 0)
    {
      uint32_t trigtime = 0;
      trigtime = rawtrighandle->at(0).GetTimeStamp();
      for (size_t ipix=0; ipix < rawpixhandle->size(); ++ipix)
	{
	  auto &rpx = rawpixhandle->at(ipix);
	  auto cinfo = cmap->GetChanInfoFromOfflChan(rpx.GetChannel());
	  double xyz[3];
	  xyz[0] = cinfo.xyz[0];
	  xyz[1] = cinfo.xyz[1];
	  xyz[2] = cinfo.xyz[2];
	  double dt = rpx.GetTimeStamp() - trigtime;
	  double driftdist = fDriftVel * dt;
	  if (xyz[0] > 0)
	    {
	      xyz[0] -= driftdist;
	    }
	  else
	    {
	      xyz[0] += driftdist;
	    }
	  Double32_t xv32[3];
	  Double32_t xe32[6];
	  recob::SpacePoint::ID_t id = ipix;
	  Double32_t chisq = 0;
	  xv32[0] = xyz[0];
	  xv32[1] = xyz[1];
	  xv32[2] = xyz[2];
	  for (size_t i=0; i<6; ++i)
	    {
	      xe32[i] = 0;
	    }
	  spacepoints->emplace_back(xv32,xe32,chisq,id);
	}
    }

  e.put(std::move(spacepoints));

}

DEFINE_ART_MODULE(NDLArModule0SpacePointMaker)
