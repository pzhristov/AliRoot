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

/// \file GPUTrackingRefitKernel.cxx
/// \author David Rohr

#include "GPUROOTDump.h"
#include "GPUTrackingRefitKernel.h"
#include "GPUTrackingRefit.h"

using namespace GPUCA_NAMESPACE::gpu;

template <int32_t I>
GPUdii() void GPUTrackingRefitKernel::Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors)
{
  auto& refit = processors.trackingRefit;
  for (uint32_t i = get_global_id(0); i < processors.ioPtrs.nMergedTracks; i += get_global_size(0)) {
    if (refit.mPTracks[i].OK()) {
      GPUTPCGMMergedTrack trk = refit.mPTracks[i];
      int32_t retval;
      if constexpr (I == mode0asGPU) {
        retval = refit.RefitTrackAsGPU(trk, false, true);
      } else if constexpr (I == mode1asTrackParCov) {
        retval = refit.RefitTrackAsTrackParCov(trk, false, true);
      }
      /*#pragma omp critical
      if (retval > 0) {
        static auto cldump = GPUROOTDump<GPUTPCGMMergedTrack, GPUTPCGMMergedTrack>::getNew("org", "refit", "debugTree");
        cldump.Fill(refit.mPTracks[i], trk);
      }*/
      if (retval > 0) {
        refit.mPTracks[i] = trk;
      } else {
        refit.mPTracks[i].SetOK(false);
      }
    }
  }
}
#if !defined(GPUCA_GPUCODE) || defined(GPUCA_GPUCODE_DEVICE) // FIXME: DR: WORKAROUND to avoid CUDA bug creating host symbols for device code.
template GPUdni() void GPUTrackingRefitKernel::Thread<0>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors);
template GPUdni() void GPUTrackingRefitKernel::Thread<1>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors);
#endif
