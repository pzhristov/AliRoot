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

/// \file GPUTRDTrackerKernels.cxx
/// \author David Rohr

#include "GPUTRDTrackerKernels.h"
#include "GPUTRDGeometry.h"
#include "GPUConstantMem.h"
#include "GPUCommonTypeTraits.h"
#if defined(WITH_OPENMP) && !defined(GPUCA_GPUCODE)
#include "GPUReconstruction.h"
#endif

using namespace GPUCA_NAMESPACE::gpu;

template <int32_t I, class T>
GPUdii() void GPUTRDTrackerKernels::Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, T* externalInstance)
{
  auto* trdTracker = &processors.getTRDTracker<I>();
#ifndef GPUCA_GPUCODE_DEVICE
  if constexpr (std::is_same_v<decltype(trdTracker), decltype(externalInstance)>) {
    if (externalInstance) {
      trdTracker = externalInstance;
    }
  }
#endif
  GPUCA_OPENMP(parallel for if(!trdTracker->GetRec().GetProcessingSettings().ompKernels) num_threads(trdTracker->GetRec().GetProcessingSettings().ompThreads))
  for (int32_t i = get_global_id(0); i < trdTracker->NTracks(); i += get_global_size(0)) {
    trdTracker->DoTrackingThread(i, get_global_id(0));
  }
}

#if !defined(GPUCA_GPUCODE) || defined(GPUCA_GPUCODE_DEVICE) // FIXME: DR: WORKAROUND to avoid CUDA bug creating host symbols for device code.
template GPUdni() void GPUTRDTrackerKernels::Thread<0>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, GPUTRDTrackerGPU* externalInstance);
#ifdef GPUCA_HAVE_O2HEADERS
template GPUdni() void GPUTRDTrackerKernels::Thread<1>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, GPUTRDTracker* externalInstance);
#endif // GPUCA_HAVE_O2HEADERS
#endif
