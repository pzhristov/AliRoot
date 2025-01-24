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

/// \file GPUReconstructionConvert.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONCONVERT_H
#define GPURECONSTRUCTIONCONVERT_H

#include <memory>
#include <functional>
#include <vector>
#include "GPUDef.h"

namespace o2
{
struct InteractionRecord;
namespace tpc
{
struct ClusterNative;
struct ClusterNativeAccess;
class Digit;
} // namespace tpc
namespace raw
{
class RawFileWriter;
} // namespace raw
} // namespace o2

struct AliHLTTPCRawCluster;

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUParam;
struct GPUTPCClusterData;
class TPCFastTransform;
struct GPUTrackingInOutDigits;
struct GPUTrackingInOutZS;

class GPUReconstructionConvert
{
 public:
  constexpr static uint32_t NSLICES = GPUCA_NSLICES;
  static void ConvertNativeToClusterData(o2::tpc::ClusterNativeAccess* native, std::unique_ptr<GPUTPCClusterData[]>* clusters, uint32_t* nClusters, const TPCFastTransform* transform, int32_t continuousMaxTimeBin = 0);
  static void ConvertRun2RawToNative(o2::tpc::ClusterNativeAccess& native, std::unique_ptr<o2::tpc::ClusterNative[]>& nativeBuffer, const AliHLTTPCRawCluster** rawClusters, uint32_t* nRawClusters);
  template <class S>
  static void RunZSEncoder(const S& in, std::unique_ptr<uint64_t[]>* outBuffer, uint32_t* outSizes, o2::raw::RawFileWriter* raw, const o2::InteractionRecord* ir, const GPUParam& param, int32_t version, bool verify, float threshold = 0.f, bool padding = false, std::function<void(std::vector<o2::tpc::Digit>&)> digitsFilter = nullptr);
  static void RunZSEncoderCreateMeta(const uint64_t* buffer, const uint32_t* sizes, void** ptrs, GPUTrackingInOutZS* out);
  static void RunZSFilter(std::unique_ptr<o2::tpc::Digit[]>* buffers, const o2::tpc::Digit* const* ptrs, size_t* nsb, const size_t* ns, const GPUParam& param, bool zs12bit, float threshold);
  static int32_t GetMaxTimeBin(const o2::tpc::ClusterNativeAccess& native);
  static int32_t GetMaxTimeBin(const GPUTrackingInOutDigits& digits);
  static int32_t GetMaxTimeBin(const GPUTrackingInOutZS& zspages);
  static std::function<void(std::vector<o2::tpc::Digit>&, const void*, uint32_t, uint32_t)> GetDecoder(int32_t version, const GPUParam* param);
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
