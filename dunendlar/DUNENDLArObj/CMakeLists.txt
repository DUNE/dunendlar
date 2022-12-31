#cet_make( LIBRARIES canvas
#		    MF_MessageLogger
#		    MF_Utilities
#		    cetlib cetlib_except
#		    ${ROOT_BASIC_LIB_LIST}
        #        NO_DICTIONARY )

      #art_dictionary( DICTIONARY_LIBRARIES dunedataobj_DuneBase )
art_make(
  LIB_LIBRARIES
  MF_MessageLogger
  cetlib cetlib_except
  ${ROOT_BASIC_LIB_LIST}
  lardataobj::Simulation
  art::Framework_Core
  lardataobj::Simulation
  larcoreobj::SimpleTypesAndConstants
#        ART_ROOT_IO_TFILE_SUPPORT
#        ROOT_Core
#        ART_ROOT_IO_TFILESERVICE_SERVICE
#        ROOT_EG
  DICT_LIBRARIES
  #  EXCLUDE OpDetDivRec.h OpDetDivRec.cc
  )


install_headers()
install_source()
