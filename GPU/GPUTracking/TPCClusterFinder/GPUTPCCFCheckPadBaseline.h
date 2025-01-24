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

/// \file GPUTPCCFCheckPadBaseline.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_GPU_TPC_CF_CHECK_PAD_BASELINE_H
#define O2_GPU_GPU_TPC_CF_CHECK_PAD_BASELINE_H

#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

#include "clusterFinderDefs.h"

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCCFCheckPadBaseline : public GPUKernelTemplate
{

 public:
  enum {
    PadsPerCacheline = 8,
    TimebinsPerCacheline = 4,
    NumOfCachedTimebins = GPUCA_GET_THREAD_COUNT(GPUCA_LB_GPUTPCCFCheckPadBaseline) / PadsPerCacheline,
  };

  struct GPUSharedMemory {
    tpccf::Charge charges[PadsPerCacheline][NumOfCachedTimebins];
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

  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer);

 private:
  GPUd() static ChargePos padToChargePos(int32_t& pad, const GPUTPCClusterFinder&);
  GPUd() static void updatePadBaseline(int32_t pad, const GPUTPCClusterFinder&, int32_t totalCharges, int32_t consecCharges, tpccf::Charge maxCharge);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
