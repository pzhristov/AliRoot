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

/// \file GPUTrackingInputProvider.h
/// \author David Rohr

#ifndef GPUTRACKINGINPUTPROVIDER_H
#define GPUTRACKINGINPUTPROVIDER_H

#include "GPUDef.h"
#include "GPUProcessor.h"

namespace o2
{
namespace tpc
{
struct ClusterNative;
struct ClusterNativeAccess;
} // namespace tpc
} // namespace o2

namespace GPUCA_NAMESPACE
{
namespace gpu
{

struct GPUTrackingInOutZS;
struct GPUTPCClusterOccupancyMapBin;
class GPUTRDTrackletWord;
class GPUTRDSpacePoint;

class GPUTrackingInputProvider : public GPUProcessor
{
 public:
#ifndef GPUCA_GPUCODE
  void InitializeProcessor();
  void RegisterMemoryAllocation();
  void SetMaxData(const GPUTrackingInOutPointers& io);

  void* SetPointersTPCOccupancyMap(void* mem);
  void* SetPointersInputZS(void* mem);
  void* SetPointersInputClusterNativeAccess(void* mem);
  void* SetPointersInputClusterNativeBuffer(void* mem);
  void* SetPointersInputClusterNativeOutput(void* mem);
  void* SetPointersInputTRD(void* mem);
  void* SetPointersErrorCodes(void* mem);
#endif

  uint16_t mResourceZS = -1;
  uint16_t mResourceClusterNativeAccess = -1;
  uint16_t mResourceClusterNativeBuffer = -1;
  uint16_t mResourceClusterNativeOutput = -1;
  uint16_t mResourceErrorCodes = -1;
  uint16_t mResourceTRD = -1;
  uint16_t mResourceOccupancyMap = -1;

  bool mHoldTPCZS = false;
  bool mHoldTPCClusterNative = false;
  bool mHoldTPCClusterNativeOutput = false;
  bool mHoldTPCOccupancyMap = false;
  uint32_t mNClusterNative = 0;

  GPUTrackingInOutZS* mPzsMeta = nullptr;
  uint32_t* mPzsSizes = nullptr;
  void** mPzsPtrs = nullptr;

  uint32_t mNTRDTracklets = 0;
  bool mDoSpacepoints = false;
  uint32_t mNTRDTriggerRecords = 0;
  GPUTRDTrackletWord* mTRDTracklets = nullptr;
  GPUTRDSpacePoint* mTRDSpacePoints = nullptr;
  float* mTRDTriggerTimes = nullptr;
  int32_t* mTRDTrackletIdxFirst = nullptr;
  uint8_t* mTRDTrigRecMask = nullptr;

  o2::tpc::ClusterNativeAccess* mPclusterNativeAccess = nullptr;
  o2::tpc::ClusterNative* mPclusterNativeBuffer = nullptr;
  o2::tpc::ClusterNative* mPclusterNativeOutput = nullptr;

  uint32_t* mTPCClusterOccupancyMap = nullptr;

  uint32_t* mErrorCodes = nullptr;
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
