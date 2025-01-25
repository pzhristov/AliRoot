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

/// \file GPUChain.cxx
/// \author David Rohr

#include "GPUChain.h"
using namespace GPUCA_NAMESPACE::gpu;

constexpr GPUChain::krnlRunRange GPUChain::krnlRunRangeNone;
constexpr GPUChain::krnlEvent GPUChain::krnlEventNone;

GPUChain::krnlExec GPUChain::GetGrid(uint32_t totalItems, uint32_t nThreads, int32_t stream, GPUReconstruction::krnlDeviceType d, GPUCA_RECO_STEP st)
{
  const uint32_t nBlocks = (totalItems + nThreads - 1) / nThreads;
  return {nBlocks, nThreads, stream, d, st};
}

GPUChain::krnlExec GPUChain::GetGrid(uint32_t totalItems, int32_t stream, GPUReconstruction::krnlDeviceType d, GPUCA_RECO_STEP st)
{
  return {(uint32_t)-1, totalItems, stream, d, st};
}

GPUChain::krnlExec GPUChain::GetGridBlk(uint32_t nBlocks, int32_t stream, GPUReconstruction::krnlDeviceType d, GPUCA_RECO_STEP st)
{
  return {(uint32_t)-2, nBlocks, stream, d, st};
}

GPUChain::krnlExec GPUChain::GetGridBlkStep(uint32_t nBlocks, int32_t stream, GPUCA_RECO_STEP st)
{
  return {(uint32_t)-2, nBlocks, stream, GPUReconstruction::krnlDeviceType::Auto, st};
}

GPUChain::krnlExec GPUChain::GetGridAuto(int32_t stream, GPUReconstruction::krnlDeviceType d, GPUCA_RECO_STEP st)
{
  return {(uint32_t)-3, 0, stream, d, st};
}

GPUChain::krnlExec GPUChain::GetGridAutoStep(int32_t stream, GPUCA_RECO_STEP st)
{
  return {(uint32_t)-3, 0, stream, GPUReconstruction::krnlDeviceType::Auto, st};
}
