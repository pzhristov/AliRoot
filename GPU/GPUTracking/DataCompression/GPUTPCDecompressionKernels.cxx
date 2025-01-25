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

/// \file GPUTPCDecompressionKernels.cxx
/// \author Gabriele Cimador

#include "GPUTPCDecompressionKernels.h"
#include "GPULogging.h"
#include "GPUConstantMem.h"
#include "GPUTPCCompressionTrackModel.h"
#include "GPUCommonAlgorithm.h"
#include "TPCClusterDecompressionCore.inc"

using namespace GPUCA_NAMESPACE::gpu;
using namespace o2::tpc;

template <>
GPUdii() void GPUTPCDecompressionKernels::Thread<GPUTPCDecompressionKernels::step0attached>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, int32_t trackStart, int32_t trackEnd)
{
  GPUTPCDecompression& GPUrestrict() decompressor = processors.tpcDecompressor;
  CompressedClusters& GPUrestrict() cmprClusters = decompressor.mInputGPU;
  const GPUParam& GPUrestrict() param = processors.param;

  const uint32_t maxTime = (param.continuousMaxTimeBin + 1) * ClusterNative::scaleTimePacked - 1;

  for (int32_t i = trackStart + get_global_id(0); i < trackEnd; i += get_global_size(0)) {
    uint32_t offset = decompressor.mAttachedClustersOffsets[i];
    TPCClusterDecompressionCore::decompressTrack(cmprClusters, param, maxTime, i, offset, decompressor);
  }
}

template <>
GPUdii() void GPUTPCDecompressionKernels::Thread<GPUTPCDecompressionKernels::step1unattached>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, int32_t sliceStart, int32_t nSlices)
{
  GPUTPCDecompression& GPUrestrict() decompressor = processors.tpcDecompressor;
  CompressedClusters& GPUrestrict() cmprClusters = decompressor.mInputGPU;
  ClusterNative* GPUrestrict() clusterBuffer = decompressor.mNativeClustersBuffer;
  const ClusterNativeAccess* outputAccess = decompressor.mClusterNativeAccess;
  uint32_t* offsets = decompressor.mUnattachedClustersOffsets;
  for (int32_t i = get_global_id(0); i < GPUCA_ROW_COUNT * nSlices; i += get_global_size(0)) {
    uint32_t iRow = i % GPUCA_ROW_COUNT;
    uint32_t iSlice = sliceStart + (i / GPUCA_ROW_COUNT);
    const uint32_t linearIndex = iSlice * GPUCA_ROW_COUNT + iRow;
    uint32_t tmpBufferIndex = computeLinearTmpBufferIndex(iSlice, iRow, decompressor.mMaxNativeClustersPerBuffer);
    ClusterNative* buffer = clusterBuffer + outputAccess->clusterOffset[iSlice][iRow];
    if (decompressor.mNativeClustersIndex[linearIndex] != 0) {
      decompressorMemcpyBasic(buffer, decompressor.mTmpNativeClusters + tmpBufferIndex, decompressor.mNativeClustersIndex[linearIndex]);
    }
    ClusterNative* clout = buffer + decompressor.mNativeClustersIndex[linearIndex];
    uint32_t end = offsets[linearIndex] + ((linearIndex >= decompressor.mInputGPU.nSliceRows) ? 0 : decompressor.mInputGPU.nSliceRowClusters[linearIndex]);
    TPCClusterDecompressionCore::decompressHits(cmprClusters, offsets[linearIndex], end, clout);
    if (processors.param.rec.tpc.clustersShiftTimebins != 0.f) {
      for (uint32_t k = 0; k < outputAccess->nClusters[iSlice][iRow]; k++) {
        auto& cl = buffer[k];
        float t = cl.getTime() + processors.param.rec.tpc.clustersShiftTimebins;
        if (t < 0) {
          t = 0;
        }
        if (processors.param.continuousMaxTimeBin > 0 && t > processors.param.continuousMaxTimeBin) {
          t = processors.param.continuousMaxTimeBin;
        }
        cl.setTime(t);
      }
    }
  }
}

template <typename T>
GPUdi() void GPUTPCDecompressionKernels::decompressorMemcpyBasic(T* GPUrestrict() dst, const T* GPUrestrict() src, uint32_t size)
{
  for (uint32_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }
}

GPUdi() bool GPUTPCDecompressionUtilKernels::isClusterKept(const o2::tpc::ClusterNative& cl, const GPUParam& GPUrestrict() param)
{
  return param.tpcCutTimeBin > 0 ? cl.getTime() < param.tpcCutTimeBin : true;
}

template <>
GPUdii() void GPUTPCDecompressionUtilKernels::Thread<GPUTPCDecompressionUtilKernels::countFilteredClusters>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors)
{
  const GPUParam& GPUrestrict() param = processors.param;
  GPUTPCDecompression& GPUrestrict() decompressor = processors.tpcDecompressor;
  const ClusterNativeAccess* clusterAccess = decompressor.mClusterNativeAccess;
  for (uint32_t i = get_global_id(0); i < GPUCA_NSLICES * GPUCA_ROW_COUNT; i += get_global_size(0)) {
    uint32_t slice = i / GPUCA_ROW_COUNT;
    uint32_t row = i % GPUCA_ROW_COUNT;
    for (uint32_t k = 0; k < clusterAccess->nClusters[slice][row]; k++) {
      ClusterNative cl = clusterAccess->clusters[slice][row][k];
      if (isClusterKept(cl, param)) {
        decompressor.mNClusterPerSectorRow[i]++;
      }
    }
  }
}

template <>
GPUdii() void GPUTPCDecompressionUtilKernels::Thread<GPUTPCDecompressionUtilKernels::storeFilteredClusters>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors)
{
  const GPUParam& GPUrestrict() param = processors.param;
  GPUTPCDecompression& GPUrestrict() decompressor = processors.tpcDecompressor;
  ClusterNative* GPUrestrict() clusterBuffer = decompressor.mNativeClustersBuffer;
  const ClusterNativeAccess* clusterAccess = decompressor.mClusterNativeAccess;
  const ClusterNativeAccess* outputAccess = processors.ioPtrs.clustersNative;
  for (uint32_t i = get_global_id(0); i < GPUCA_NSLICES * GPUCA_ROW_COUNT; i += get_global_size(0)) {
    uint32_t slice = i / GPUCA_ROW_COUNT;
    uint32_t row = i % GPUCA_ROW_COUNT;
    uint32_t count = 0;
    for (uint32_t k = 0; k < clusterAccess->nClusters[slice][row]; k++) {
      const ClusterNative cl = clusterAccess->clusters[slice][row][k];
      if (isClusterKept(cl, param)) {
        clusterBuffer[outputAccess->clusterOffset[slice][row] + count] = cl;
        count++;
      }
    }
  }
}

template <>
GPUdii() void GPUTPCDecompressionUtilKernels::Thread<GPUTPCDecompressionUtilKernels::sortPerSectorRow>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors)
{
  ClusterNative* GPUrestrict() clusterBuffer = processors.tpcDecompressor.mNativeClustersBuffer;
  const ClusterNativeAccess* outputAccess = processors.ioPtrs.clustersNative;
  for (uint32_t i = get_global_id(0); i < GPUCA_NSLICES * GPUCA_ROW_COUNT; i += get_global_size(0)) {
    uint32_t slice = i / GPUCA_ROW_COUNT;
    uint32_t row = i % GPUCA_ROW_COUNT;
    ClusterNative* buffer = clusterBuffer + outputAccess->clusterOffset[slice][row];
    GPUCommonAlgorithm::sort(buffer, buffer + outputAccess->nClusters[slice][row]);
  }
}
