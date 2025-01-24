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

/// \file GPUGeneralKernels.cxx
/// \author David Rohr

#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"
using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUdii() void GPUMemClean16::Thread<0>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUglobalref() void* ptr, uint64_t size)
{
  const uint64_t stride = get_global_size(0);
  int4 i0;
  i0.x = i0.y = i0.z = i0.w = 0;
  int4* ptra = (int4*)ptr;
  uint64_t len = (size + sizeof(int4) - 1) / sizeof(int4);
  for (uint64_t i = get_global_id(0); i < len; i += stride) {
    ptra[i] = i0;
  }
}

template <>
GPUdii() void GPUitoa::Thread<0>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUglobalref() int32_t* ptr, uint64_t size)
{
  const uint64_t stride = get_global_size(0);
  for (uint64_t i = get_global_id(0); i < size; i += stride) {
    ptr[i] = i;
  }
}
