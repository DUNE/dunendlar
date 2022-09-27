simple_plugin(NDLArModule0RawInput "source"
         cetlib_except::cetlib_except
         fhiclcpp::fhiclcpp
         messagefacility::MF_MessageLogger
         lardataobj::RawData
         art::Framework_Principal
         art::Framework_Core
         art::Framework_Principal
         art::Framework_Services_Registry
	 dunecore::HDF5Utils
	 dunendlar::Prototypes_Module0_ChannelMap_NDLArModule0ChannelMapService_service
         LIBHDF5 
         pthread
         z
  art::Framework_Services_Registry
)

install_headers()
install_fhicl()
install_source()
install_scripts()
