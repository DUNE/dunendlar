////////////////////////////////////////////////////////////////////////
//
// RawPixel.h
// Data product to save ND LAr raw data 
// Tingjun Yang, August 18, 2022
//
////////////////////////////////////////////////////////////////////////

#ifndef  RAWPIXEL_H
#define  RAWPIXEL_H

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::Compress_t, raw::Channel_t

#include "RtypesCore.h"
#include <stdint.h>

namespace raw {

  class RawPixel
  {

  public:

  RawPixel() : fChannel(0), fADC(0), fTimeStamp(0) {}; // Default constructor

  RawPixel(ChannelID_t channel,
        short       ADC,
        uint32_t    timeStamp)
    : fChannel(channel), 
      fADC(ADC),
      fTimeStamp(timeStamp) {};

    ChannelID_t GetChannel()   const { return fChannel; };
    short       GetADC()       const { return fADC; };
    uint32_t    GetTimeStamp() const { return fTimeStamp; };

  private:

    ChannelID_t fChannel;   ///< Channel number
    short       fADC;       ///< ADC value of the packet
    uint32_t    fTimeStamp; ///< Time stamp of the packet

  };


} // namespace raw

#endif // RawPixel_H
