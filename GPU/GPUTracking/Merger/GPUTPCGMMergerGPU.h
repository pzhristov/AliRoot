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

/// \file GPUTPCGMMergerGPU.h
/// \author David Rohr

#ifndef GPUTPCGMMERGERGPUCA_H
#define GPUTPCGMMERGERGPUCA_H

#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"
#include "GPUTPCGMMergerTypes.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUTPCGMMergerGeneral : public GPUKernelTemplate
{
 public:
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUDataTypes::RecoStep::TPCMerging; }
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  typedef GPUTPCGMMerger processorType;
  GPUhdi() static processorType* Processor(GPUConstantMem& processors)
  {
    return &processors.tpcMerger;
  }
#endif
};

class GPUTPCGMMergerTrackFit : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t mode);
#endif
};

class GPUTPCGMMergerFollowLoopers : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerSliceRefit : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t iSlice);
#endif
};

class GPUTPCGMMergerUnpackGlobal : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t iSlice);
#endif
};

class GPUTPCGMMergerUnpackSaveNumber : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t id);
#endif
};

class GPUTPCGMMergerUnpackResetIds : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t id);
#endif
};

class GPUTPCGMMergerResolve : public GPUTPCGMMergerGeneral
{
 public:
  struct GPUSharedMemory : public gputpcgmmergertypes::GPUResolveSharedMemory {
  };

#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel, typename... Args>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, Args... args);
#endif
};

class GPUTPCGMMergerClearLinks : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int8_t nOutput);
#endif
};

class GPUTPCGMMergerMergeWithinPrepare : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerMergeSlicesPrepare : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, int32_t border0, int32_t border1, int8_t useOrigTrackParam);
#endif
};

class GPUTPCGMMergerMergeBorders : public GPUTPCGMMergerGeneral
{
 public:
  enum K { defaultKernel = 0,
           step0 = 0,
           step1 = 1,
           step2 = 2,
           variant = 3 };
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel, typename... Args>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger, Args... args);
#endif
};

class GPUTPCGMMergerMergeCE : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerLinkGlobalTracks : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerCollect : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerPrepareClusters : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerSortTracks : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerSortTracksQPt : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerSortTracksPrepare : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerFinalize : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

class GPUTPCGMMergerMergeLoopers : public GPUTPCGMMergerGeneral
{
 public:
#if !defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE)
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
#endif
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
