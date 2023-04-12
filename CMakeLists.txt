# ======================================================================
#  larsoft main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larsoft/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================
# LarSoft is an empty product
# It sets up all of the larsoft ups products (lardata, larsim, larreco, etc.)
# ======================================================================

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

project(dunendlar VERSION 01.01.00 LANGUAGES CXX)

## Make target names shorter (dunecore::X vs dunecore::dune_X).
# set(dunecore_NAMESPACE dune)

# Project variable defaults (_INIT) and overrides.
set(${PROJECT_NAME}_SCRIPTS_DIR_INIT scripts)

include(CetCMakeEnv)
cet_cmake_env()

# Find our own CMake modules, and tell everyone else where to find them.
cet_cmake_module_directories(Modules BINARY)

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)
##add_definitions(-DGENIE_PRE_R3)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
#find_ups_product( art_root_io )
find_ups_root()
find_ups_product( dunecore )
#find_ups_product( larsoft )
find_ups_product( larcoreobj )
find_ups_product( edepsim )
#find_ups_product( hep_hpc )
find_ups_product( dunedaqdataformats )
find_ups_product( dunedetdataformats )
find_package( HDF5 REQUIRED EXPORT )

# Define an export set for tool support to enable simpler per-tool build
# call.
cet_register_export_set(SET_NAME PluginTypes NAMESPACE art_plugin_types)

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(dunendlar)
add_subdirectory(fcl)
## release database
# add_subdirectory(releaseDB)
# subdirectory for test code
# add_subdirectory(test)
# CMake Modules
#add_subdirectory(Modules)
# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
