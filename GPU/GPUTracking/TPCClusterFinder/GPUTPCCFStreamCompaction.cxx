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

/// \file StreamCompaction.cxx
/// \author Felix Weiglhofer

#include "GPUTPCCFStreamCompaction.h"
#include "GPUCommonAlgorithm.h"

#include "ChargePos.h"
#include "CfUtils.h"

using namespace GPUCA_NAMESPACE::gpu;
using namespace GPUCA_NAMESPACE::gpu::tpccf;

template <>
GPUdii() void GPUTPCCFStreamCompaction::Thread<GPUTPCCFStreamCompaction::scanStart>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, int32_t iBuf, int32_t stage)
{
  int32_t nElems = CompactionElems(clusterer, stage);

  const auto* predicate = clusterer.mPisPeak;
  auto* scanOffset = clusterer.GetScanBuffer(iBuf);

  int32_t iThreadGlobal = get_global_id(0);
  int32_t pred = 0;
  if (iThreadGlobal < nElems) {
    pred = predicate[iThreadGlobal];
  }

  int32_t nElemsInBlock = CfUtils::blockPredicateSum<GPUCA_THREAD_COUNT_SCAN>(smem, pred);

  int32_t lastThread = nThreads - 1;
  if (iThread == lastThread) {
    scanOffset[iBlock] = nElemsInBlock;
  }
}

template <>
GPUdii() void GPUTPCCFStreamCompaction::Thread<GPUTPCCFStreamCompaction::scanUp>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, int32_t iBuf, int32_t nElems)
{
  auto* scanOffset = clusterer.GetScanBuffer(iBuf - 1);
  auto* scanOffsetNext = clusterer.GetScanBuffer(iBuf);

  int32_t iThreadGlobal = get_global_id(0);
  int32_t offsetInBlock = work_group_scan_inclusive_add((iThreadGlobal < nElems) ? scanOffset[iThreadGlobal] : 0);

  // TODO: This write isn't needed??
  scanOffset[iThreadGlobal] = offsetInBlock;

  int32_t lastThread = nThreads - 1;
  if (iThread == lastThread) {
    scanOffsetNext[iBlock] = offsetInBlock;
  }
}

template <>
GPUdii() void GPUTPCCFStreamCompaction::Thread<GPUTPCCFStreamCompaction::scanTop>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, int32_t iBuf, int32_t nElems)
{
  int32_t iThreadGlobal = get_global_id(0);
  int32_t* scanOffset = clusterer.GetScanBuffer(iBuf - 1);

  bool inBounds = (iThreadGlobal < nElems);

  int32_t offsetInBlock = work_group_scan_inclusive_add(inBounds ? scanOffset[iThreadGlobal] : 0);

  if (inBounds) {
    scanOffset[iThreadGlobal] = offsetInBlock;
  }
}

template <>
GPUdii() void GPUTPCCFStreamCompaction::Thread<GPUTPCCFStreamCompaction::scanDown>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& /*smem*/, processorType& clusterer, int32_t iBuf, uint32_t offset, int32_t nElems)
{
  int32_t iThreadGlobal = get_global_id(0) + offset;

  int32_t* scanOffsetPrev = clusterer.GetScanBuffer(iBuf - 1);
  const int32_t* scanOffset = clusterer.GetScanBuffer(iBuf);

  int32_t shift = scanOffset[iBlock];

  if (iThreadGlobal < nElems) {
    scanOffsetPrev[iThreadGlobal] += shift;
  }
}

template <>
GPUdii() void GPUTPCCFStreamCompaction::Thread<GPUTPCCFStreamCompaction::compactDigits>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUSharedMemory& smem, processorType& clusterer, int32_t iBuf, int32_t stage, ChargePos* in, ChargePos* out)
{
  uint32_t nElems = CompactionElems(clusterer, stage);
  SizeT bufferSize = (stage) ? clusterer.mNMaxClusters : clusterer.mNMaxPeaks;

  uint32_t iThreadGlobal = get_global_id(0);

  const auto* predicate = clusterer.mPisPeak;
  const auto* scanOffset = clusterer.GetScanBuffer(iBuf);

  bool iAmDummy = (iThreadGlobal >= nElems);

  int32_t pred = (iAmDummy) ? 0 : predicate[iThreadGlobal];
  int32_t offsetInBlock = CfUtils::blockPredicateScan<GPUCA_THREAD_COUNT_SCAN>(smem, pred);

  SizeT globalOffsetOut = offsetInBlock;
  if (iBlock > 0) {
    globalOffsetOut += scanOffset[iBlock - 1];
  }

  if (pred && globalOffsetOut < bufferSize) {
    out[globalOffsetOut] = in[iThreadGlobal];
  }

  uint32_t lastId = get_global_size(0) - 1;
  if (iThreadGlobal == lastId) {
    SizeT nFinal = globalOffsetOut + pred;
    if (nFinal > bufferSize) {
      clusterer.raiseError(stage ? GPUErrors::ERROR_CF_CLUSTER_OVERFLOW : GPUErrors::ERROR_CF_PEAK_OVERFLOW, clusterer.mISlice, nFinal, bufferSize);
      nFinal = bufferSize;
    }
    if (stage) {
      clusterer.mPmemory->counters.nClusters = nFinal;
    } else {
      clusterer.mPmemory->counters.nPeaks = nFinal;
    }
  }
}

GPUdii() int32_t GPUTPCCFStreamCompaction::CompactionElems(processorType& clusterer, int32_t stage)
{
  return (stage) ? clusterer.mPmemory->counters.nPeaks : clusterer.mPmemory->counters.nPositions;
}
