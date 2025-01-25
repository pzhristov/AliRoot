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

/// \file GPUTPCTrackletSelector.h
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#ifndef GPUTPCTRACKLETSELECTOR_H
#define GPUTPCTRACKLETSELECTOR_H

#include "GPUTPCDef.h"
#include "GPUTPCHitId.h"
#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUTPCTracker;

/**
 * @class GPUTPCTrackletSelector
 *
 */
class GPUTPCTrackletSelector : public GPUKernelTemplate
{
 public:
  struct GPUSharedMemory {
    int32_t mItr0;          // index of the first track in the block
    int32_t mNThreadsTotal; // total n threads
    int32_t mNTracklets;    // n of tracklets
    int32_t mReserved;      // for alignment reasons
#if GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
    GPUTPCHitId mHits[GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE][GPUCA_GET_THREAD_COUNT(GPUCA_LB_GPUTPCTrackletSelector)];
#endif // GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
  };

  typedef GPUconstantref() GPUTPCTracker processorType;
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUCA_RECO_STEP::TPCSliceTracking; }
  GPUhdi() static processorType* Processor(GPUConstantMem& processors)
  {
    return processors.tpcTrackers;
  }
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& tracker);
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUTPCTRACKLETSELECTOR_H
