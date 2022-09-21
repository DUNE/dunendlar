////////////////////////////////////////////////////////////////////////
//
// Module0Trigger.h
// Data product to save ND LAr raw data 
// Tom Junk, September 21, 2022
//
////////////////////////////////////////////////////////////////////////

#ifndef  MODULE0TRIGGER_H
#define  MODULE0TRIGGER_H

#include "RtypesCore.h"
#include <stdint.h>

namespace raw {

  class Module0Trigger
  {

  public:

  Module0Trigger() : fTriggerBits(0), fTimeStamp(0) {}; // Default constructor

  Module0Trigger(uint8_t     triggerBits,
                 uint32_t    timeStamp)
    : fTriggerBits(triggerBits), 
      fTimeStamp(timeStamp) {};

    uint8_t     GetTriggerBits()   const { return fTriggerBits; };
    uint32_t    GetTimeStamp()     const { return fTimeStamp; };

  private:

    uint8_t fTriggerBits;   ///< Trigger Bits
    uint32_t    fTimeStamp; ///< Time stamp of the packet
  };


} // namespace raw

#endif // MODULE0TRIGGER_H
