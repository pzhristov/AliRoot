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

/// \file ChargeMapFiller.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_CHARGE_MAP_FILLER_H
#define O2_GPU_CHARGE_MAP_FILLER_H

#include "clusterFinderDefs.h"
#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"
#include "GPUTPCClusterFinder.h"
#include "Array2D.h"
#include "PackedCharge.h"

namespace o2::tpc
{
class Digit;
} // namespace o2::tpc

namespace GPUCA_NAMESPACE::gpu
{

struct ChargePos;

class GPUTPCCFChargeMapFiller : public GPUKernelTemplate
{
 public:
  enum K : int32_t {
    fillIndexMap,
    fillFromDigits,
    findFragmentStart,
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

  static GPUd() void fillIndexMapImpl(int32_t, int32_t, int32_t, int32_t, const CfFragment&, const tpc::Digit*, Array2D<uint32_t>&, size_t);

  static GPUd() void fillFromDigitsImpl(int32_t, int32_t, int32_t, int32_t, processorType&, const CfFragment&, size_t, const tpc::Digit*, ChargePos*, Array2D<PackedCharge>&);

 private:
  static GPUd() size_t findTransition(int32_t, const tpc::Digit*, size_t, size_t);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
