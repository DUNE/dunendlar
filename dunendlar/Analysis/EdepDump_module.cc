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

#include "edep_hdf5_utils.h"

#include <vector>
#include <string>

using namespace hdf5_utils;
using namespace std;

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

  int fLogLevel;
  const art::InputTag fGenieGenModuleLabel;
  const art::InputTag fGeantModuleLabel;
  vector<art::InputTag> fSEDModuleLabels;

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
  vector<int>    sed_id;        //MCParticle index
  vector<int>    sed_pdg;       //PDG code
  vector<string> sed_det;       //Detector name

  void reset();

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
  void FillHDF5();
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

  //create array type
  //hsize_t arr_dim3[] = {3};
  //array_type= H5Tarray_create(H5T_NATIVE_FLOAT, 1, arr_dim3);

}


/* void dunend::EdepDump::analyze(art::Event const& e)
{
  reset();

  run = e.run();
  subrun = e.subRun();
  event = e.id().event();

  // * MC truth information
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  auto mctruthListHandle = e.getHandle< std::vector<simb::MCTruth> >(fGenieGenModuleLabel);
  if (mctruthListHandle)
    art::fill_ptr_vector(mclist, mctruthListHandle);

  for (auto & mctruth: mclist){
    if (mctruth->Origin() == simb::kBeamNeutrino){
      nuPDG.push_back(mctruth->GetNeutrino().Nu().PdgCode());
      ccnc.push_back(mctruth->GetNeutrino().CCNC());
      mode.push_back(mctruth->GetNeutrino().Mode());
      Q2.push_back(mctruth->GetNeutrino().QSqr());
      W.push_back(mctruth->GetNeutrino().W());
      X.push_back(mctruth->GetNeutrino().X());
      Y.push_back(mctruth->GetNeutrino().Y());
      hitnuc.push_back(mctruth->GetNeutrino().HitNuc());
      target.push_back(mctruth->GetNeutrino().Target());
      enu.push_back(mctruth->GetNeutrino().Nu().E());
      nuvtxx.push_back(mctruth->GetNeutrino().Nu().Vx());
      nuvtxy.push_back(mctruth->GetNeutrino().Nu().Vy());
      nuvtxz.push_back(mctruth->GetNeutrino().Nu().Vz());
      if (mctruth->GetNeutrino().Nu().P()){
        nu_dcosx.push_back(mctruth->GetNeutrino().Nu().Px()/mctruth->GetNeutrino().Nu().P());
        nu_dcosy.push_back(mctruth->GetNeutrino().Nu().Py()/mctruth->GetNeutrino().Nu().P());
        nu_dcosz.push_back(mctruth->GetNeutrino().Nu().Pz()/mctruth->GetNeutrino().Nu().P());
      }
      else{
        nu_dcosx.push_back(-999);
        nu_dcosy.push_back(-999);
        nu_dcosz.push_back(-999);
      }
      lep_mom.push_back(mctruth->GetNeutrino().Lepton().P());
      if (mctruth->GetNeutrino().Lepton().P()){
        lep_dcosx.push_back(mctruth->GetNeutrino().Lepton().Px()/mctruth->GetNeutrino().Lepton().P());
        lep_dcosy.push_back(mctruth->GetNeutrino().Lepton().Py()/mctruth->GetNeutrino().Lepton().P());
        lep_dcosz.push_back(mctruth->GetNeutrino().Lepton().Pz()/mctruth->GetNeutrino().Lepton().P());
      }
      else{
        lep_dcosx.push_back(-999);
        lep_dcosy.push_back(-999);
        lep_dcosz.push_back(-999);
      }
      if (mctruth->NParticles()){
        simb::MCParticle particle = mctruth->GetParticle(0);
        t0.push_back(particle.T());
      }
      else{
        t0.push_back(-999);
      }
    }
  }

  // Get Geant4 information

  std::vector<art::Ptr<simb::MCParticle>> mcplist;
  auto mcpListHandle = e.getHandle< std::vector<simb::MCParticle> >(fGeantModuleLabel);
  if (mcpListHandle){
    art::fill_ptr_vector(mcplist, mcpListHandle);
  }
  art::FindManyP<simb::MCTruth,sim::GeneratedParticleInfo> fmth(mcpListHandle, e, fGeantModuleLabel);

  for (auto & mcp : mcplist){
    mcp_id.push_back(mcp->TrackId());
    mcp_mother.push_back(mcp->Mother());
    mcp_pdg.push_back(mcp->PdgCode());
    mcp_energy.push_back(mcp->E());
    mcp_px.push_back(mcp->Px());
    mcp_py.push_back(mcp->Py());
    mcp_pz.push_back(mcp->Pz());
    mcp_startx.push_back(mcp->Vx());
    mcp_starty.push_back(mcp->Vy());
    mcp_startz.push_back(mcp->Vz());
    mcp_endx.push_back(mcp->EndPosition()[0]);
    mcp_endy.push_back(mcp->EndPosition()[1]);
    mcp_endz.push_back(mcp->EndPosition()[2]);
    if (fmth.isValid()){
      auto vmcth = fmth.at(mcp.key());
      //cout<<vmcth.size()<<endl;
      if (!vmcth.empty()) mcp_nuid.push_back(vmcth[0].key());
    }
} */

void dunend::EdepDump::analyze(art::Event const& e)
{
  reset();

  run = e.run();
  subrun = e.subRun();
  event = e.id().event();

  unsigned int eventID = event;

  // * MC truth information
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  auto mctruthListHandle = e.getHandle< std::vector<simb::MCTruth> >(fGenieGenModuleLabel);
  if (mctruthListHandle)
    art::fill_ptr_vector(mclist, mctruthListHandle);

  for ( auto & mctruth : mclist )
  {
    //primaries
    if( mctruth->Origin() == simb::kBeamNeutrino )
    {
      vertices_buffer.push_back( {
          eventID, 
          (float) mctruth->GetNeutrino().Nu().Vx(),
          (float) mctruth->GetNeutrino().Nu().Vy(),
          (float) mctruth->GetNeutrino().Nu().Vz() } );
    }
  }


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

  // Get SimEnergyDeposit information

  for (auto & label : fSEDModuleLabels){
    string det = label.instance().substr(20);
    //cout<<label.instance().substr(20)<<endl;
    std::vector<art::Ptr<sim::SimEnergyDeposit>> sedlist;
    auto sedListHandle = e.getHandle< std::vector<sim::SimEnergyDeposit> >(label);
    if (sedListHandle){
      art::fill_ptr_vector(sedlist, sedListHandle);
    }
    for (auto & sed : sedlist){

      segments_dt data;
      if(fLogLevel>10) std::cout<<eventID<<std::endl;
      data.eventID=eventID;
      data.trackID=sed->TrackID();
      data.n_electrons=sed->NumElectrons();
      data.n_photons=sed->NumPhotons();
      data.n_photons=sed->PdgCode();
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

      data.pixel_plane=0;
      data.t_start = 0;
      data.t_end= 0;
      data.t = 0;
      data.long_diff = 0;
      data.tran_diff = 0;
      segments_buffer.push_back(data);
    }
  }

  fTree->Fill();
  if (trajectories_buffer.size() > fBuffSize )
  {
    FillHDF5();
  }

}

void dunend::EdepDump::beginJob()
{
  art::ServiceHandle<geo::Geometry> geom;
  TGeoManager *geoman = geom->ROOTGeoManager();
  TFile *f = TFile::Open("geometry.root", "recreate");
  geoman->Write("EDepSimGeometry");
  f->Close();

  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("ndsim","ND simulation tree");
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
  fTree->Branch("sed_id", &sed_id);
  fTree->Branch("sed_pdg", &sed_pdg);
  fTree->Branch("sed_det", &sed_det);

  //=====================================================================================
  //hdf5
  hdffile = H5Fcreate(fHDF5DumpFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  std::cout<<"hdffile created"<<std::endl;
}

void dunend::EdepDump::endJob()
{
  FillHDF5();
  /* close type */
  H5Tclose( vec3_type );
  /* close the file */
  H5Fclose( hdffile );
}

void dunend::EdepDump::reset(){

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
  sed_id.clear();
  sed_pdg.clear();
  sed_det.clear();
}

void dunend::EdepDump::FillHDF5()
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

DEFINE_ART_MODULE(dunend::EdepDump)
