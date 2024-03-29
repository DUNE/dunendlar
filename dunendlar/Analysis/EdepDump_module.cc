////////////////////////////////////////////////////////////////////////
// Class:       EdepDump
// Plugin Type: analyzer (Unknown Unknown)
// File:        EdepDump_module.cc
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
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
#include "lardataobj/Simulation/GeneratedParticleInfo.h"
#include "larcore/Geometry/Geometry.h"

#include "TTree.h"
#include "TFile.h"
#include "TGeoManager.h"

#include "TG4Event.h"
#include "TG4HitSegment.h"
#include "TG4PrimaryVertex.h"
#include "TG4Trajectory.h"
//#include "EDepSimSegmentSD.cc"

#include "edep_hdf5_utils.h"
#include "edep_calc_utils.h"


#include <vector>
#include <string>

using namespace std;
using namespace edep_hdf5_utils;

namespace dunend {
  class EdepDump;
}



class dunend::EdepDump : public art::EDAnalyzer {
public:
  explicit EdepDump(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  EdepDump(EdepDump const&) = delete;
  EdepDump(EdepDump&&) = delete;
  EdepDump& operator=(EdepDump const&) = delete;
  EdepDump& operator=(EdepDump&&) = delete;

  virtual ~EdepDump() noexcept {};

  // Required functions.
  void analyze(art::Event const& e) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:


  int fDeltaTrackID;

  int fLogLevel;

  float fMaxSagitta;
  float fMaxSeparation;
  float fMaxLength;
  bool fSetEdepSimPositionToMM;
  bool fSetEdepSimEnergyToGeV;
  bool fMergeSteps;

  float DistanceConversion(){ return (fSetEdepSimPositionToMM)? 10: 1;};
  float EnergyConversion(){ return (fSetEdepSimEnergyToGeV)? 0.001: 1;};

  const art::InputTag fGenieGenModuleLabel;
  const art::InputTag fGeantModuleLabel;
  vector<art::InputTag> fSEDModuleLabels;


  string fFileName;
  TFile *outFile;
  TTree *eventTree;
  TG4Event *tg4event;
  void FillTG4Event( art::Event const &e );
  TG4PrimaryParticle ConvertParticle( const simb::MCParticle &particle );

  //SedList --> TG4HitSegmentContainer
  std::vector<edep_utils::NDHitSegment> CombineSEDToSegment(
    std::vector<art::Ptr<sim::SimEnergyDeposit>> sedlist,
    float maxSagitta, float maxSeparation, float maxLength );

  //a new implementation
  std::vector<edep_utils::NDHitSegment> CombineSEDToSegment2(
    std::vector<art::Ptr<sim::SimEnergyDeposit>> &sedlist,
    float maxSagitta, float maxSeparation, float maxLength );

  std::vector<sim::SimEnergyDeposit> ConvertSegmentToSED( 
      std::vector<edep_utils::NDHitSegment> &segments
      );

  std::vector<edep_utils::NDHitSegment> ConvertSEDToNDHitSegment( 
      std::vector<art::Ptr<sim::SimEnergyDeposit>> &sedlist
      );


  std::map<std::string, std::vector<sim::SimEnergyDeposit> > combined_sedlists;
  std::map<std::string, std::vector<edep_utils::NDHitSegment> > combined_segments;

  void FillTree(art::Event const&e);
  void FillSingleMCP(art::Ptr<simb::MCParticle> &mcp);
  void FillSingleSED(art::Ptr<sim::SimEnergyDeposit> &sed, std::string det);
  void FillSingleNDHitSegment(edep_utils::NDHitSegment &seg, std::string det);

  TTree *fTree;
  // Run information
  int run;
  int subrun;
  int event;

  // Genie information
  vector<int> nuPDG;            //Neutrino PDG code
  vector<int> ccnc;             //0: CC; 1: NC
  vector<int> mode;             //0: QE; 1: Resonance; 2: DIS; 3: Coherent pion; 10: MEC
  vector<float> enu;            //Neutrino energy (GeV)
  vector<float> Q2;             //Momentum transfer
  vector<float> W;              //Invariant mass of hadronic system
  vector<float> X;              //Bjorken-X
  vector<float> Y;              //Hadronic-y
  vector<int> hitnuc;           //PDG code of struck nucleon
  vector<int> target;           //PDG code of target
  vector<float> nuvtxx;         //neutrino vertex x (cm)
  vector<float> nuvtxy;         //neutrino vertex y (cm)
  vector<float> nuvtxz;         //neutrino vertex z (cm)
  vector<float> nu_dcosx;       //neutrino direction cosine x
  vector<float> nu_dcosy;       //neutrino direction cosine y
  vector<float> nu_dcosz;       //neutrino direction cosine z
  vector<float> lep_mom;        //lepton momentum (GeV)
  vector<float> lep_dcosx;      //lepton direction cosine x
  vector<float> lep_dcosy;      //lepton direction cosine y
  vector<float> lep_dcosz;      //lepton direction cosine z
  vector<float> t0;             //neutrino interaction time

  // Geant4 MCParticle information
  vector<int>    mcp_id;        //g4 track ID
  vector<int>    mcp_mother;    //mother track ID
  vector<int>    mcp_pdg;       //PDG code
  vector<int>    mcp_nuid;      //GENIE index
  vector<float>  mcp_energy;    //particle energy (GeV)
  vector<float>  mcp_px;        //particle momentum px (GeV/c)
  vector<float>  mcp_py;        //particle momentum py (GeV/c)
  vector<float>  mcp_pz;        //particle momentum pz (GeV/c)
  vector<float>  mcp_startx;    //particle start x (cm)
  vector<float>  mcp_starty;    //particle start y (cm)
  vector<float>  mcp_startz;    //particle start z (cm)
  vector<float>  mcp_endx;      //particle end x (cm)
  vector<float>  mcp_endy;      //particle end y (cm)
  vector<float>  mcp_endz;      //particle end z (cm)

  // SimEnergyDeposit (sed) information vector<float>  sed_startx;    //Edep start x (cm)
  vector<float>  sed_startx;    //Edep start x (cm)
  vector<float>  sed_starty;    //Edep start y (cm)
  vector<float>  sed_startz;    //Edep start z (cm)
  vector<float>  sed_endx;      //Edep end x (cm)
  vector<float>  sed_endy;      //Edep end y (cm)
  vector<float>  sed_endz;      //Edep end z (cm)
  vector<float>  sed_energy;    //energy deposition (MeV)
  vector<float>  sed_sec_energy;//secondary energy deposition (MeV)
  vector<int>    sed_id;        //TrackID index
  vector<int>    sed_pdg;       //PDG code
  vector<string> sed_det;       //Detector name
  vector<int> sed_nelectrons;       //n electrons
  vector<int> sed_nphotons;       //n photons

  // DetectorSegment information 
  vector<float>  seg_startx;    //Edep start x (cm)
  vector<float>  seg_starty;    //Edep start y (cm)
  vector<float>  seg_startz;    //Edep start z (cm)
  vector<float>  seg_endx;      //Edep end x (cm)
  vector<float>  seg_endy;      //Edep end y (cm)
  vector<float>  seg_endz;      //Edep end z (cm)
  vector<float>  seg_energy;    //energy deposition (MeV)
  vector<float>  seg_sec_energy;    //energy deposition (MeV)
  vector<int>    seg_id;        //TrackID index
  vector<int>    seg_pdg;       //PDG code
  vector<string> seg_det;       //Detector name
  vector<int> seg_nelectrons;       //n electrons
  vector<int> seg_nphotons;       //n photons

  void Reset();

  // HDF5 objects
  // https://docs.hdfgroup.org/archive/support/HDF5/Tutor/h5table.html for how to write table
  // Buffer
  std::vector<vertices_dt> vertices_buffer;
  std::vector<trajectories_dt> trajectories_buffer;
  std::vector<segments_dt> segments_buffer;

  hsize_t fBuffSize;
  hsize_t fChunkSize;




  hid_t  hdffile; //file

  //vertices
  hid_t      vertices_field_type[4];
  //trajectories
  hid_t      trajectories_field_type[14];
  hid_t      segments_field_type[24];
  void FillHDF5( art::Event const& e);
  void FlushHDF5Buffer();
  bool CreateNewTable;

  

  herr_t status;
  std::string fHDF5DumpFileName;

};


dunend::EdepDump::EdepDump(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  ,
  fGenieGenModuleLabel(p.get<art::InputTag>("GenieGenModuleLabel")),
  fGeantModuleLabel(p.get<art::InputTag>("GeantModuleLabel")),
  fSEDModuleLabels(p.get<vector<art::InputTag>>("SEDModuleLabels")),
  fBuffSize(p.get<int>("BufferSize",1000)),
  fChunkSize(p.get<int>("ChunkSize",100))
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
  fHDF5DumpFileName      = p.get<std::string>("HDF5NAME", "hdf5out.h5");
  CreateNewTable= true;

  fLogLevel              = p.get<int>("LogLevel", 3 );

  fMergeSteps           = p.get<bool>("DoMergeStep", true);
  // LArSoft TrackID starts at 1, this in incompatible with Edepsim, decrease it by one by default.
  fDeltaTrackID          = p.get<int>("DeltaTrackID",-1);

  fMaxSagitta   = p.get<float>("MaxSagitta", 0.1);
  fMaxSeparation   = p.get<float>("MaxSeparation", 0.1);
  fMaxLength   = p.get<float>("MaxLength", 0.3);
  fSetEdepSimPositionToMM   = p.get<bool>("SetEdepSimPositionToMM", true);
  fSetEdepSimEnergyToGeV   = p.get<bool>("SetEdepEnergyToGeV", true);

  fFileName = p.get<string>("FileName", "edep_dump.root");


  //create array type
  //hsize_t arr_dim3[] = {3};
  //array_type= H5Tarray_create(H5T_NATIVE_FLOAT, 1, arr_dim3);

}


void dunend::EdepDump::analyze(art::Event const& e)
{
  run = e.run();
  subrun = e.subRun();
  event = e.id().event();


  std::cout<<"FillHDF5 -- entering"<<std::endl;
  FillHDF5( e );
  std::cout<<"FillHDF5 -- done"<<std::endl;
  std::cout<<"FillTG4Event -- entering"<<std::endl;
  FillTG4Event( e );
  std::cout<<"FillTG4Event -- done"<<std::endl;

  fTree->Fill();
  Reset();
}

void dunend::EdepDump::beginJob()
{
  art::ServiceHandle<geo::Geometry> geom;

  outFile = new TFile( fFileName.c_str(), "recreate");

  // TGeoManager::UnlockGeometry();
  // TGeoManager::LockDefaultUnits(false);

  TGeoManager *geoman = geom->ROOTGeoManager();

  // TGeoManager::SetDefaultUnits(TGeoManager::kG4Units);
  geoman->Write("EDepSimGeometry");
  // delete geoman;

  // TGeoManager::UnlockGeometry();
  // TGeoManager::LockDefaultUnits(false);
  // TGeoManager::SetDefaultUnits(TGeoManager::kRootUnits);
  // TGeoManager::LockDefaultUnits(true);
  // TGeoManager::LockGeometry();


  eventTree = new TTree("EDepSimEvents", "Energy Deposition for Simulated Events");
  //eventTree = tfs->make<TTree>("EDepSimEvents", "Energy Deposition for Simulated Events");
  tg4event = new TG4Event();
  eventTree->Branch("Event",tg4event);




  //fTree = tfs->make<TTree>("ndsim","ND simulation tree");
  fTree = new TTree("ndsim","ND simulation tree");
  fTree->Branch("run",&run,"run/I");
  fTree->Branch("subrun",&subrun,"subrun/I");
  fTree->Branch("event",&event,"event/I");

  fTree->Branch("nuPDG", &nuPDG);
  fTree->Branch("ccnc", &ccnc);
  fTree->Branch("mode", &mode);
  fTree->Branch("enu", &enu);
  fTree->Branch("Q2", &Q2);
  fTree->Branch("W", &W);
  fTree->Branch("X", &X);
  fTree->Branch("Y", &Y);
  fTree->Branch("hitnuc", &hitnuc);
  fTree->Branch("target", &target);
  fTree->Branch("nuvtxx", &nuvtxx);
  fTree->Branch("nuvtxy", &nuvtxy);
  fTree->Branch("nuvtxz", &nuvtxz);
  fTree->Branch("nu_dcosx", &nu_dcosx);
  fTree->Branch("nu_dcosy", &nu_dcosy);
  fTree->Branch("nu_dcosz", &nu_dcosz);
  fTree->Branch("lep_mom", &lep_mom);
  fTree->Branch("lep_dcosx", &lep_dcosx);
  fTree->Branch("lep_dcosy", &lep_dcosy);
  fTree->Branch("lep_dcosz", &lep_dcosz);
  fTree->Branch("t0", &t0);

  fTree->Branch("mcp_id", &mcp_id);
  fTree->Branch("mcp_mother", &mcp_mother);
  fTree->Branch("mcp_pdg", &mcp_pdg);
  fTree->Branch("mcp_nuid", &mcp_nuid);
  fTree->Branch("mcp_energy", &mcp_energy);
  fTree->Branch("mcp_px", &mcp_px);
  fTree->Branch("mcp_py", &mcp_py);
  fTree->Branch("mcp_pz", &mcp_pz);
  fTree->Branch("mcp_startx", &mcp_startx);
  fTree->Branch("mcp_starty", &mcp_starty);
  fTree->Branch("mcp_startz", &mcp_startz);
  fTree->Branch("mcp_endx", &mcp_endx);
  fTree->Branch("mcp_endy", &mcp_endy);
  fTree->Branch("mcp_endz", &mcp_endz);

  fTree->Branch("sed_startx", &sed_startx);
  fTree->Branch("sed_starty", &sed_starty);
  fTree->Branch("sed_startz", &sed_startz);
  fTree->Branch("sed_endx", &sed_endx);
  fTree->Branch("sed_endy", &sed_endy);
  fTree->Branch("sed_endz", &sed_endz);
  fTree->Branch("sed_energy", &sed_energy);
  fTree->Branch("sed_sec_energy", &sed_sec_energy);
  fTree->Branch("sed_id", &sed_id);
  fTree->Branch("sed_pdg", &sed_pdg);
  fTree->Branch("sed_det", &sed_det);
  fTree->Branch("sed_nelectrons", &sed_nelectrons);
  fTree->Branch("sed_nphotons", &sed_nphotons);

  fTree->Branch("seg_startx", &seg_startx);
  fTree->Branch("seg_starty", &seg_starty);
  fTree->Branch("seg_startz", &seg_startz);
  fTree->Branch("seg_endx", &seg_endx);
  fTree->Branch("seg_endy", &seg_endy);
  fTree->Branch("seg_endz", &seg_endz);
  fTree->Branch("seg_energy", &seg_energy);
  fTree->Branch("seg_sec_energy", &seg_sec_energy);
  fTree->Branch("seg_id", &seg_id);
  fTree->Branch("seg_pdg", &seg_pdg);
  fTree->Branch("seg_det", &seg_det);
  fTree->Branch("seg_nelectrons", &seg_nelectrons);
  fTree->Branch("seg_nphotons", &seg_nphotons);


  //fTree->Branch("segments", &segments);

  //=====================================================================================
  //hdf5
  hdffile = H5Fcreate(fHDF5DumpFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  std::cout<<"hdffile created"<<std::endl;
}

void dunend::EdepDump::endJob()
{
  FlushHDF5Buffer();
  /* close type */
  H5Tclose( vec3_type );
  /* close the file */
  H5Fclose( hdffile );

  //eventTree->Write();
  outFile->Write();
  outFile->Close();
  delete tg4event;
}

void dunend::EdepDump::Reset(){

  combined_sedlists.clear();
  combined_segments.clear();

  tg4event->RunId = -1;
  tg4event->EventId= -1;
  tg4event->Primaries.clear();
  tg4event->Trajectories.clear();
  tg4event->SegmentDetectors.clear();

  run = -1;
  subrun = -1;
  event = -1;
  nuPDG.clear();
  ccnc.clear();
  mode.clear();
  enu.clear();
  Q2.clear();
  W.clear();
  X.clear();
  Y.clear();
  hitnuc.clear();
  target.clear();
  nuvtxx.clear();
  nuvtxy.clear();
  nuvtxz.clear();
  nu_dcosx.clear();
  nu_dcosy.clear();
  nu_dcosz.clear();
  lep_mom.clear();
  lep_dcosx.clear();
  lep_dcosy.clear();
  lep_dcosz.clear();
  t0.clear();
  mcp_id.clear();
  mcp_mother.clear();
  mcp_pdg.clear();
  mcp_nuid.clear();
  mcp_energy.clear();
  mcp_px.clear();
  mcp_py.clear();
  mcp_pz.clear();
  mcp_startx.clear();
  mcp_starty.clear();
  mcp_startz.clear();
  mcp_endx.clear();
  mcp_endy.clear();
  mcp_endz.clear();
  sed_startx.clear();
  sed_starty.clear();
  sed_startz.clear();
  sed_endx.clear();
  sed_endy.clear();
  sed_endz.clear();
  sed_energy.clear();
  sed_sec_energy.clear();
  sed_id.clear();
  sed_pdg.clear();
  sed_det.clear();
  sed_nelectrons.clear();
  sed_nphotons.clear();

  seg_startx.clear();
  seg_starty.clear();
  seg_startz.clear();
  seg_endx.clear();
  seg_endy.clear();
  seg_endz.clear();
  seg_energy.clear();
  seg_sec_energy.clear();
  seg_id.clear();
  seg_pdg.clear();
  seg_det.clear();
  seg_nelectrons.clear();
  seg_nphotons.clear();

}

void dunend::EdepDump::FlushHDF5Buffer()
{
  std::string vertices_name = "vertices";
  std::string trajectories_name = "trajectories";
  std::string segments_name = "segments";


  const hsize_t nVtx = vertices_buffer.size();
  const hsize_t nTraj = trajectories_buffer.size();
  const hsize_t nSeg = segments_buffer.size();
  if(CreateNewTable)
  {
    //vertices table
    status=H5TBmake_table( vertices_name.c_str(),hdffile,vertices_name.c_str(),NFIELDS_VTX,nVtx,vertices_size, vertices_names, vertices_offset, vertices_types, fChunkSize, NULL, 0, &vertices_buffer[0] );
    //trajectories table
    status=H5TBmake_table( trajectories_name.c_str(),hdffile,trajectories_name.c_str(),NFIELDS_TRAJ,nTraj,trajectories_size, trajectories_names, trajectories_offset, trajectories_types, fChunkSize, NULL, 0, &trajectories_buffer[0] );
    //segments table
    status=H5TBmake_table( segments_name.c_str(),hdffile,segments_name.c_str(),NFIELDS_SEG,nSeg,segments_size, segments_names, segments_offset, segments_types, fChunkSize, NULL, 0, &segments_buffer[0] );

    CreateNewTable = false;
  }
  else
  {
    status=H5TBappend_records(hdffile, vertices_name.c_str() ,nVtx, vertices_size, vertices_offset, vertices_sizes, &vertices_buffer[0] );
    status=H5TBappend_records(hdffile, trajectories_name.c_str() ,nTraj, trajectories_size, trajectories_offset, trajectories_sizes, &trajectories_buffer[0] );
    status=H5TBappend_records(hdffile, segments_name.c_str() ,nSeg, segments_size, segments_offset, segments_sizes, &segments_buffer[0] );
  }
  vertices_buffer.clear();
}


std::vector<edep_utils::NDHitSegment> dunend::EdepDump::CombineSEDToSegment(
    std::vector<art::Ptr<sim::SimEnergyDeposit>> sedlist,
    float maxSagitta, float maxSeparation, float maxLength )
  {
    /// will search addtional fMaxAttempt before force creating new hit
    //  int fMaxAttempt = 20;

    //std::vector<sim::SimEnergyDeposit> ret;
    //std::sort( sedlist.begin(), sedlist.end(), []( auto &a, auto& b )
    //    {
    //    return a->EndX() < b->EndX();
    //    }
    //    );
    if(fLogLevel >= 6) std::cout<<maxSagitta<<", "<<maxLength<<", "<<maxSeparation<<std::endl;


    std::vector<edep_utils::NDHitSegment> hits;
    int fLastHit = -1;

    while( sedlist.size() > 0 )
    {
      auto sed = sedlist.front();

      edep_utils::NDHitSegment* currentHit = NULL;
      if( 0<=fLastHit && fLastHit< (int) hits.size() && fMergeSteps )
      {
        edep_utils::NDHitSegment *tmpHit = &hits[fLastHit];
        if (tmpHit->SameHit(sed))
        {
          if(fLogLevel >= 6) std::cout<<"SameHit"<<std::endl;
          currentHit = tmpHit;
        }
      }


      // If a hit wasn't found, create one
      if (!currentHit)
      {
        currentHit = new edep_utils::NDHitSegment(maxSagitta, 
            maxSeparation,
            maxLength,
            fDeltaTrackID);

        fLastHit = hits.size();
        hits.push_back(*currentHit);
        if(fLogLevel >= 6) std::cout<<"NewHit"<<std::endl;
      }

      hits.back().AddStep( sed );
      sedlist.erase( sedlist.begin() );
     
    }

    if(fLogLevel >= 6) 
    {
      std::cout<<"Number of hits: "<<hits.size()<<std::endl;
      std::cout<<"\tView hits: "<<std::endl;

      int ihit = 0;
      for( auto hit : hits )
      {
        std::cout<<"\t===\t"<<++ihit<<"\t===\t"<<std::endl;    
        hit.PrintHit();
      }
    }

    return hits;
}

std::vector<edep_utils::NDHitSegment> dunend::EdepDump::CombineSEDToSegment2(
    std::vector<art::Ptr<sim::SimEnergyDeposit>> &sedlist,
    float maxSagitta, float maxSeparation, float maxLength )
  {
    /// will search addtional fMaxAttempt before force creatingtesting  new hit
    //  int fMaxAttempt = 20;

    //std::vector<sim::SimEnergyDeposit> ret;
    auto sedlist_cp = sedlist;
    std::sort( sedlist_cp.begin(), sedlist_cp.end(), []( auto &a, auto& b )
        {
          if (a->TrackID() < b->TrackID()) return true;
          if (a->TrackID() > b->TrackID()) return false;

          if (a->PdgCode() < b->PdgCode()) return true;
          if (a->PdgCode() > b->PdgCode()) return false;

          if ( a->StartT() < b->StartT() ) return true;
          if ( a->StartT() > b->StartT() ) return false;
          return a->StartZ() < b->StartZ();
        }
      );
    if(fLogLevel >= 6) std::cout<<maxSagitta<<", "<<maxLength<<", "<<maxSeparation<<std::endl;


    std::vector<edep_utils::NDHitSegment> hits;
    int fLastHit = -1;

    if (fMergeSteps)
    {
      while( sedlist_cp.size() > 0 )
      {
        auto sed = sedlist_cp.front();

        edep_utils::NDHitSegment* currentHit = NULL;
        if( 0<=fLastHit && fLastHit< (int) hits.size())
        {
          edep_utils::NDHitSegment *tmpHit = &hits[fLastHit];
          if (tmpHit->SameHit(sed))
          {
            if(fLogLevel >= 6) std::cout<<"SameHit"<<std::endl;
            currentHit = tmpHit;
          }
        }


        // If a hit wasn't found, create one
        if (!currentHit)
        {
          currentHit = new edep_utils::NDHitSegment(maxSagitta, 
              maxSeparation,
              maxLength,
              fDeltaTrackID);

          fLastHit = hits.size();
          hits.push_back(*currentHit);
          if(fLogLevel >= 6) std::cout<<"NewHit"<<std::endl;
        }

        hits.back().AddStep( sed );
        sedlist_cp.erase( sedlist_cp.begin() );
      }
    }
    else
    {
      hits = ConvertSEDToNDHitSegment( sedlist_cp );
      // ConvertSegmentToSED(hits);
    }

    if(fLogLevel >= 6) 
    {
      std::cout<<"Number of hits: "<<hits.size()<<std::endl;
      std::cout<<"\tView hits: "<<std::endl;

      int ihit = 0;
      for( auto hit : hits )
      {
        std::cout<<"\t===\t"<<++ihit<<"\t===\t"<<std::endl;    
        hit.PrintHit();
      }
    }

    return hits;
}


std::vector<sim::SimEnergyDeposit> dunend::EdepDump::ConvertSegmentToSED( 
      std::vector<edep_utils::NDHitSegment> &segments
    )
{
  std::vector<sim::SimEnergyDeposit> ret;
  if (fLogLevel >=5) std::cout<<"\t\tConvertSegmentToSED: begin converting"<<std::endl;
  for( auto hit : segments )
  {
    if(fLogLevel >= 5)
    {
      std::cout<<"\t\t=== print hit === "<<std::endl;
      std::cout<<"\t\t\thit.NumPhotons(): "<<hit.NumPhotons()<<std::endl;
      std::cout<<"\t\t\thit.NumFPhotons(): "<<hit.NumFPhotons()<<std::endl;
      std::cout<<"\t\t\thit.NumSPhotons(): "<<hit.NumSPhotons()<<std::endl;
      std::cout<<"\t\t\thit.NumElectrons(): "<<hit.NumElectrons()<<std::endl;
      std::cout<<"\t\t\thit.ScintYieldRatio(): "<<    hit.ScintYieldRatio()<<std::endl;
      std::cout<<"\t\t\thit.TotalEnergyDeposit(): "<< hit.TotalEnergyDeposit()<<std::endl;
      std::cout<<"\t\t\thit.StartPos().X(): "<<       hit.StartPos().X()<<std::endl;
      std::cout<<"\t\t\thit.StartPos().Y(): "<<       hit.StartPos().Y()<<std::endl;
      std::cout<<"\t\t\thit.StartPos().Z(): "<<       hit.StartPos().Z()<<std::endl;
      std::cout<<"\t\t\thit.StopPos().X(): "<<        hit.StopPos().X()<<std::endl;
      std::cout<<"\t\t\thit.StopPos().Y(): "<<        hit.StopPos().Y()<<std::endl;
      std::cout<<"\t\t\thit.StopPos().Z(): "<<        hit.StopPos().Z()<<std::endl;
      std::cout<<"\t\t\thit.StartPos().T(): "<<       hit.StartPos().T()<<std::endl;
      std::cout<<"\t\t\thit.StopPos().T(): "<<        hit.StopPos().T()<<std::endl;
      std::cout<<"\t\t\thit.TrackID(): "<<            hit.TrackID()<<std::endl;
      std::cout<<"\t\t\thit.PDG(): "<<                hit.PDG()<<std::endl;
    }
    sim::SimEnergyDeposit sed(
        hit.NumPhotons(),
        hit.NumElectrons(),
        hit.ScintYieldRatio(),
        hit.TotalEnergyDeposit(),
        {hit.StartPos().X(), hit.StartPos().Y(),hit.StartPos().Z()},
        {hit.StopPos().X(), hit.StopPos().Y(),hit.StopPos().Z()},
        hit.StartPos().T(),
        hit.StopPos().T(),
        hit.TrackID(),
        hit.PDG()
        ); 
    ret.push_back(sed);

  }
 
  if (fLogLevel >=5) std::cout<<"\t\tConvertSegmentToSED: "<<ret.size()<<" seds"<<std::endl;
  return ret;
}



std::vector<edep_utils::NDHitSegment> dunend::EdepDump::ConvertSEDToNDHitSegment( 
      std::vector<art::Ptr<sim::SimEnergyDeposit>> &sedlist
      )
{
  std::vector<edep_utils::NDHitSegment> ret;
   for( auto &sed: sedlist )
   {
     //edep_utils::NDHitSegment *hit = new edep_utils::NDHitSegment(sed,0,0,0.5,-1);
      //ret.push_back(*std::move(hit));
      ret.emplace_back(sed,0,0,0.5,-1);
     // ret.back().AddStep( sed );
   }
  return ret;
}

TG4PrimaryParticle dunend::EdepDump::ConvertParticle( const simb::MCParticle &particle )
{
  TG4PrimaryParticle part;
  part.TrackId = particle.TrackId();
  part.Name = "None";
  part.PDGCode = particle.PdgCode();
  return part;
}


void dunend::EdepDump::FillTG4Event(art::Event const &e)
{  
  bool eConv = EnergyConversion();
  bool distConv = DistanceConversion();

  if (fLogLevel >= 5) std::cout<<"FillTG4Event -- begin"<<std::endl;
  if (! tg4event )
  {
    std::cout<<"!tg4event"<<std::endl;
    tg4event = new TG4Event();
    eventTree->Branch("Event",tg4event);
  }


  if (fLogLevel >= 5) std::cout<<"FillTG4Event -- setting"<<std::endl;
  tg4event->RunId = (int) e.run();
  tg4event->EventId= (int) e.id().event();

  /// Fill Primary Vertex
  if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill Primary Vertex"<<std::endl;
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  auto mctruthListHandle = e.getHandle< std::vector<simb::MCTruth> >(fGenieGenModuleLabel);
  if (mctruthListHandle)
    art::fill_ptr_vector(mclist, mctruthListHandle);

  for ( auto it = mclist.begin(); it!=mclist.end(); ++it )
  {
    auto mctruth = *it;
    auto tg4vtx = TG4PrimaryVertex();
    // PrimaryParticles   Particles
    for ( int i = 0; i < mctruth->NParticles(); i++ )
    {
      tg4vtx.Particles.push_back( ConvertParticle( mctruth->GetParticle( i ) ) );
    }
    // TG4PrimaryVertexContainer  Informational
    // ---- TODO: fill sensible Informational

    // TLorentzVector   Position
    //Position Unit: convert to mm from cm
    if (tg4vtx.Particles.size() > 0 ) tg4vtx.Position = mctruth->GetParticle(0).Position()*distConv;

    // std::string  GeneratorName
    tg4vtx.GeneratorName = edep_utils::generatorTable()[ mctruth->GeneratorInfo().generator ];

    // std::string  Reaction
    // ---- TODO: fill sensible reaction
    tg4vtx.Reaction = "NONE";

    // std::string  Filename
    // ---- TODO: fill sensible Filename 
    tg4vtx.Filename = "NONE";

    // Int_t  InteractionNumber
    tg4vtx.InteractionNumber = (int) (it - mclist.begin() );

    // ---- TODO: needs to check if these info is saved in LArSoft
    // Float_t  CrossSection
    tg4vtx.CrossSection=0;
    // Float_t  DiffCrossSection
    tg4vtx.DiffCrossSection=0;
    // Float_t  Weight
    tg4vtx.Weight = 1;
    // Float_t Probability
    tg4vtx.Probability= 1;
    // push vtx into vector
    tg4event->Primaries.push_back( tg4vtx );
  }


  /// Fill Trajectories
  // Get Geant4 information
  if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill Trajectories"<<std::endl;
  std::vector<art::Ptr<simb::MCParticle>> mcplist;
  auto mcpListHandle = e.getHandle< std::vector<simb::MCParticle> >(fGeantModuleLabel);
  if (mcpListHandle)
    art::fill_ptr_vector(mcplist, mcpListHandle);

  art::FindManyP<simb::MCTruth,sim::GeneratedParticleInfo> fmth(mcpListHandle, e, fGeantModuleLabel);

  for ( auto it = mcplist.begin(); it!=mcplist.end(); ++it )
  {

    if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill MCP "<<it - mcplist.begin()<<std::endl;
    auto mcp = *it;
    // Fill MCP info to fTree for analysis
    FillSingleMCP(mcp);

    TG4Trajectory traj;

    auto mctraj = mcp->Trajectory();
    // TrajectoryPoints  Points
    if (fLogLevel >= 5) 
    {
      std::cout<<"FillTG4Event -- Fill MCP -- npoints: "<<mctraj.size()<<std::endl;
      std::cout<<"FillTG4Event -- Fill MCP -- nprocesses: "<<mctraj.TrajectoryProcesses().size()<<std::endl;
    }
    for( auto itp = mctraj.begin(); itp != mctraj.end(); ++itp )
    {
      if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill MCP -- point: "<<itp-mctraj.begin()<<std::endl;
      TG4TrajectoryPoint tg4point;

      //TLorentzVector  Position
      //The position of this trajectory point. More...
      //Position Unit: convert to mm from cm
      tg4point.Position = itp->first*distConv;
     
      //TVector3  Momentum
      //The momentum of the particle at this trajectory point. More...
      // convert from GeV to MeV
      tg4point.Momentum = itp->second.Vect()*eConv;
     
      // TODO: completed larg4 output does not provide full process information
      // This info might be available during larg4 generation.
      // The original name is available in LarG4::ParticleListAction, 
      // but was reduced to a few hardcoded processes by MCTrajectory
      //Int_t   Process
      //Int_t   Subprocess


      if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill MCP -- point: getprocess"<<std::endl;
      /// TODO: needs to implement some sort of process tag, but the TrajectoryProcesses are not always filled
      /// Assume first point was generated -- process unknown
      /// Process[i] is for point[i+1]

      // int ithpoint = (int) (itp - mctraj.begin());
      // auto process = ( ithpoint > 0)? edep_utils::processTable()[mctraj.TrajectoryProcesses().at(ithpoint-1).second] : edep_utils::processTable()[0];
      // tg4point.Process = process.first;
      // tg4point.Subprocess = process.second;


      //push tg4point to traj
      if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill MCP -- point: push"<<std::endl;
      traj.Points.push_back( tg4point );



    }
   
    // Int_t   TrackId
    // LArSoft TrackID starts at 1, this in incompatible with Edepsim
    traj.TrackId = mcp->TrackId()+fDeltaTrackID;

   
    // Int_t   ParentId
    traj.ParentId = mcp->Mother()+fDeltaTrackID;
   
    // std::string   Name
    // The name of the particle
    // TODO: Fill sensible name
    traj.Name = "None";
   
    // Int_t   PDGCode
    traj.PDGCode = mcp->PdgCode();
   
    // TLorentzVector  InitialMomentum
    traj.InitialMomentum = mcp->Momentum();

    //Fill trajectory
    tg4event->Trajectories.push_back( traj );
  }


  /// Fill SegmentDetectors
  if (fLogLevel >= 5) std::cout<<"FillTG4Event -- Fill SegmentDetectors"<<std::endl;

  if (combined_segments.size() == 0 )
  {
    for (auto & label : fSEDModuleLabels){
      cout<<label.instance().substr(20)<<endl;
      std::vector<art::Ptr<sim::SimEnergyDeposit>> sedlist;
      auto sedListHandle = e.getHandle< std::vector<sim::SimEnergyDeposit> >(label);
      if (sedListHandle){
        art::fill_ptr_vector(sedlist, sedListHandle);
      }


      for( auto &sed: sedlist )
      {
        if (fLogLevel >= 5)
        {
          // DEBUG
          std::cout<<" == n segments: "<<sedlist.size()<<std::endl;
          std::cout<<sed->TrackID()<<"\t";
          std::cout<<std::endl;
        }

        FillSingleSED( sed, label.instance().substr(20) );

      }

      //string det = label.instance().substr(20);
      std::string encode = label.encode();
      combined_segments[encode]= CombineSEDToSegment2(
          sedlist,
          fMaxSagitta,
          fMaxSeparation,
          fMaxLength
          );

      //combined_sedlists[encode] = ConvertSegmentToSED( combined_segments[encode] );
      }
  }

  for (auto & [label, segments]: combined_segments)
  {
    for( auto &seg : segments )
    {
      FillSingleNDHitSegment(seg, label);
      TG4HitSegment tg4seg;

      tg4seg.Contrib = seg.Contributors();
      tg4seg.PrimaryId = seg.TrackID();


      tg4seg.EnergyDeposit = seg.TotalEnergyDeposit();
      tg4seg.SecondaryDeposit= seg.SecondaryEnergyDeposit();
      //Position Unit: convert to mm from cm
      float conv = DistanceConversion();
      tg4seg.TrackLength = seg.TrackLength()*conv;
      tg4seg.Start.SetXYZT( seg.StartPos().X()*conv,seg.StartPos().Y()*conv,seg.StartPos().Z()*conv,seg.StartPos().T());
      tg4seg.Stop.SetXYZT( seg.StopPos().X()*conv,seg.StopPos().Y()*conv,seg.StopPos().Z()*conv,seg.StopPos().T());

      //Fill
      tg4event->SegmentDetectors[label].push_back(tg4seg);
    }
  }


  if (fLogLevel >= 5)
  {
    // DEBUG: Display track IDs
    std::set<int> trackid_traj, trackid_seg, trackid_seg_primary;
    
    std::cout<<" === trajectory full tid: "<<tg4event->Trajectories.size()<<std::endl;
    for (auto &traj : tg4event->Trajectories) 
    {
      trackid_traj.insert( traj.GetTrackId() );
      //std::cout<<traj.GetTrackId()<<"\t";
    }

    for (auto &[ det, segments ]: tg4event->SegmentDetectors )
    {
      std::cout<<" == segdet: "<<det<<" == with "<<segments.size()<<" detsegs: "<<std::endl;
      for (auto &seg : segments) 
      {
        for( auto &c: seg.Contrib ) trackid_seg.insert(c);
        trackid_seg_primary.insert(seg.PrimaryId);
        std::cout<<seg.PrimaryId<<"\t";
      }
      std::cout<<std::endl;
    }

    // DEBUG: Print Tracks
    std::cout<<" === trajectory tid: "<<std::endl;
    for( auto c: trackid_traj ) std::cout<<c<<"\t";
    std::cout<<std::endl;

    std::cout<<" === seg tid: "<<std::endl;
    for( auto c: trackid_seg ) std::cout<<c<<"\t";
    std::cout<<std::endl;

    std::cout<<" === seg tid primary: "<<std::endl;
    for( auto c: trackid_seg_primary ) std::cout<<c<<"\t";
    std::cout<<std::endl;
  }

  eventTree->Fill();
}




void dunend::EdepDump::FillSingleMCP(art::Ptr<simb::MCParticle> &mcp)
{
  this->mcp_id.push_back(mcp->TrackId() );
  this->mcp_mother.push_back(mcp->Mother() );
  this->mcp_pdg.push_back(mcp->PdgCode() );
  this->mcp_energy.push_back(mcp->E() );
  this->mcp_px.push_back(mcp->Px() );
  this->mcp_py.push_back(mcp->Py() );
  this->mcp_pz.push_back(mcp->Pz() );
  this->mcp_startx.push_back(mcp->Vx() );
  this->mcp_starty.push_back(mcp->Vy() );
  this->mcp_startz.push_back(mcp->Vz() );
  this->mcp_endx.push_back(mcp->EndX() );
  this->mcp_endy.push_back(mcp->EndY() );
  this->mcp_endz.push_back(mcp->EndZ() );

}

void dunend::EdepDump::FillSingleSED(art::Ptr<sim::SimEnergyDeposit> &sed, std::string det)
{
  this->sed_startx.push_back(sed->StartX() );
  this->sed_starty.push_back(sed->StartY() );
  this->sed_startz.push_back(sed->StartZ() );
  this->sed_endx.push_back(sed->EndX() );
  this->sed_endy.push_back(sed->EndY() );
  this->sed_endz.push_back(sed->EndZ() );
  this->sed_energy.push_back(sed->Energy() );
  this->sed_nelectrons.push_back(sed->NumElectrons());
  this->sed_nphotons.push_back(sed->NumPhotons());
  this->sed_sec_energy.push_back(sed->Energy()*sed->NumPhotons()*(sed->NumPhotons()+sed->NumElectrons()) );
  this->sed_id.push_back(sed->TrackID() );
  this->sed_pdg.push_back(sed->PdgCode() );
  this->sed_det.push_back(det);
}

void dunend::EdepDump::FillSingleNDHitSegment(edep_utils::NDHitSegment &seg, std::string det)
{
  this->seg_startx.push_back(seg.StartPos().X() );
  this->seg_starty.push_back(seg.StartPos().Y() );
  this->seg_startz.push_back(seg.StartPos().Z() );
  this->seg_endx.push_back(seg.StopPos().X() );
  this->seg_endy.push_back(seg.StopPos().Y() );
  this->seg_endz.push_back(seg.StopPos().Z() );
  this->seg_energy.push_back(seg.TotalEnergyDeposit() );
  this->seg_sec_energy.push_back(seg.SecondaryEnergyDeposit() );
  this->seg_id.push_back(seg.TrackID() );
  this->seg_pdg.push_back(seg.PDG() );
  this->seg_det.push_back(det);
  this->seg_nelectrons.push_back(seg.NumElectrons());
  this->seg_nphotons.push_back(seg.NumPhotons());
}

void dunend::EdepDump::FillHDF5( art::Event const& e)
{
  if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: Entered"<<std::endl;

  unsigned int eventID = e.id().event();
  // * MC truth information
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  auto mctruthListHandle = e.getHandle< std::vector<simb::MCTruth> >(fGenieGenModuleLabel);
  if (mctruthListHandle)
    art::fill_ptr_vector(mclist, mctruthListHandle);

  if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: begin 1"<<std::endl;
  int ic = 0;
  for ( auto & mctruth : mclist )
  {
    //primaries
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: count 1: "<<++ic<<std::endl;
    if( mctruth->Origin() == simb::kBeamNeutrino )
    {
      vertices_buffer.push_back( {
          eventID, 
          (float) mctruth->GetNeutrino().Nu().Vx(),
          (float) mctruth->GetNeutrino().Nu().Vy(),
          (float) mctruth->GetNeutrino().Nu().Vz() } );
    }
  }
  if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: Filled vertices_buffer"<<std::endl;



  // Get Geant4 information
  std::vector<art::Ptr<simb::MCParticle>> mcplist;
  auto mcpListHandle = e.getHandle< std::vector<simb::MCParticle> >(fGeantModuleLabel);
  if (mcpListHandle)
    art::fill_ptr_vector(mcplist, mcpListHandle);

  art::FindManyP<simb::MCTruth,sim::GeneratedParticleInfo> fmth(mcpListHandle, e, fGeantModuleLabel);

  for (auto & mcp : mcplist){
    trajectories_dt data;
    data.eventID= eventID;
    data.trackID = mcp->TrackId();
    data.start_process = 0;
    data.start_sub_process = 0;
    data.end_process = 0;
    data.end_sub_process = 0;
    data.pdgId = mcp->PdgCode();
    data.parentID = mcp->Mother();
    data.pxyz_start[0] = (float) mcp->Px();
    data.pxyz_start[1] = (float) mcp->Py();
    data.pxyz_start[2] = (float) mcp->Pz();
    data.xyz_start[0] = (float) mcp->Vx();
    data.xyz_start[1] = (float) mcp->Vy();
    data.xyz_start[2] = (float) mcp->Vz();
    data.pxyz_end[0] = (float) mcp->EndPx();
    data.pxyz_end[1] = (float) mcp->EndPy();
    data.pxyz_end[2] = (float) mcp->EndPz();
    data.xyz_end[0] = (float) mcp->EndX();
    data.xyz_end[0] = (float) mcp->EndY();
    data.xyz_end[0] = (float) mcp->EndZ();
    data.t_start = mcp->T();
    data.t_end = mcp->EndT();
    
    trajectories_buffer.push_back(data);
  }
  if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: filled trajectories_buffer"<<std::endl;

  // Get SimEnergyDeposit information

  for (auto & label : fSEDModuleLabels){
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: sed label "<<label<<std::endl;
    //cout<<label.instance().substr(20)<<endl;
    std::vector<art::Ptr<sim::SimEnergyDeposit>> sedlist;
    auto sedListHandle = e.getHandle< std::vector<sim::SimEnergyDeposit> >(label);
    if (sedListHandle){
      art::fill_ptr_vector(sedlist, sedListHandle);
    }

    std::vector<edep_utils::NDHitSegment> combined_hits = CombineSEDToSegment2(
        sedlist,
        fMaxSagitta,
        fMaxSeparation,
        fMaxLength
        );
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: sed: \tcreated combined_hits with "<<combined_hits.size()<<" hits"<<std::endl;
    auto csedlist = ConvertSegmentToSED( combined_hits );
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: sed: \tconverted combined_hits to sed"<<std::endl;





    //for (auto & sed : sedlist){
    for (auto & Sed : csedlist){
      auto sed = &Sed;

      segments_dt data;
      if(fLogLevel>=10) std::cout<<eventID<<std::endl;
      data.eventID=eventID;
      data.trackID=sed->TrackID();
      data.n_electrons=sed->NumElectrons();
      data.n_photons=sed->NumPhotons();
      data.pdgId=sed->PdgCode();
      data.x_start = sed->StartX();
      data.y_start = sed->StartY();
      data.z_start = sed->StartZ();
      data.t0_start = sed->StartT();
      data.x_end = sed->EndX();
      data.y_end = sed->EndY();
      data.z_end = sed->EndZ();
      data.t0_end = sed->EndT();
      data.x = sed->MidPointX();
      data.y = sed->MidPointY();
      data.z = sed->MidPointZ();
      data.t0 = sed->Time();
      data.dE = sed->Energy();

      // As secondary = total * n_photon/(n_q), where n_q = n_photon + n_e
      // double nonIonizingDeposit = data.dE*data.n_photons/(data.n_photons+data.n_electrons);


      float dx = sqrt(pow(data.x_start-data.x_end,2)+
                      pow(data.y_start-data.y_end,2)+
                      pow(data.z_start-data.z_end,2));
      if (dx) data.dEdx = data.dE/dx;
      else data.dEdx = 0;
      data.pixel_plane=0;
      data.t_start = 0;
      data.t_end= 0;
      data.t = 0;
      data.long_diff = 0;
      data.tran_diff = 0;
      segments_buffer.push_back(data);
    }
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: sed: \tfilled segments_buffer"<<std::endl;
  }

  if (trajectories_buffer.size() > fBuffSize )
  {
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: FlushHDF5Buffer()"<<std::endl;
    FlushHDF5Buffer();
    if (fLogLevel >=5) std::cout<<"\t\tFillHDF5: FlushHDF5Buffer() -- done"<<std::endl;
  }

}


DEFINE_ART_MODULE(dunend::EdepDump)
