//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUReconstructionHelpers.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONHELPERS_H
#define GPURECONSTRUCTIONHELPERS_H

#include <mutex>

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUReconstructionDeviceBase;
class GPUReconstructionHelpers
{
 public:
  class helperDelegateBase
  {
  };

  struct helperParam {
    pthread_t threadId;
    GPUReconstructionDeviceBase* cls;
    int32_t num;
    std::mutex mutex[2];
    int8_t terminate;
    helperDelegateBase* functionCls;
    int32_t (helperDelegateBase::*function)(int32_t, int32_t, helperParam*);
    int32_t phase;
    int32_t count;
    volatile int32_t done;
    volatile int8_t error;
    volatile int8_t reset;
  };
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
