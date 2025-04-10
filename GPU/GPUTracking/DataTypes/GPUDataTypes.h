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

/// \file GPUDataTypes.h
/// \author David Rohr

#ifndef GPUDATATYPES_H
#define GPUDATATYPES_H

#include "GPUCommonDef.h"

// These are basic and non-complex data types, which will also be visible on the GPU.
// Please add complex data types required on the host but not GPU to GPUHostDataTypes.h and forward-declare!
#ifndef GPUCA_GPUCODE_DEVICE
#include <cstddef>
#endif
#include "GPUCommonTypeTraits.h"
#include "GPUTRDDef.h"

struct AliHLTTPCClusterMCLabel;
struct AliHLTTPCRawCluster;
namespace o2
{
namespace tpc
{
struct ClusterNativeAccess;
struct CompressedClustersFlat;
class Digit;
class TrackTPC;
namespace constants
{
} // namespace constants
} // namespace tpc
} // namespace o2

namespace o2
{
class MCCompLabel;
template <typename T>
class BaseCluster;
namespace base
{
template <typename T>
class PropagatorImpl;
class MatLayerCylSet;
} // namespace base
namespace track
{
template <typename value_T>
class TrackParametrizationWithError;
using TrackParCov = TrackParametrizationWithError<float>;
} // namespace track
namespace trd
{
class GeometryFlat;
} // namespace trd
namespace dataformats
{
class TrackTPCITS;
class MatchInfoTOF;
template <class T>
class MCTruthContainer;
template <class T>
class ConstMCTruthContainerView;
} // namespace dataformats
namespace itsmft
{
class CompClusterExt;
class ROFRecord;
class TopologyDictionary;
} // namespace itsmft
namespace its
{
class TrackITS;
} // namespace its
namespace tof
{
class Cluster;
} // namespace tof
namespace tpc
{
class CalibdEdxContainer;
} // namespace tpc
} // namespace o2

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class CorrectionMapsHelper;
class TPCFastTransform;
struct TPCPadGainCalib;
struct TPCZSLinkMapping;
} // namespace gpu
} // namespace GPUCA_NAMESPACE

namespace GPUCA_NAMESPACE
{
namespace gpu
{
#include "utils/bitfield.h"
#define ENUM_CLASS class
#define ENUM_UINT : uint32_t
#define GPUCA_RECO_STEP GPUDataTypes::RecoStep

class GPUTPCTrack;
class GPUTPCHitId;
class GPUTPCGMMergedTrack;
struct GPUTPCGMMergedTrackHit;
struct GPUTPCGMMergedTrackHitXYZ;
class GPUTRDTrackletWord;
class GPUTRDSpacePoint;
struct GPUTPCMCInfo;
struct GPUTPCMCInfoCol;
struct GPUTPCClusterData;
struct GPUTRDTrackletLabels;
struct GPUTPCDigitsMCInput;
struct GPUSettingsTF;

class GPUDataTypes
{
 public:
  enum ENUM_CLASS GeometryType ENUM_UINT{RESERVED_GEOMETRY = 0, ALIROOT = 1, O2 = 2};
  enum DeviceType ENUM_UINT { INVALID_DEVICE = 0,
                              CPU = 1,
                              CUDA = 2,
                              HIP = 3,
                              OCL = 4 };
  enum ENUM_CLASS GeneralStep { Prepare = 1,
                                QA = 2 };

  enum ENUM_CLASS RecoStep { TPCConversion = 1,
                             TPCSliceTracking = 2,
                             TPCMerging = 4,
                             TPCCompression = 8,
                             TRDTracking = 16,
                             ITSTracking = 32,
                             TPCdEdx = 64,
                             TPCClusterFinding = 128,
                             TPCDecompression = 256,
                             Refit = 512,
                             AllRecoSteps = 0x7FFFFFFF,
                             NoRecoStep = 0 };
  enum ENUM_CLASS InOutType { TPCClusters = 1,
                              TPCSectorTracks = 2,
                              TPCMergedTracks = 4,
                              TPCCompressedClusters = 8,
                              TRDTracklets = 16,
                              TRDTracks = 32,
                              TPCRaw = 64,
                              ITSClusters = 128,
                              ITSTracks = 256 };
#ifndef __OPENCL__
  static constexpr const char* const DEVICE_TYPE_NAMES[] = {"INVALID", "CPU", "CUDA", "HIP", "OCL"};
  static constexpr const char* const RECO_STEP_NAMES[] = {"TPC Transformation", "TPC Sector Tracking", "TPC Track Merging and Fit", "TPC Compression", "TRD Tracking", "ITS Tracking", "TPC dEdx Computation", "TPC Cluster Finding", "TPC Decompression", "Global Refit"};
  static constexpr const char* const GENERAL_STEP_NAMES[] = {"Prepare", "QA"};
  constexpr static int32_t N_RECO_STEPS = sizeof(GPUDataTypes::RECO_STEP_NAMES) / sizeof(GPUDataTypes::RECO_STEP_NAMES[0]);
  constexpr static int32_t N_GENERAL_STEPS = sizeof(GPUDataTypes::GENERAL_STEP_NAMES) / sizeof(GPUDataTypes::GENERAL_STEP_NAMES[0]);
#endif
  typedef bitfield<RecoStep, uint32_t> RecoStepField;
  typedef bitfield<InOutType, uint32_t> InOutTypeField;
  static constexpr uint32_t NSLICES = 36;
  static DeviceType GetDeviceType(const char* type);
};

struct GPURecoStepConfiguration {
  GPUDataTypes::RecoStepField steps = 0;
  GPUDataTypes::RecoStepField stepsGPUMask = GPUDataTypes::RecoStep::AllRecoSteps;
  GPUDataTypes::InOutTypeField inputs = 0;
  GPUDataTypes::InOutTypeField outputs = 0;
};

template <class T>
struct DefaultPtr {
  typedef T type;
};
template <class T>
struct ConstPtr {
  typedef const T type;
};

template <template <typename T> class S>
struct GPUCalibObjectsTemplate { // use only pointers on PODs or flat objects here
  typename S<TPCFastTransform>::type* fastTransform = nullptr;
  typename S<TPCFastTransform>::type* fastTransformRef = nullptr;
  typename S<TPCFastTransform>::type* fastTransformMShape = nullptr;
  typename S<CorrectionMapsHelper>::type* fastTransformHelper = nullptr;
  typename S<o2::base::MatLayerCylSet>::type* matLUT = nullptr;
  typename S<o2::trd::GeometryFlat>::type* trdGeometry = nullptr;
  typename S<TPCPadGainCalib>::type* tpcPadGain = nullptr;
  typename S<TPCZSLinkMapping>::type* tpcZSLinkMapping = nullptr;
  typename S<o2::tpc::CalibdEdxContainer>::type* dEdxCalibContainer = nullptr;
  typename S<o2::base::PropagatorImpl<float>>::type* o2Propagator = nullptr;
  typename S<o2::itsmft::TopologyDictionary>::type* itsPatternDict = nullptr;
};
typedef GPUCalibObjectsTemplate<DefaultPtr> GPUCalibObjects; // NOTE: These 2 must have identical layout since they are memcopied
typedef GPUCalibObjectsTemplate<ConstPtr> GPUCalibObjectsConst;

struct GPUTrackingInOutZS {
  static constexpr uint32_t NSLICES = GPUDataTypes::NSLICES;
  static constexpr uint32_t NENDPOINTS = 20;
  struct GPUTrackingInOutZSSlice {
    const void* const* zsPtr[NENDPOINTS];
    const uint32_t* nZSPtr[NENDPOINTS];
    uint32_t count[NENDPOINTS];
  };
  struct GPUTrackingInOutZSCounts {
    uint32_t count[NSLICES][NENDPOINTS] = {};
  };
  struct GPUTrackingInOutZSMeta {
    void* ptr[NSLICES][NENDPOINTS];
    uint32_t n[NSLICES][NENDPOINTS];
  };
  GPUTrackingInOutZSSlice slice[NSLICES];
};

struct GPUTrackingInOutDigits {
  static constexpr uint32_t NSLICES = GPUDataTypes::NSLICES;
  const o2::tpc::Digit* tpcDigits[NSLICES] = {nullptr};
  size_t nTPCDigits[NSLICES] = {0};
  const GPUTPCDigitsMCInput* tpcDigitsMC = nullptr;
};

struct GPUTrackingInOutPointers {
  GPUTrackingInOutPointers() = default;

  // TPC
  static constexpr uint32_t NSLICES = GPUDataTypes::NSLICES;
  const GPUTrackingInOutZS* tpcZS = nullptr;
  const GPUTrackingInOutDigits* tpcPackedDigits = nullptr;
  const GPUTPCClusterData* clusterData[NSLICES] = {nullptr};
  uint32_t nClusterData[NSLICES] = {0};
  const AliHLTTPCRawCluster* rawClusters[NSLICES] = {nullptr};
  uint32_t nRawClusters[NSLICES] = {0};
  const o2::tpc::ClusterNativeAccess* clustersNative = nullptr;
  const GPUTPCTrack* sliceTracks[NSLICES] = {nullptr};
  uint32_t nSliceTracks[NSLICES] = {0};
  const GPUTPCHitId* sliceClusters[NSLICES] = {nullptr};
  uint32_t nSliceClusters[NSLICES] = {0};
  const AliHLTTPCClusterMCLabel* mcLabelsTPC = nullptr;
  uint32_t nMCLabelsTPC = 0;
  const GPUTPCMCInfo* mcInfosTPC = nullptr;
  uint32_t nMCInfosTPC = 0;
  const GPUTPCMCInfoCol* mcInfosTPCCol = nullptr;
  uint32_t nMCInfosTPCCol = 0;
  const GPUTPCGMMergedTrack* mergedTracks = nullptr;
  uint32_t nMergedTracks = 0;
  const GPUTPCGMMergedTrackHit* mergedTrackHits = nullptr;
  const GPUTPCGMMergedTrackHitXYZ* mergedTrackHitsXYZ = nullptr;
  uint32_t nMergedTrackHits = 0;
  const uint32_t* mergedTrackHitAttachment = nullptr;
  const uint8_t* mergedTrackHitStates = nullptr;
  const o2::tpc::TrackTPC* outputTracksTPCO2 = nullptr;
  uint32_t nOutputTracksTPCO2 = 0;
  const uint32_t* outputClusRefsTPCO2 = nullptr;
  uint32_t nOutputClusRefsTPCO2 = 0;
  const o2::MCCompLabel* outputTracksTPCO2MC = nullptr;
  const o2::tpc::CompressedClustersFlat* tpcCompressedClusters = nullptr;

  // TPC links
  int32_t* tpcLinkITS = nullptr;
  int32_t* tpcLinkTRD = nullptr;
  int32_t* tpcLinkTOF = nullptr;
  const o2::track::TrackParCov** globalTracks = nullptr;
  float* globalTrackTimes = nullptr;
  uint32_t nGlobalTracks = 0;

  // TRD
  const GPUTRDTrackletWord* trdTracklets = nullptr;
  const GPUTRDSpacePoint* trdSpacePoints = nullptr;
  uint32_t nTRDTracklets = 0;
  const GPUTRDTrackGPU* trdTracks = nullptr;
  const GPUTRDTrack* trdTracksO2 = nullptr;
  uint32_t nTRDTracks = 0;
  const float* trdTriggerTimes = nullptr;
  const int32_t* trdTrackletIdxFirst = nullptr;
  const uint8_t* trdTrigRecMask = nullptr;
  uint32_t nTRDTriggerRecords = 0;
  const GPUTRDTrack* trdTracksITSTPCTRD = nullptr;
  uint32_t nTRDTracksITSTPCTRD = 0;
  const GPUTRDTrack* trdTracksTPCTRD = nullptr;
  uint32_t nTRDTracksTPCTRD = 0;

  // TOF
  const o2::tof::Cluster* tofClusters = nullptr;
  uint32_t nTOFClusters = 0;
  const o2::dataformats::MatchInfoTOF* itstpctofMatches = nullptr;
  uint32_t nITSTPCTOFMatches = 0;
  const o2::dataformats::MatchInfoTOF* itstpctrdtofMatches = nullptr;
  uint32_t nITSTPCTRDTOFMatches = 0;
  const o2::dataformats::MatchInfoTOF* tpctrdtofMatches = nullptr;
  uint32_t nTPCTRDTOFMatches = 0;
  const o2::dataformats::MatchInfoTOF* tpctofMatches = nullptr;
  uint32_t nTPCTOFMatches = 0;

  // ITS
  const o2::itsmft::CompClusterExt* itsCompClusters = nullptr;
  const o2::dataformats::MCTruthContainer<o2::MCCompLabel>* itsClusterMC = nullptr;
  const o2::BaseCluster<float>* itsClusters = nullptr;
  uint32_t nItsClusters = 0;
  const o2::itsmft::ROFRecord* itsClusterROF = nullptr;
  uint32_t nItsClusterROF = 0;
  const o2::its::TrackITS* itsTracks = nullptr;
  const o2::MCCompLabel* itsTrackMC = nullptr;
  uint32_t nItsTracks = 0;
  const int32_t* itsTrackClusIdx = nullptr;
  const o2::itsmft::ROFRecord* itsTrackROF = nullptr;
  uint32_t nItsTrackROF = 0;

  // TPC-ITS
  const o2::dataformats::TrackTPCITS* tracksTPCITSO2 = nullptr;
  uint32_t nTracksTPCITSO2 = 0;

  // Common
  const GPUSettingsTF* settingsTF = nullptr;
};

#undef ENUM_CLASS
#undef ENUM_UINT
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
