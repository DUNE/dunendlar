include_directories( $ENV{EDEPSIM_INC}/EDepSim )

# print out all cmake variables
#message("======================================")
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach() 
#message("======================================")

art_make(
  LIB_LIBRARIES
  EDepSim::edepsim_io
  BASENAME_ONLY
  MODULE_LIBRARIES
  cetlib cetlib_except
  larsim::Simulation
  lardataobj::Simulation
  lardataobj::RecoBase
  lardata::Utilities
  lardata::ArtDataHelper
  larcorealg::Geometry
  nusimdata::SimulationBase
  larsim::MCCheater_PhotonBackTrackerService_service           
  larsim::MCCheater_BackTrackerService_service           
  larsim::MCCheater_ParticleInventoryService_service
  larreco::RecoAlg
  ROOT_BASIC_LIB_LIST
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art_root_io::tfile_support
  ROOT::Core
  EDepSim::edepsim_io
  art_root_io::TFileService_service
  art::Persistency_Common
  canvas::canvas
  art::Persistency_Provenance
  art::Utilities
  messagefacility::MF_MessageLogger
  HDF5::HDF5
  HDF5_HL_SHARED_LIBRARY
  )


install_headers()
install_fhicl()
install_source()





