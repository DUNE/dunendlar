install_fhicl()

# Also put a copy in the source tree.
FILE(GLOB fcl_files *.fcl)
install_source( EXTRAS ${fcl_files} )

FILE(GLOB xml_files *.xml)
install(FILES ${xml_files} DESTINATION xml)
install_source( EXTRAS ${xml_files} )
