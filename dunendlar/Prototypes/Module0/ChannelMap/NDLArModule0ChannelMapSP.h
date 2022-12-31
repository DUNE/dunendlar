///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       NDLArModule0ChannelMapSP
// Module type: algorithm
// File:        NDLArModule0ChannelMapSP.h
// Author:      Tom Junk, September 26, 2022
//
// Implementation of hardware-offline channel mapping reading from a file.
// art-independent class  
// The SP in the class and file name means "Service Provider"
// DUNE ND-LAr Module 0
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NDLArModule0ChannelMapSP_H
#define NDLArModule0ChannelMapSP_H

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

namespace dune {
  class NDLArModule0ChannelMapSP;
}

class dune::NDLArModule0ChannelMapSP {

 public:

  typedef struct NDLArModule0ChanInfo {
    unsigned int offlinechan;     // in gdml and channel sorting convention
    unsigned int io_group;        // io_group, which corresponds to TPC.  1 or 2
    unsigned int io_channel;      // uart.  Four of these per tile
    unsigned int tile;            // tile -- should be discernable from io_group and io_channel
    unsigned int chip;            // 100 per tile
    unsigned int chipchannel;     // 49 different numbers from 0 to 63
    double xyz[3];                // position in 3-space (cm)
    bool valid;                   // true if valid, false otherwise
  } NDLArModule0ChanInfo_t;

  NDLArModule0ChannelMapSP();  // constructor

  // initialize:  read map from two files, one containing two APAs worth of channel mapping (inverted and upright),
  // and the other containing a crate and APA name listing.

  void ReadMapFromFile(const std::string &chanmapfile, 
                       std::vector<double> &anodexoffset,
                       std::vector<double> &anodeyoffset,
                       std::vector<double> &anodezoffset);


  NDLArModule0ChanInfo_t GetChanInfoFromElectronics(
                                                    unsigned int io_group,
                                                    unsigned int io_channel,
                                                    unsigned int chip,
                                                    unsigned int chipchannel) const;

  NDLArModule0ChanInfo_t GetChanInfoFromOfflChan(unsigned int offlchan) const;

  // lookup by Y and Z within the readout plane, and use X to determine which readout plane to look up

  NDLArModule0ChanInfo_t GetChanInfoFromXYZ(double x, double y, double z) const;

  // this method returns a vector of channel infos, the first of which correspond to the x, y, z input, and the
  // remainder are within a radius r (in cm) in the yz plane

  std::vector<NDLArModule0ChanInfo_t> GetChanInfoFromXYZWithNeighbors(double x, double y, double z, double r) const;

 private:

  void InitializeChanLocParams();

  bool fInitialized;

  unsigned int fNChans;  // 78400 nominal chans but compute it from input

  double fXMin;
  double fXMax;
  double fYMin;
  double fYMax;
  double fZMin;
  double fZMax;
  double fPixelPitch;
  double fYTileSep;
  double fZTileSep;
  size_t fNChansPerRow;
  size_t fNChansPerTRow;
  size_t fNChansPerTileRow;
  size_t fNChansPerSide;
  size_t fTPCCathodeLoc;

  std::unordered_map<unsigned int,   // io_group
    std::unordered_map<unsigned int, // tile
    std::unordered_map<unsigned int, // chip
    std::unordered_map<unsigned int, // chipchannel
    NDLArModule0ChanInfo_t > > > > fDetToChanInfo;

  // implement this as a lookup table of pointers, so that we don't have to duplicate the map
  // in memory

  std::vector<NDLArModule0ChanInfo_t*> fOfflToChanInfo;

  //-----------------------------------------------

  void check_offline_channel(unsigned int offlineChannel) const
  {
    if (offlineChannel >= fNChans)
      {      
        throw std::range_error("NDLArModule0ChannelMapSP offline Channel out of range"); 
      }
  };

};


#endif
