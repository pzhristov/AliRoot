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

/// \file GPUTrackingInputProvider.cxx
/// \author David Rohr

#include "GPUTrackingInputProvider.h"
#include "GPUDataTypes.h"
#include "GPUTRDTrackletWord.h"
#include "GPUReconstruction.h"
#include "GPUTPCClusterOccupancyMap.h"
#include "GPUErrors.h"

using namespace GPUCA_NAMESPACE::gpu;
using namespace o2::tpc;

void GPUTrackingInputProvider::InitializeProcessor() {}
void* GPUTrackingInputProvider::SetPointersInputZS(void* mem)
{
  if (mRec->GetRecoStepsGPU() & GPUDataTypes::RecoStep::TPCClusterFinding) {
    computePointerWithAlignment(mem, mPzsMeta);
    computePointerWithAlignment(mem, mPzsSizes, GPUTrackingInOutZS::NSLICES * GPUTrackingInOutZS::NENDPOINTS);
    computePointerWithAlignment(mem, mPzsPtrs, GPUTrackingInOutZS::NSLICES * GPUTrackingInOutZS::NENDPOINTS);
  }
  return mem;
}

void* GPUTrackingInputProvider::SetPointersInputClusterNativeAccess(void* mem)
{
  if (mHoldTPCClusterNative) {
    computePointerWithAlignment(mem, mPclusterNativeAccess);
  }
  return mem;
}

void* GPUTrackingInputProvider::SetPointersInputClusterNativeBuffer(void* mem)
{
  if (mHoldTPCClusterNative) {
    computePointerWithAlignment(mem, mPclusterNativeBuffer, mNClusterNative);
  }
  return mem;
}

void* GPUTrackingInputProvider::SetPointersInputClusterNativeOutput(void* mem)
{
  if (mHoldTPCClusterNativeOutput) {
    computePointerWithoutAlignment(mem, mPclusterNativeOutput, mNClusterNative); // TODO: Should decide based on some settings whether with or without alignment. Without only needed for output to unaligned shared memory in workflow.
  }
  return mem;
}

void* GPUTrackingInputProvider::SetPointersErrorCodes(void* mem)
{
  computePointerWithAlignment(mem, mErrorCodes, 4 * GPUErrors::getMaxErrors() + 1);
  return mem;
}

void* GPUTrackingInputProvider::SetPointersInputTRD(void* mem)
{
  computePointerWithAlignment(mem, mTRDTracklets, mNTRDTracklets);
  if (mDoSpacepoints) {
    computePointerWithAlignment(mem, mTRDSpacePoints, mNTRDTracklets);
  }
  computePointerWithAlignment(mem, mTRDTriggerTimes, mNTRDTriggerRecords);
  computePointerWithAlignment(mem, mTRDTrackletIdxFirst, mNTRDTriggerRecords);
  computePointerWithAlignment(mem, mTRDTrigRecMask, mNTRDTriggerRecords);

  return mem;
}

void* GPUTrackingInputProvider::SetPointersTPCOccupancyMap(void* mem)
{
  if (mHoldTPCOccupancyMap) {
    computePointerWithAlignment(mem, mTPCClusterOccupancyMap, (mRec->GetParam().rec.tpc.occupancyMapTimeBins ? GPUTPCClusterOccupancyMapBin::getNBins(mRec->GetParam()) + 1 : 0) + 1); // +1 for total occupancy estimator, +1 for sanity check information
  }
  return mem;
}

void GPUTrackingInputProvider::RegisterMemoryAllocation()
{
  mResourceErrorCodes = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersErrorCodes, GPUMemoryResource::MEMORY_PERMANENT, "ErrorCodes");
  mResourceZS = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersInputZS, GPUMemoryResource::MEMORY_INPUT | GPUMemoryResource::MEMORY_PERMANENT, "InputZS");
  mResourceOccupancyMap = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersTPCOccupancyMap, GPUMemoryResource::MEMORY_INOUT | GPUMemoryResource::MEMORY_CUSTOM, "OccupancyMap");
  mResourceClusterNativeAccess = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersInputClusterNativeAccess, GPUMemoryResource::MEMORY_INPUT, "ClusterNativeAccess");
  mResourceClusterNativeBuffer = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersInputClusterNativeBuffer, GPUMemoryResource::MEMORY_INPUT_FLAG | GPUMemoryResource::MEMORY_GPU | GPUMemoryResource::MEMORY_EXTERNAL | GPUMemoryResource::MEMORY_CUSTOM, "ClusterNativeBuffer");
  mResourceClusterNativeOutput = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersInputClusterNativeOutput, GPUMemoryResource::MEMORY_OUTPUT_FLAG | GPUMemoryResource::MEMORY_HOST | GPUMemoryResource::MEMORY_CUSTOM, "ClusterNativeOutput");
  mResourceTRD = mRec->RegisterMemoryAllocation(this, &GPUTrackingInputProvider::SetPointersInputTRD, GPUMemoryResource::MEMORY_INPUT_FLAG | GPUMemoryResource::MEMORY_GPU | GPUMemoryResource::MEMORY_EXTERNAL | GPUMemoryResource::MEMORY_CUSTOM, "TRDInputBuffer");
}

void GPUTrackingInputProvider::SetMaxData(const GPUTrackingInOutPointers& io)
{
  mHoldTPCZS = io.tpcZS && (mRec->GetRecoStepsGPU() & GPUDataTypes::RecoStep::TPCClusterFinding);
  mHoldTPCClusterNative = (io.tpcZS || io.tpcPackedDigits || io.clustersNative || io.tpcCompressedClusters) && (mRec->IsGPU() || io.tpcCompressedClusters);
  mHoldTPCOccupancyMap = (io.tpcZS || io.tpcPackedDigits || io.clustersNative || io.tpcCompressedClusters) && (mRec->GetParam().rec.tpc.occupancyMapTimeBins || mRec->GetParam().rec.tpc.sysClusErrorC12Norm);
  mHoldTPCClusterNativeOutput = io.tpcZS || io.tpcPackedDigits || io.tpcCompressedClusters;
}
