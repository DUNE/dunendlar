// standalone unit test of the XYZ positions lookup for the
// ND-LAr Module 0 channel map
// compile with  
// g++ -g -I${DUNENDLAR_INC} -o m0testmap m0testmap.C ../NDLArModule0ChannelMapSP.cxx

#include "dunendlar/Prototypes/Module0/ChannelMap/NDLArModule0ChannelMapSP.h"
#include <iostream>
#include <cmath>

int main(int argc, char **argv)
{
  std::string filename = "../module0_pixel_layout_2.2.16.dat";
  dune::NDLArModule0ChannelMapSP mapsp;
  std::vector<double> zoff;
  zoff.push_back(0.0);
  zoff.push_back(0.0);

  mapsp.ReadMapFromFile(filename,zoff,zoff,zoff);
  unsigned int nchans = 16*49*100;
  for (unsigned int ichan=0; ichan<nchans; ++ichan)
    {
      auto cinfo1 = mapsp.GetChanInfoFromOfflChan(ichan);
      if (cinfo1.offlinechan != ichan)
	{
	  std::cout << "cinfo1 ichan number mismatch: " << cinfo1.offlinechan << " " << ichan << std::endl;
	}
      
      auto cinfo2 = mapsp.GetChanInfoFromXYZ(cinfo1.xyz[0],cinfo1.xyz[1],cinfo1.xyz[2]);
      if (cinfo1.offlinechan != cinfo2.offlinechan)
	{
	  std::cout << "cinfo1 cinfo2 channel number mismatch: " << cinfo1.offlinechan << " " << cinfo2.offlinechan 
		    << " " << cinfo1.xyz[0] << " " << cinfo1.xyz[1] << " " << cinfo1.xyz[2] << std::endl;
	  std::cout <<  cinfo2.xyz[0] << " " << cinfo2.xyz[1] << " " << cinfo2.xyz[2] << std::endl;
	}
      //if ( !cinfo2.valid )
      //	{
      //  std::cout << "invalid cinfo2 " << cinfo2.offlinechan << " " << cinfo1.offlinechan 
      //	    << " " << cinfo1.xyz[0] << " " << cinfo1.xyz[1] << " " << cinfo1.xyz[2] << std::endl;
      //  std::cout <<  cinfo2.xyz[0] << " " << cinfo2.xyz[1] << " " << cinfo2.xyz[2] << std::endl;
      //	}

      double r = 3.0;
      auto cinfolist = mapsp.GetChanInfoFromXYZWithNeighbors(cinfo1.xyz[0],cinfo1.xyz[1],cinfo1.xyz[2],r);
      if (cinfolist.empty())
	{
	  // should at least get one channel, even if it's invalid
	  std::cout << "empty cinfolist when calling the XYZ with neighbors method" << std::endl;
	}
      for (const auto &cn : cinfolist)
	{
	  double rtest = std::sqrt( (cinfo1.xyz[0]-cn.xyz[0])*(cinfo1.xyz[0]-cn.xyz[0]) +
                                    (cinfo1.xyz[1]-cn.xyz[1])*(cinfo1.xyz[1]-cn.xyz[1]) +
			            (cinfo1.xyz[2]-cn.xyz[2])*(cinfo1.xyz[2]-cn.xyz[2]));

	  if (rtest > r + 0.5)
	    {
	      std::cout << "neighbor too far away" << std::endl;
	    }
	}
    }
}
