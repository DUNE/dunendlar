#ifndef NDLArModule0RawInput_h
#define NDLArModule0RawInput_h
#include "art/Framework/Core/InputSourceMacros.h" 
#include "art/Framework/IO/Sources/Source.h" 
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Core/Frameworkfwd.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "dunecore/HDF5Utils/HDF5Utils.h"
#include "TH1F.h"

namespace dune {
//Forward declare the class
class NDLArModule0RawInputDetail;
}

class dune::NDLArModule0RawInputDetail {
 public:
  NDLArModule0RawInputDetail(fhicl::ParameterSet const & ps,
                              art::ProductRegistryHelper & rh,
                              art::SourceHelper const & sh);

  void readFile(std::string const & filename, art::FileBlock*& fb);

  bool readNext(art::RunPrincipal const* const inR,
                art::SubRunPrincipal const* const inSR,
                art::RunPrincipal*& outR,
                art::SubRunPrincipal*& outSR,
                art::EventPrincipal*& outE);

  void closeCurrentFile() {
    if (fHDFFile->filePtr)
      dune::HDF5Utils::closeFile(std::move(fHDFFile));
    if (fRData)
      {
        H5Dvlen_reclaim (fVlt, fRDataspaceid, H5P_DEFAULT, fRData);
        free(fRData);
	fRData = 0;
	H5Sclose(fRDataspaceid);
	H5Tclose(fVlt);
      }
    fCurRun++;
  };

 private:
  std::unique_ptr<dune::HDF5Utils::HDFFileInfo> fHDFFile;
  std::string pretend_module_name;
  int fLogLevel;
  art::SourceHelper const & pmaker;

  size_t fConfigRunNumber;
  size_t fConfigSubRunNumber;
  size_t fConfigNTickTriggerReadout;  // number of timestmap ticks (0.1 microseconds each) to look for data to include
  size_t fConfigNTickNextTriggerGap;  // number of timestmap ticks (0.1 microseconds each) after a trigger that
                                      // the next trigger will be considered part of the first one and not get its own event.
                                      // e.g. Michel decays may be a few microseconds after the origian event and don't want
                                      // to call these new events
  size_t fConfigMaxMessageLookBack;

  size_t fNMessages;
  size_t fCurEvent;  // triggers are not divided up in the file, so we keep track here.
  size_t fCurRun;  // triggers are not divided up in the file, so we keep track here.
  size_t fCurMessage;
  uint32_t fCurTrigTS;
  uint8_t fCurTrigBits;
  uint8_t fCurIO_Group;

  // buffer for i/o groups
  typedef struct iogroupstruct_t {
      char iog;
  } ig_t;
  std::vector<ig_t> fIogBuff;

  // pointer to the buffer for the message data and some associated ID's

  hvl_t *fRData;
  hid_t fVlt;
  hid_t fRDataspaceid;

  TH1F *fHist_tts;
  TH1F *fHist_dts;
  TH1F *fHist_deltats;
  TH1F *fHist_deltatrigtime;
 };
#endif
