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

/// \file GPUTPCDecompressionKernels.h
/// \author Gabriele Cimador

#ifndef GPUTPCDECOMPRESSIONKERNELS_H
#define GPUTPCDECOMPRESSIONKERNELS_H

#include "GPUGeneralKernels.h"
#include "GPUO2DataTypes.h"
#include "GPUParam.h"
#include "GPUConstantMem.h"

#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/CompressedClusters.h"
#else
namespace o2::tpc
{
struct CompressedClusters {
};
} // namespace o2::tpc
#endif

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCDecompressionKernels : public GPUKernelTemplate
{
 public:
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUDataTypes::RecoStep::TPCDecompression; }

  enum K : int32_t {
    step0attached = 0,
    step1unattached = 1,
  };

  template <int32_t iKernel = defaultKernel, typename... Args>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, Args... args);

  GPUd() static uint32_t computeLinearTmpBufferIndex(uint32_t slice, uint32_t row, uint32_t maxClustersPerBuffer)
  {
    return slice * (GPUCA_ROW_COUNT * maxClustersPerBuffer) + row * maxClustersPerBuffer;
  }

  template <typename T>
  GPUdi() static void decompressorMemcpyBasic(T* dst, const T* src, uint32_t size);
};

class GPUTPCDecompressionUtilKernels : public GPUKernelTemplate
{
 public:
  enum K : int32_t {
    countFilteredClusters = 0,
    storeFilteredClusters = 1,
    sortPerSectorRow = 2,
  };

  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors);

  GPUdi() static bool isClusterKept(const o2::tpc::ClusterNative& cl, const GPUParam& GPUrestrict() param);
};

} // namespace GPUCA_NAMESPACE::gpu
#endif // GPUTPCDECOMPRESSIONKERNELS_H
