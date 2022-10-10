////////////////////////////////////////////////////////////////////////
// Class:       SPDump
// Plugin Type: analyzer (Unknown Unknown)
// File:        SPDump_module.cc
//
// Generated at Sun Jun 26 15:23:13 2022 by Tingjun Yang using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art_root_io/TFileService.h"
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/PointCharge.h"

#include "TTree.h"
#include "TFile.h"
#include "TGeoManager.h"

#include <vector>
#include <string>

using namespace std;

namespace dunend {
  class SPDump;
}


class dunend::SPDump : public art::EDAnalyzer {
public:
  explicit SPDump(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  SPDump(SPDump const&) = delete;
  SPDump(SPDump&&) = delete;
  SPDump& operator=(SPDump const&) = delete;
  SPDump& operator=(SPDump&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  // Selected optional functions.
  void beginJob() override;

private:

  const art::InputTag fSpacePointModuleLabel;
  const string        fGeometryFileName;

  TTree *fTree;
  // Run information
  int run;
  int subrun;
  int event;

  // Space Point information
  vector<float> x;             //x coordinate
  vector<float> y;             //y coordinate
  vector<float> z;             //z coordinate
  vector<float> charge;        //charge

  void reset();

};


dunend::SPDump::SPDump(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  ,
  fSpacePointModuleLabel(p.get<art::InputTag>("SpacePointModuleLabel")),
  fGeometryFileName(p.get<string>("GeometryFileName"))
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void dunend::SPDump::analyze(art::Event const& e)
{
  reset();

  run = e.run();
  subrun = e.subRun();
  event = e.id().event();

  // * Space Point information
  std::vector<art::Ptr<recob::SpacePoint> > splist;
  auto spListHandle = e.getHandle< std::vector<recob::SpacePoint> >(fSpacePointModuleLabel);
  if (spListHandle)
    art::fill_ptr_vector(splist, spListHandle);

  // * Space Point information
  std::vector<art::Ptr<recob::PointCharge> > pclist;
  auto pcListHandle = e.getHandle< std::vector<recob::PointCharge> >(fSpacePointModuleLabel);
  if (pcListHandle)
    art::fill_ptr_vector(pclist, pcListHandle);

  //for (auto & sp: splist){
  for (size_t i = 0; i<splist.size(); ++i){
    x.push_back(splist[i]->XYZ()[0]);
    y.push_back(splist[i]->XYZ()[1]);
    z.push_back(splist[i]->XYZ()[2]);
    charge.push_back(pclist[i]->charge());
  }

  fTree->Fill();
}

void dunend::SPDump::beginJob()
{

  art::ServiceHandle<geo::Geometry> geom;
  TGeoManager *geoman = geom->ROOTGeoManager();
  TFile *f = TFile::Open(fGeometryFileName.c_str(), "recreate");
  geoman->Write("EDepSimGeometry");
  f->Close();

  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("sp","space point tree");
  fTree->Branch("run",&run,"run/I");
  fTree->Branch("subrun",&subrun,"subrun/I");
  fTree->Branch("event",&event,"event/I");

  fTree->Branch("x", &x);
  fTree->Branch("y", &y);
  fTree->Branch("z", &z);
  fTree->Branch("charge", &charge);
}

void dunend::SPDump::reset(){

  run = -1;
  subrun = -1;
  event = -1;
  x.clear();
  y.clear();
  z.clear();
  charge.clear();
}

DEFINE_ART_MODULE(dunend::SPDump)
