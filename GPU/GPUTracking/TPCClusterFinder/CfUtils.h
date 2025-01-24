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

/// \file CfUtils.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_CF_UTILS_H
#define O2_GPU_CF_UTILS_H

#include "clusterFinderDefs.h"
#include "GPUCommonAlgorithm.h"
#include "Array2D.h"
#include "CfConsts.h"

namespace GPUCA_NAMESPACE::gpu
{

class CfUtils
{

 public:
  static GPUdi() bool isAtEdge(const ChargePos& pos, tpccf::GlobalPad padsPerRow)
  {
    return (pos.pad() < 2 || pos.pad() >= padsPerRow - 2);
  }

  static GPUdi() bool innerAboveThreshold(uint8_t aboveThreshold, uint16_t outerIdx)
  {
    return aboveThreshold & (1 << cfconsts::OuterToInner[outerIdx]);
  }

  static GPUdi() bool innerAboveThresholdInv(uint8_t aboveThreshold, uint16_t outerIdx)
  {
    return aboveThreshold & (1 << cfconsts::OuterToInnerInv[outerIdx]);
  }

  static GPUdi() bool isPeak(uint8_t peak) { return peak & 0x01; }

  static GPUdi() bool isAboveThreshold(uint8_t peak) { return peak >> 1; }

  static GPUdi() int32_t warpPredicateScan(int32_t pred, int32_t* sum)
  {
#ifdef __HIPCC__
    int32_t iLane = hipThreadIdx_x % warpSize;
    uint64_t waveMask = __ballot(pred);
    uint64_t lowerWarpMask = (1ull << iLane) - 1ull;
    int32_t myOffset = __popcll(waveMask & lowerWarpMask);
    *sum = __popcll(waveMask);
    return myOffset;
#elif defined(__CUDACC__)
    int32_t iLane = threadIdx.x % warpSize;
    uint32_t waveMask = __ballot_sync(0xFFFFFFFF, pred);
    uint32_t lowerWarpMask = (1u << iLane) - 1u;
    int32_t myOffset = __popc(waveMask & lowerWarpMask);
    *sum = __popc(waveMask);
    return myOffset;
#else // CPU / OpenCL fallback
    int32_t myOffset = warp_scan_inclusive_add(pred ? 1 : 0);
    *sum = warp_broadcast(myOffset, GPUCA_WARP_SIZE - 1);
    myOffset--;
    return myOffset;
#endif
  }

  template <size_t BlockSize, typename SharedMemory>
  static GPUdi() int32_t blockPredicateScan(SharedMemory& smem, int32_t pred, int32_t* sum = nullptr)
  {
#if defined(__HIPCC__) || defined(__CUDACC__)
    int32_t iThread =
#ifdef __HIPCC__
      hipThreadIdx_x;
#else
      threadIdx.x;
#endif

    int32_t iWarp = iThread / warpSize;
    int32_t nWarps = BlockSize / warpSize;

    int32_t warpSum;
    int32_t laneOffset = warpPredicateScan(pred, &warpSum);

    if (iThread % warpSize == 0) {
      smem.warpPredicateSum[iWarp] = warpSum;
    }
    __syncthreads();

    int32_t warpOffset = 0;

    if (sum == nullptr) {
      for (int32_t w = 0; w < iWarp; w++) {
        int32_t s = smem.warpPredicateSum[w];
        warpOffset += s;
      }
    } else {
      *sum = 0;
      for (int32_t w = 0; w < nWarps; w++) {
        int32_t s = smem.warpPredicateSum[w];
        if (w < iWarp) {
          warpOffset += s;
        }
        *sum += s;
      }
    }

    return warpOffset + laneOffset;
#else // CPU / OpenCL fallback
    int32_t lpos = work_group_scan_inclusive_add(pred ? 1 : 0);
    if (sum != nullptr) {
      *sum = work_group_broadcast(lpos, BlockSize - 1);
    }
    lpos--;
    return lpos;
#endif
  }

  template <size_t BlockSize, typename SharedMemory>
  static GPUdi() int32_t blockPredicateSum(SharedMemory& smem, int32_t pred)
  {
#if defined(__HIPCC__) || defined(__CUDACC__)
    int32_t iThread =
#ifdef __HIPCC__
      hipThreadIdx_x;
#else
      threadIdx.x;
#endif

    int32_t iWarp = iThread / warpSize;
    int32_t nWarps = BlockSize / warpSize;

    int32_t warpSum =
#ifdef __HIPCC__
      __popcll(__ballot(pred));
#else
      __popc(__ballot_sync(0xFFFFFFFF, pred));
#endif

    if (iThread % warpSize == 0) {
      smem.warpPredicateSum[iWarp] = warpSum;
    }
    __syncthreads();

    int32_t sum = 0;
    for (int32_t w = 0; w < nWarps; w++) {
      sum += smem.warpPredicateSum[w];
    }

    return sum;
#else // CPU / OpenCL fallback
    return work_group_reduce_add(pred ? 1 : 0);
#endif
  }

  template <size_t SCRATCH_PAD_WORK_GROUP_SIZE, typename SharedMemory>
  static GPUdi() uint16_t partition(SharedMemory& smem, uint16_t ll, bool pred, uint16_t partSize, uint16_t* newPartSize)
  {
    bool participates = ll < partSize;

    int32_t part;
    int32_t lpos = blockPredicateScan<SCRATCH_PAD_WORK_GROUP_SIZE>(smem, int32_t(!pred && participates), &part);

    uint16_t pos = (participates && !pred) ? lpos : part;

    *newPartSize = part;
    return pos;
  }

  template <typename T>
  static GPUdi() void blockLoad(
    const Array2D<T>& map,
    uint32_t wgSize,
    uint32_t elems,
    uint16_t ll,
    uint32_t offset,
    uint32_t N,
    GPUconstexprref() const tpccf::Delta2* neighbors,
    const ChargePos* posBcast,
    GPUgeneric() T* buf)
  {
#if defined(GPUCA_GPUCODE)
    GPUbarrier();
    uint16_t x = ll % N;
    uint16_t y = ll / N;
    tpccf::Delta2 d = neighbors[x + offset];

    for (uint32_t i = y; i < wgSize; i += (elems / N)) {
      ChargePos readFrom = posBcast[i];
      uint32_t writeTo = N * i + x;
      buf[writeTo] = map[readFrom.delta(d)];
    }
    GPUbarrier();
#else
    if (ll >= wgSize) {
      return;
    }

    ChargePos readFrom = posBcast[ll];

    GPUbarrier();

    for (uint32_t i = 0; i < N; i++) {
      tpccf::Delta2 d = neighbors[i + offset];

      uint32_t writeTo = N * ll + i;
      buf[writeTo] = map[readFrom.delta(d)];
    }

    GPUbarrier();
#endif
  }

  template <typename T, bool Inv = false>
  static GPUdi() void condBlockLoad(
    const Array2D<T>& map,
    uint16_t wgSize,
    uint16_t elems,
    uint16_t ll,
    uint16_t offset,
    uint16_t N,
    GPUconstexprref() const tpccf::Delta2* neighbors,
    const ChargePos* posBcast,
    const uint8_t* aboveThreshold,
    GPUgeneric() T* buf)
  {
#if defined(GPUCA_GPUCODE)
    GPUbarrier();
    uint16_t y = ll / N;
    uint16_t x = ll % N;
    tpccf::Delta2 d = neighbors[x + offset];
    for (uint32_t i = y; i < wgSize; i += (elems / N)) {
      ChargePos readFrom = posBcast[i];
      uint8_t above = aboveThreshold[i];
      uint32_t writeTo = N * i + x;
      T v(0);
      bool cond = (Inv) ? innerAboveThresholdInv(above, x + offset)
                        : innerAboveThreshold(above, x + offset);
      if (cond) {
        v = map[readFrom.delta(d)];
      }
      buf[writeTo] = v;
    }
    GPUbarrier();
#else
    if (ll >= wgSize) {
      return;
    }

    ChargePos readFrom = posBcast[ll];
    uint8_t above = aboveThreshold[ll];
    GPUbarrier();

    for (uint32_t i = 0; i < N; i++) {
      tpccf::Delta2 d = neighbors[i + offset];

      uint32_t writeTo = N * ll + i;
      T v(0);
      bool cond = (Inv) ? innerAboveThresholdInv(above, i + offset)
                        : innerAboveThreshold(above, i + offset);
      if (cond) {
        v = map[readFrom.delta(d)];
      }
      buf[writeTo] = v;
    }

    GPUbarrier();
#endif
  }
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
