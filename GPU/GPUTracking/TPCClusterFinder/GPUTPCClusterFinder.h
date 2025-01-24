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

/// \file GPUTPCClusterFinder.h
/// \author David Rohr

#ifndef O2_GPU_GPUTPCCLUSTERFINDER_H
#define O2_GPU_GPUTPCCLUSTERFINDER_H

#include "GPUDef.h"
#include "GPUProcessor.h"
#include "GPUDataTypes.h"
#include "CfFragment.h"

namespace o2
{

class MCCompLabel;

namespace dataformats
{
template <typename TruthElement>
class MCTruthContainer;
template <typename TruthElement>
class ConstMCTruthContainerView;
} // namespace dataformats

namespace tpc
{
struct ClusterNative;
class Digit;
} // namespace tpc

} // namespace o2

namespace GPUCA_NAMESPACE::gpu
{
struct GPUTPCClusterMCInterimArray;
struct TPCPadGainCalib;

struct ChargePos;

class GPUTPCGeometry;

class GPUTPCClusterFinder : public GPUProcessor
{
 public:
  struct Memory {
    struct counters_t {
      size_t nDigits = 0;
      tpccf::SizeT nDigitsInFragment = 0; // num of digits in fragment can differ from nPositions if ZS is active
      tpccf::SizeT nPositions = 0;
      tpccf::SizeT nPeaks = 0;
      tpccf::SizeT nClusters = 0;
      uint32_t maxTimeBin = 0;
      uint32_t nPagesSubslice = 0;
    } counters;
    CfFragment fragment;
  };

  struct ZSOffset {
    uint32_t offset;
    uint16_t endpoint;
    uint16_t num;
  };

  struct MinMaxCN {
    uint32_t zsPtrFirst, zsPageFirst, zsPtrLast, zsPageLast;
  };

#ifndef GPUCA_GPUCODE
  ~GPUTPCClusterFinder();
  void InitializeProcessor();
  void RegisterMemoryAllocation();
  void SetMaxData(const GPUTrackingInOutPointers& io);

  void* SetPointersInput(void* mem);
  void* SetPointersOutput(void* mem);
  void* SetPointersScratch(void* mem);
  void* SetPointersMemory(void* mem);
  void* SetPointersZS(void* mem);
  void* SetPointersZSOffset(void* mem);

  uint32_t getNSteps(size_t items) const;
  void SetNMaxDigits(size_t nDigits, size_t nPages, size_t nDigitsFragment, size_t nDigitsEndpointMax);

  void PrepareMC();
  void clearMCMemory();
#endif
  uint8_t* mPzs = nullptr;
  ZSOffset* mPzsOffsets = nullptr;
  MinMaxCN* mMinMaxCN = nullptr;
  uint8_t* mPpadIsNoisy = nullptr;
  tpc::Digit* mPdigits = nullptr; // input digits, only set if ZS is skipped
  ChargePos* mPpositions = nullptr;
  ChargePos* mPpeakPositions = nullptr;
  ChargePos* mPfilteredPeakPositions = nullptr;
  uint8_t* mPisPeak = nullptr;
  uint32_t* mPclusterPosInRow = nullptr; // store the index where the corresponding cluster is stored in a bucket.
                                         // Required when MC are enabled to write the mc data to the correct position.
                                         // Set to >= mNMaxClusterPerRow if cluster was discarded.
  uint16_t* mPchargeMap = nullptr;
  uint8_t* mPpeakMap = nullptr;
  uint32_t* mPindexMap = nullptr;
  uint32_t* mPclusterInRow = nullptr;
  tpc::ClusterNative* mPclusterByRow = nullptr;
  GPUTPCClusterMCInterimArray* mPlabelsByRow = nullptr;
  int32_t* mPbuf = nullptr;
  Memory* mPmemory = nullptr;

  GPUdi() int32_t* GetScanBuffer(int32_t iBuf) const { return mPbuf + iBuf * mBufSize; }

  o2::dataformats::ConstMCTruthContainerView<o2::MCCompLabel> const* mPinputLabels = nullptr;
  uint32_t* mPlabelsInRow = nullptr;
  uint32_t mPlabelsHeaderGlobalOffset = 0;
  uint32_t mPlabelsDataGlobalOffset = 0;

  int32_t mISlice = 0;
  constexpr static int32_t mScanWorkGroupSize = GPUCA_THREAD_COUNT_SCAN;
  uint32_t mNMaxClusterPerRow = 0;
  uint32_t mNMaxClusters = 0;
  uint32_t mNMaxPages = 0;
  size_t mNMaxDigits = 0;
  size_t mNMaxDigitsFragment = 0;
  size_t mNMaxDigitsEndpoint = 0;
  size_t mNMaxPeaks = 0;
  size_t mBufSize = 0;
  uint32_t mNBufs = 0;

  int16_t mMemoryId = -1;
  int16_t mScratchId = -1;
  int16_t mZSId = -1;
  int16_t mZSOffsetId = -1;
  int16_t mOutputId = -1;

#ifndef GPUCA_GPUCODE
  void DumpDigits(std::ostream& out);
  void DumpChargeMap(std::ostream& out, std::string_view);
  void DumpPeakMap(std::ostream& out, std::string_view);
  void DumpPeaks(std::ostream& out);
  void DumpPeaksCompacted(std::ostream& out);
  void DumpSuppressedPeaks(std::ostream& out);
  void DumpSuppressedPeaksCompacted(std::ostream& out);
  void DumpClusters(std::ostream& out);
#endif
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
