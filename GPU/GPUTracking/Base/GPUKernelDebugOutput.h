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

/// \file GPUGeneralKernels.h
/// \author David Rohr

#ifndef O2_GPU_KERNELDEBUGOUTPUT_H
#define O2_GPU_KERNELDEBUGOUTPUT_H

#include "GPUDef.h"
#include "GPUProcessor.h"
#ifdef GPUCA_KERNEL_DEBUGGER_OUTPUT

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUKernelDebugOutput : public GPUProcessor
{
 public:
#ifndef GPUCA_GPUCODE
  void InitializeProcessor();
  void RegisterMemoryAllocation();
  void SetMaxData(const GPUTrackingInOutPointers& io);

  void* SetPointersMemory(void* mem);

  void Print()
  {
    printf("------ Kernel Debug Output\n");
    for (int32_t i = 0; i < 100 * 1024; i++) {
      int32_t* pos = mDebugOutMemory + i * 1024;
      int32_t count = *(pos++);
      if (count) {
        printf("Thread %d: ", i);
        for (int32_t j = 0; j < count; j++) {
          printf("%d, ", pos[j]);
        }
        printf("\n");
      }
    }
    printf("------ End of Kernel Debug Output\n");
  }
#endif
  GPUdi() int32_t* memory()
  {
    return mDebugOutMemory;
  }
  GPUdi() static size_t memorySize() { return 100 * 1024 * 1024; }

  GPUd() void Add(uint32_t id, int32_t val) const
  {
    printf("Filling debug: id %d, val %d, current count %d\n", id, val, *(mDebugOutMemory + id * 1024));
    if (id > 100 * 1024) {
      return;
    }
    int32_t* pos = mDebugOutMemory + id * 1024;
    if (*pos >= 1023) {
      return;
    }
    pos += ++(*pos);
    *pos = val;
  }

 private:
  mutable int32_t* mDebugOutMemory;
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
#endif
