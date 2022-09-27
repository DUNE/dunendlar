art_make( BASENAME_ONLY
          MODULE_LIBRARIES 
			lardataobj::RecoBase
			dunendlar::Prototypes_Module0_ChannelMap_NDLArModule0ChannelMapService_service
                        ART_FRAMEWORK_CORE
			ART_FRAMEWORK_PRINCIPAL
			ART_FRAMEWORK_SERVICES_REGISTRY
			ART_ROOT_IO_TFILE_SUPPORT
			ROOT::Core
			ART_ROOT_IO_TFILESERVICE_SERVICE
			art::Persistency_Common canvas
			art::Persistency_Provenance canvas
			art::Utilities canvas
			messagefacility::MF_MessageLogger			
	                fhiclcpp::fhiclcpp
			cetlib::cetlib cetlib_except
        )

# install_headers()
install_fhicl()
install_source()
