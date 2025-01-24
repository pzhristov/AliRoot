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

/// \file GPUTPCCreateOccupancyMap.cxx
/// \author David Rohr

#include "GPUTPCCreateOccupancyMap.h"
#include "GPUTPCClusterOccupancyMap.h"

using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUdii() void GPUTPCCreateOccupancyMap::Thread<GPUTPCCreateOccupancyMap::fill>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUTPCClusterOccupancyMapBin* GPUrestrict() map)
{
  const GPUTrackingInOutPointers& GPUrestrict() ioPtrs = processors.ioPtrs;
  const o2::tpc::ClusterNativeAccess* GPUrestrict() clusters = ioPtrs.clustersNative;
  GPUParam& GPUrestrict() param = processors.param;
  const int32_t iSliceRow = iBlock * nThreads + iThread;
  if (iSliceRow >= GPUCA_ROW_COUNT * GPUCA_NSLICES) {
    return;
  }
  const uint32_t iSlice = iSliceRow / GPUCA_ROW_COUNT;
  const uint32_t iRow = iSliceRow % GPUCA_ROW_COUNT;
  for (uint32_t i = 0; i < clusters->nClusters[iSlice][iRow]; i++) {
    const uint32_t bin = clusters->clusters[iSlice][iRow][i].getTime() / param.rec.tpc.occupancyMapTimeBins;
    map[bin].bin[iSlice][iRow]++;
  }
}

template <>
GPUdii() void GPUTPCCreateOccupancyMap::Thread<GPUTPCCreateOccupancyMap::fold>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUTPCClusterOccupancyMapBin* GPUrestrict() map, uint32_t* GPUrestrict() output)
{
  GPUParam& GPUrestrict() param = processors.param;
  const uint32_t bin = iBlock * nThreads + iThread;
  if (bin >= GPUTPCClusterOccupancyMapBin::getNBins(param)) {
    return;
  }
  int32_t binmin = CAMath::Max<int32_t>(0, bin - param.rec.tpc.occupancyMapTimeBinsAverage);
  int32_t binmax = CAMath::Min<int32_t>(GPUTPCClusterOccupancyMapBin::getNBins(param), bin + param.rec.tpc.occupancyMapTimeBinsAverage + 1);
  uint32_t sum = 0;
  for (int32_t i = binmin; i < binmax; i++) {
    for (int32_t iSliceRow = 0; iSliceRow < GPUCA_NSLICES * GPUCA_ROW_COUNT; iSliceRow++) {
      sum += (&map[i].bin[0][0])[iSliceRow];
    }
  }
  sum /= binmax - binmin;
  output[bin] = sum;
}
