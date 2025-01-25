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

/// \file GPUTPCTrack.h
/// \author Sergey Gorbunov, David Rohr

#ifndef GPUTPCTRACK_H
#define GPUTPCTRACK_H

#include "GPUTPCBaseTrackParam.h"
#include "GPUTPCDef.h"
#include "GPUTPCSliceOutCluster.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
/**
 * @class GPUTPCTrack
 *
 * The class describes the [partially] reconstructed TPC track [candidate].
 * The class is dedicated for internal use by the GPUTPCTracker algorithm.
 * The track parameters at both ends are stored separately in the GPUTPCEndPoint class
 */
class GPUTPCTrack
{
 public:
#if !defined(GPUCA_GPUCODE)
  GPUTPCTrack() : mFirstHitID(0), mNHits(0), mLocalTrackId(-1), mParam()
  {
  }
  ~GPUTPCTrack() = default;
#endif //! GPUCA_GPUCODE

  GPUhd() int32_t NHits() const { return mNHits; }
  GPUhd() int32_t LocalTrackId() const { return mLocalTrackId; }
  GPUhd() int32_t FirstHitID() const { return mFirstHitID; }
  GPUhd() const GPUTPCBaseTrackParam& Param() const { return mParam; }

  GPUhd() void SetNHits(int32_t v) { mNHits = v; }
  GPUhd() void SetLocalTrackId(int32_t v) { mLocalTrackId = v; }
  GPUhd() void SetFirstHitID(int32_t v) { mFirstHitID = v; }

  GPUhd() void SetParam(const GPUTPCBaseTrackParam& v) { mParam = v; }

  // Only if used as replacement for SliceOutTrack
  GPUhd() static int32_t GetSize(int32_t nClust) { return sizeof(GPUTPCTrack) + nClust * sizeof(GPUTPCSliceOutCluster); }
  GPUhd() const GPUTPCTrack* GetNextTrack() const { return (const GPUTPCTrack*)(((char*)this) + GetSize(mNHits)); }
  GPUhd() GPUTPCTrack* NextTrack() { return (GPUTPCTrack*)(((char*)this) + GetSize(mNHits)); }
  GPUhd() void SetOutTrackCluster(int32_t i, const GPUTPCSliceOutCluster& v) { ((GPUTPCSliceOutCluster*)((char*)this + sizeof(*this)))[i] = v; }
  GPUhd() const GPUTPCSliceOutCluster* OutTrackClusters() const { return (const GPUTPCSliceOutCluster*)((char*)this + sizeof(*this)); }
  GPUhd() const GPUTPCSliceOutCluster& OutTrackCluster(int32_t i) const { return OutTrackClusters()[i]; }

 private:
  int32_t mFirstHitID;   // index of the first track cell in the track->cell pointer array
  int32_t mNHits;        // number of track cells
  int32_t mLocalTrackId; // Id of local track this global track belongs to, index of this track itself if it is a local track
  GPUTPCBaseTrackParam mParam; // track parameters

 private:
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUTPCTRACK_H
