///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       NDLArModule0ChannelMapSP
// Module type: standalone algorithm
// File:        NDLArModule0ChannelMapSP.cxx
// Author:      Tom Junk, September 26, 2022
//
// Implementation of hardware-offline channel mapping for ND-LAr Module 0, reading from one file
//
// The SP in the class and file name means "Service Provider"
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "dunendlar/Prototypes/Module0/ChannelMap/NDLArModule0ChannelMapSP.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

dune::NDLArModule0ChannelMapSP::NDLArModule0ChannelMapSP()
{
  fInitialized = false;   // set to true when we read the map from the file. Use to protect against second initialization
                          // to ensure thread safety.
}

void dune::NDLArModule0ChannelMapSP::ReadMapFromFile(const std::string &chanmapfile,
                                                     std::vector<double> &anodexoffset,
                                                     std::vector<double> &anodeyoffset,
                                                     std::vector<double> &anodezoffset)
{

  if (fInitialized)   // ignore second and subsequent initializations
    {
      return;
    }

  std::ifstream inFile(chanmapfile, std::ios::in);
  std::string line;

  std::getline(inFile,line);
  std::string tile_layout_version = line;

  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> fPixelPitch;
  }
  fPixelPitch /= 10.0;


  typedef struct ccpos_struct
  {
    unsigned p0;
    unsigned p1;
  } ccpos_t;
  std::unordered_map<unsigned, std::unordered_map<unsigned, ccpos_t > > ccpos; // chip and chipchan keys

  // calculate min and max local x and y (in Peter Madigan's geometry.py coordinates; x and y are in the pixel plane)
  unsigned xmin=0;
  unsigned xmax=0;
  unsigned ymin=0;
  unsigned ymax=0;

  std::set<unsigned> ccset;

  size_t nread = 0;
  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }

  for (size_t i=0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      ccpos_t pos;
      unsigned chip=0;
      unsigned chipchan=0;
      linestream >> chip >> chipchan >> pos.p0 >> pos.p1;
      ccpos[chip][chipchan] = pos;
      ccset.emplace(chipchan);
      if (i == 0 || pos.p0 < xmin)
        {
          xmin = pos.p0;
        }
      if (i == 0 || pos.p1 < ymin)
        {
          ymin = pos.p1;
        }
      if (i == 0 || pos.p0 > xmax)
        {
          xmax = pos.p0;
        }
      if (i == 0 || pos.p1 > ymax)
        {
          ymax = pos.p1;
        }
    }

  double x_size = (xmax - xmin + 1) * fPixelPitch;
  double y_size = (ymax - ymin + 1) * fPixelPitch;
  double half_x_size = x_size/2.0;
  double half_y_size = y_size/2.0;
  // std::cout << "x, y sizes: " << x_size << " " << y_size << std::endl;
  // std::cout << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;

  typedef struct tileioinfo_struct
  {
    unsigned io_group;
    unsigned io_channel;
  } tileioinfo_t;
  std::unordered_map<unsigned, std::unordered_map<unsigned, tileioinfo_t > > tileioinfo;  // tile, chip are keys

  // 16 tiles x 100 chips each.  packed tight

  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }

  for (size_t i = 0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      tileioinfo_t t;
      unsigned tile=0;
      unsigned chip=0;
      linestream >> tile >> chip >> t.io_group >> t.io_channel;
      tileioinfo[tile][chip] = t;
      //std::cout << "tcio: " << tile << " " << chip << " " << t.io_group << " " << t.io_channel << std::endl;
    }

  // one per tile. indices are non-negative

  typedef struct tileindex_struct
  {
    unsigned i0;
    unsigned i1;
  } tileindex_t;
  std::unordered_map<unsigned, tileindex_t> tileindex;

  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }

  for (size_t i = 0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      tileindex_t t;
      unsigned tile=0;
      linestream >> tile >> t.i0 >> t.i1;
      tileindex[tile] = t;
      //std::cout << "tilei: " << tile << " " << t.i0 << " " << t.i1 << std::endl;
    }

  // orientations are signed integers (+-1).  One struct per tile

  typedef struct tileorientation_struct
  {
    int v0;
    int v1;
    int v2;
  } tileorientation_t;
  std::unordered_map<unsigned, tileorientation_t> tileorientation;

  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }

  for (size_t i = 0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      tileorientation_t t;
      unsigned tile = 0;
      linestream >> tile >> t.v0 >> t.v1 >> t.v2;
      tileorientation[tile] = t;
      //std::cout << "tileo: " << tile << " " << t.v0 << " " << t.v1 << " " << t.v2 << std::endl;
    }

  typedef struct tileposition_struct
  {
    double v0;
    double v1;
    double v2;
  } tileposition_t;
  std::unordered_map<unsigned, tileposition_t> tileposition;
  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }
  for (size_t i = 0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      unsigned tile=0;
      tileposition_t t;
      linestream >> tile >> t.v0 >> t.v1 >> t.v2;
      t.v0 /= 10.0;
      t.v1 /= 10.0;
      t.v2 /= 10.0;
      tileposition[tile] = t;
    }

  typedef struct tpccenter_struct
  {
    double v0;
    double v1;
    double v2;
  } tpccenter_t;
  std::unordered_map<unsigned, tpccenter_t> tpccenter;
  std::getline(inFile,line);
  { 
    std::stringstream linestream(line);
    linestream >> nread;
  }
  for (size_t i = 0; i < nread; ++i)
    {
      std::getline(inFile,line);
      std::stringstream linestream(line);
      tpccenter_t t;
      unsigned tpc=0;
      linestream >> tpc >> t.v0 >> t.v1 >> t.v2;
      t.v0 /= 10.0;
      t.v1 /= 10.0;
      t.v2 /= 10.0;
      tpccenter[tpc] = t;
    }

  // collect pixel information
  // IO_Group, IO_Channel, tile, chip, chan_in_chip, x, y z 
  // in larsoft coordinates.

  std::vector<NDLArModule0ChanInfo_t> pinfo;

  for (unsigned tile=1; tile<=16; ++tile)
    {
      double tpox = tpccenter[tileindex[tile].i1].v0 + tileposition[tile].v2;
      double tpoy = tpccenter[tileindex[tile].i1].v1 + tileposition[tile].v1;
      int tileo1 = tileorientation[tile].v1;
      int tileo2 = tileorientation[tile].v2;
      //std::cout << "tileo: " << tileo1 << " " << tileo2 << std::endl;

      for (unsigned chip=11; chip<111; ++chip)
        {
	  // look up io for this chip on this tile.  Set to zero if not found

	  unsigned io_group = 0;
	  unsigned io_channel = 0;
          auto tip = tileioinfo.find(tile);
          if (tip != tileioinfo.end())
	    {
               auto& tip2 = tip->second;
               auto tip3 = tip2.find(chip);
               if (tip3 != tip2.end())
		 {
                   auto tinfo = tip3->second;
		   io_group = tinfo.io_group;
                   io_channel = tinfo.io_channel;
		 }
	    }

          for (unsigned chipchan=0; chipchan<64; ++chipchan)
            {
              if (ccset.find(chipchan) == ccset.end()) continue;
              auto pos = ccpos[chip][chipchan];
              unsigned p0 = pos.p0;
              unsigned p1 = pos.p1;           

              double x = (p0 + 0.5) * fPixelPitch - half_x_size;
              double y = (p1 + 0.5) * fPixelPitch - half_y_size;

              x *= tileo2;
              y *= tileo1;

              x += tpox;
              y += tpoy;

              NDLArModule0ChanInfo_t p;
              p.offlinechan = 0;
              p.tile = tile;
              p.chip = chip;
              p.io_group = io_group;
              p.io_channel = io_channel;
              p.chipchannel = chipchan;
	      size_t ioffsetindex = 0;
	      if (io_group == 2) ioffsetindex = 1;
              p.xyz[2] = x + anodezoffset.at(ioffsetindex);    // switch to larsoft axes
              p.xyz[1] = y + anodeyoffset.at(ioffsetindex);
	      p.xyz[0] = tileposition[tile].v0 + tpccenter[tileindex[tile].i1].v2 + anodexoffset.at(ioffsetindex);
	      //std::cout << "tile: " << tile << " x: " << p.xyz[0] << " " << io_group << std::endl;
              p.valid = true;
	      if (p.io_group == 0) p.valid = false;
              pinfo.push_back(p);
            }
        }
    }
  std::sort(pinfo.begin(), pinfo.end(), 
            [](const NDLArModule0ChanInfo_t & a, const NDLArModule0ChanInfo_t & b) -> bool
            {
              double eps = 0.001;
              if (a.xyz[0] < b.xyz[0] - eps) return true;
              if (a.xyz[0] < b.xyz[0] + eps)
                {
                  if (a.xyz[1] < b.xyz[1] - eps) return true;
                  if (a.xyz[1] < b.xyz[1] + eps)
                    {
                      if (a.xyz[2] < b.xyz[2] - eps) return true;
                    }
                }
              return false;
            });

  // save information in the fDetToChanInfo map and the fOfflToChanInfo lookup table of pointers

  fOfflToChanInfo.resize(pinfo.size());

  fNChans = pinfo.size();

  for (size_t i=0; i<pinfo.size(); ++i)
    {
      pinfo.at(i).offlinechan = i;
      auto p = pinfo.at(i);
      fDetToChanInfo[p.io_group][p.tile][p.chip][p.chipchannel] = p;

      //std::cout << p.offlinechan << " " << p.tile << " " << p.chip << " " << p.io_group << 
      // " " << p.io_channel << " " << p.chipchannel  << " " << p.xyz[0] << " " << p.xyz[1] << " " << p.xyz[2] <<std::endl;

      fOfflToChanInfo.at(p.offlinechan) = &fDetToChanInfo[p.io_group][p.tile][p.chip][p.chipchannel];      
    }

  fInitialized = true;

  // calculate some parameters needed for the position to channel formulas
  // split into a separate method for readability.  This method calls GetChanInfoFromOfflChan,
  // so the fInitialized flag needs to be set

  InitializeChanLocParams();
}

void dune::NDLArModule0ChannelMapSP::InitializeChanLocParams()
{
  auto c0info = GetChanInfoFromOfflChan(0);      // corner channel.  Already have pitch
  auto cztile = GetChanInfoFromOfflChan(7*10);   // first channel in the next tile over in z
  uint32_t nchanspertile = 4900;
  auto cytile = GetChanInfoFromOfflChan(2*nchanspertile);  // first channel one tile up in y

  fNChansPerSide = 8*nchanspertile;
  fNChansPerTRow = 7*10;             // channels along an edge of a tile
  fNChansPerRow = 2*fNChansPerTRow;  // channels along edge of two tiles -- one row in y
  fNChansPerTileRow = 2*nchanspertile;
  auto lastchan = GetChanInfoFromOfflChan(fNChans - 1);

  fXMin = c0info.xyz[0];
  fYMin = c0info.xyz[1];  // to check -- half a pixel pitch offset needed?
  fZMin = c0info.xyz[2];  // to check -- half a pixel pitch offset needed?
  fXMax = lastchan.xyz[0];
  fYMax = lastchan.xyz[1]; // to check -- half a pixel pitch offset needed?
  fZMax = lastchan.xyz[2]; // to check -- half a pixel pitch offset needed?

  fYTileSep = cytile.xyz[1] - c0info.xyz[1];
  fZTileSep = cztile.xyz[2] - c0info.xyz[2];

  fTPCCathodeLoc = 0.5*(fXMin + fXMax);
}

dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t 
dune::NDLArModule0ChannelMapSP::GetChanInfoFromElectronics(
                                                           unsigned int io_group,
                                                           unsigned int io_channel,
                                                           unsigned int chip,
                                                           unsigned int chipchannel ) const {

  if (!fInitialized)
    {
      throw std::logic_error("NDLArModule0 channel map lookup GetChanInfoFromElectronics called before initialized");
    }

  NDLArModule0ChanInfo_t badInfo = {};
  badInfo.valid = false;

  if (io_group != 1 && io_group != 2) return badInfo;
  if (chip < 11 || chip > 111) return badInfo;
  if (io_channel == 0 || io_channel > 32) return badInfo;
  if (chipchannel > 63) return badInfo;

  unsigned int tile = ((io_channel -1) >> 2) + 1 + 8*(io_group - 1);

  auto fm1 = fDetToChanInfo.find(io_group);
  if (fm1 == fDetToChanInfo.end()) return badInfo;
  auto &m1 = fm1->second;

  auto fm2 = m1.find(tile);
  if (fm2 == m1.end()) return badInfo;
  auto &m2 = fm2->second;

  auto fm3 = m2.find(chip);
  if (fm3 == m2.end()) return badInfo;
  auto &m3 = fm3->second;

  auto fm4 = m3.find(chipchannel);
  if (fm4 == m3.end()) return badInfo;
  auto chaninfo = fm4->second;

  return chaninfo;
}


dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t dune::NDLArModule0ChannelMapSP::GetChanInfoFromOfflChan(unsigned int offlineChannel) const {

  if (!fInitialized)
    {
      throw std::logic_error("NDLArModule0 channel map lookup GetChanInfoFromOfflChan called before initialized");
    }

  if (offlineChannel >= fNChans)
    {
      NDLArModule0ChanInfo_t badInfo = {};
      badInfo.valid = false;
      return badInfo;
    }

  NDLArModule0ChanInfo_t outputinfo = *fOfflToChanInfo.at(offlineChannel);

  return outputinfo;
}

// Compute offline channel number algorithmically, and then look up the channel info from the LUT

dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t dune::NDLArModule0ChannelMapSP::GetChanInfoFromXYZ(double x, double y, double z) const {

  if (!fInitialized)
    {
      throw std::logic_error("NDLArModule0 channel map lookup GetChanInfoFromXYZ called before initialized");
    }

  NDLArModule0ChanInfo_t badInfo = {};
  badInfo.valid = false;
  if (x < fXMin || x > fXMax || y < fYMin || y > fYMax || z < fZMin || z > fZMax)
    {
      return badInfo;
    } 

  unsigned int offlineChannel = 0;

  unsigned int xCo = 0;     // x-channel offset
  if (x > fTPCCathodeLoc)
    {
      xCo = fNChansPerSide;
    }
  
  unsigned int iYTile = (y - fYMin)/fYTileSep;                // truncate to an integer
  unsigned int iYRow = 0.5 + (y - fYMin - iYTile*fYTileSep)/fPixelPitch;

  unsigned int iZTile = (z -fZMin)/fZTileSep;
  unsigned int iZColumn = 0.5 + (z - fZMin - iZTile*fZTileSep)/fPixelPitch;

  offlineChannel = iZColumn + fNChansPerTRow*iZTile + fNChansPerRow*iYRow + fNChansPerTileRow*iYTile + xCo;
  return GetChanInfoFromOfflChan(offlineChannel);
}


std::vector<dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t>
  dune::NDLArModule0ChannelMapSP::GetChanInfoFromXYZWithNeighbors(double x, double y, double z, double r) const {

  if (r<0)
    {
      throw std::range_error("NDLArModule0ChannelMapSP::GetChanInfoFromXYZWithNeighbors Negative Radius");
    }

  std::vector<dune::NDLArModule0ChannelMapSP::NDLArModule0ChanInfo_t> cvec;
  cvec.push_back(GetChanInfoFromXYZ(x,y,z));   // first element is the channel in the center

  // search for neighbors in a box

  std::set<uint32_t> idset;
  double ycent = cvec.back().xyz[1];
  double zcent = cvec.back().xyz[1];
  double eps = 0.01;
  double rsq = r*r;
  size_t irad = (size_t) ((r-eps)/fPixelPitch + 1.0);
  double fp2 = fPixelPitch/2.0;

  double ymin = ycent - irad*fPixelPitch;
  double zmin = zcent - irad*fPixelPitch;
  size_t nsteps = 2*irad + 1;

  // to think about -- this logic assumes the pixels are on a regular grid and tries to be
  // a bit conservative about what to include.  But if some neighbors are on a neighboring tile,
  // the sub-pixel distances may be inaccurate

  for (size_t iy=0; iy<nsteps; ++iy)
    {
      double yseek = ymin + iy*fPixelPitch;
      for (size_t iz=0; iz<nsteps; ++iz)
	{
	  double zseek = zmin + iz*fPixelPitch;
	  double rseeksq = (yseek - y)*(yseek - y) + (zseek - z)*(zseek - z);
	  bool inside = false;
	  if (rseeksq < rsq + eps) 
	    {
	      inside = true;
	    }
	  else  // center is outside -- check the corners to see if any of them are inside the circle
	    {
	      for (int icy=-1;icy<=1; icy +=2 )
		{
		  double ysc = yseek + icy*fp2;
		  for (int icz=-1;icz<=1; icz += 2)
		    {
		      double zsc = zseek + icz*fp2;
 	              double rsc = (ysc - y)*(ysc - y) + (zsc - z)*(zsc - z);
		      if (rsc < rsq + eps)
			{
			  inside = true;
			  break;
			}
		    }
		}
	    }
	  if (inside)
	    {
	      auto pn = GetChanInfoFromXYZ(x,yseek,zseek);
	      if (pn.offlinechan != cvec.back().offlinechan && pn.valid)
		{
		  idset.emplace(pn.offlinechan);
		}
	    }
	}
    }
  for (const auto &si : idset)
    {
      cvec.push_back(GetChanInfoFromOfflChan(si));
    }
  return cvec;
}
