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

/// \file PeakFinder.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_PEAK_FINDER_H
#define O2_GPU_PEAK_FINDER_H

#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

#include "clusterFinderDefs.h"
#include "Array2D.h"
#include "PackedCharge.h"

namespace GPUCA_NAMESPACE::gpu
{

struct ChargePos;

class GPUTPCCFPeakFinder : public GPUKernelTemplate
{
 public:
  static constexpr size_t SCRATCH_PAD_WORK_GROUP_SIZE = GPUCA_GET_THREAD_COUNT(GPUCA_LB_GPUTPCCFPeakFinder);
  struct GPUSharedMemory : public GPUKernelTemplate::GPUSharedMemoryScan64<int16_t, SCRATCH_PAD_WORK_GROUP_SIZE> {
    ChargePos posBcast[SCRATCH_PAD_WORK_GROUP_SIZE];
    PackedCharge buf[SCRATCH_PAD_WORK_GROUP_SIZE * SCRATCH_PAD_SEARCH_N];
  };

#ifdef GPUCA_HAVE_O2HEADERS
  typedef GPUTPCClusterFinder processorType;
  GPUhdi() static processorType* Processor(GPUConstantMem& processors)
  {
    return processors.tpcClusterer;
  }
#endif

  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep()
  {
    return GPUDataTypes::RecoStep::TPCClusterFinding;
  }

  template <int32_t iKernel = defaultKernel, typename... Args>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, Args... args);

 private:
  static GPUd() void findPeaksImpl(int32_t, int32_t, int32_t, int32_t, GPUSharedMemory&, const Array2D<PackedCharge>&, const uint8_t*, const ChargePos*, tpccf::SizeT, const GPUSettingsRec&, const TPCPadGainCalib&, uint8_t*, Array2D<uint8_t>&);

  static GPUd() bool isPeak(GPUSharedMemory&, tpccf::Charge, const ChargePos&, uint16_t, const Array2D<PackedCharge>&, const GPUSettingsRec&, ChargePos*, PackedCharge*);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
