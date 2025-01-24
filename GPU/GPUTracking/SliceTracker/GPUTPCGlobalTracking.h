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

/// \file GPUTPCGlobalTracking.h
/// \author David Rohr

#ifndef GPUTPCGLOBALTRACKING_H
#define GPUTPCGLOBALTRACKING_H

#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUTPCTracker;

class GPUTPCGlobalTracking : public GPUKernelTemplate
{
 public:
  struct GPUSharedMemory {
    CA_SHARED_STORAGE(GPUTPCRow mRows[GPUCA_ROW_COUNT]);
  };

  typedef GPUconstantref() GPUTPCTracker processorType;
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUCA_RECO_STEP::TPCSliceTracking; }
  GPUhdi() static processorType* Processor(GPUConstantMem& processors)
  {
    return processors.tpcTrackers;
  }
  template <int32_t iKernel = GPUKernelTemplate::defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& tracker);

  GPUd() static int32_t GlobalTrackingSliceOrder(int32_t iSlice);
  GPUd() static void GlobalTrackingSliceLeftRight(uint32_t iSlice, uint32_t& left, uint32_t& right);

 private:
  GPUd() static int32_t PerformGlobalTrackingRun(GPUTPCTracker& tracker, GPUsharedref() GPUSharedMemory& smem, const GPUTPCTracker& sliceSource, int32_t iTrack, int32_t rowIndex, float angle, int32_t direction);
  GPUd() static void PerformGlobalTracking(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, const GPUTPCTracker& tracker, GPUsharedref() GPUSharedMemory& smem, GPUTPCTracker& sliceTarget, bool right);
};

class GPUTPCGlobalTrackingCopyNumbers : public GPUKernelTemplate
{
 public:
  typedef GPUconstantref() GPUTPCTracker processorType;
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUCA_RECO_STEP::TPCSliceTracking; }
  GPUhdi() static processorType* Processor(GPUConstantMem& processors)
  {
    return processors.tpcTrackers;
  }
  template <int32_t iKernel = GPUKernelTemplate::defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& tracker, int32_t n);
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUTPCTRACKLETCONSTRUCTOR_H
