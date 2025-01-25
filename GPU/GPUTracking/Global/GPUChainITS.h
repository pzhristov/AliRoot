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

/// \file GPUChainITS.h
/// \author David Rohr

#ifndef GPUCHAINITS_H
#define GPUCHAINITS_H

#include "GPUChain.h"
namespace o2::its
{
struct Cluster;
template <uint8_t N>
class Road;
class Cell;
struct TrackingFrameInfo;
class TrackITSExt;
class GPUFrameworkExternalAllocator;
} // namespace o2::its

namespace GPUCA_NAMESPACE::gpu
{
class GPUChainITS : public GPUChain
{
  friend class GPUReconstruction;

 public:
  ~GPUChainITS() override;
  void RegisterPermanentMemoryAndProcessors() override;
  void RegisterGPUProcessors() override;
  int32_t Init() override;
  int32_t PrepareEvent() override;
  int32_t Finalize() override;
  int32_t RunChain() override;
  void MemorySize(size_t& gpuMem, size_t& pageLockedHostMem) override;

  o2::its::TrackerTraits* GetITSTrackerTraits();
  o2::its::VertexerTraits* GetITSVertexerTraits();
  o2::its::TimeFrame* GetITSTimeframe();

 protected:
  GPUChainITS(GPUReconstruction* rec, uint32_t maxTracks = GPUCA_MAX_ITS_FIT_TRACKS);
  std::unique_ptr<o2::its::TrackerTraits> mITSTrackerTraits;
  std::unique_ptr<o2::its::VertexerTraits> mITSVertexerTraits;
  std::unique_ptr<o2::its::TimeFrame> mITSTimeFrame;
  std::unique_ptr<o2::its::GPUFrameworkExternalAllocator> mFrameworkAllocator;

  uint32_t mMaxTracks;
};
} // namespace GPUCA_NAMESPACE::gpu

#endif
