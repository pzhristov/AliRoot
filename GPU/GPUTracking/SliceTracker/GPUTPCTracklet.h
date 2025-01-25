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

/// \file GPUTPCTracklet.h
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#ifndef GPUTPCTRACKLET_H
#define GPUTPCTRACKLET_H

#include "GPUTPCBaseTrackParam.h"
#include "GPUTPCDef.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
/**
 * @class GPUTPCTracklet
 *
 * The class describes the reconstructed TPC track candidate.
 * The class is dedicated for internal use by the GPUTPCTracker algorithm.
 */
class GPUTPCTracklet
{
 public:
#if !defined(GPUCA_GPUCODE)
  GPUTPCTracklet() : mFirstRow(0), mLastRow(0), mParam(), mHitWeight(0), mFirstHit(0){};
#endif //! GPUCA_GPUCODE

  GPUhd() int32_t FirstRow() const { return mFirstRow; }
  GPUhd() int32_t LastRow() const { return mLastRow; }
  GPUhd() int32_t HitWeight() const { return mHitWeight; }
  GPUhd() uint32_t FirstHit() const { return mFirstHit; }
  GPUhd() const GPUTPCBaseTrackParam& Param() const { return mParam; }

  GPUhd() void SetFirstRow(int32_t v) { mFirstRow = v; }
  GPUhd() void SetLastRow(int32_t v) { mLastRow = v; }
  GPUhd() void SetFirstHit(uint32_t v) { mFirstHit = v; }
  GPUhd() void SetParam(const GPUTPCBaseTrackParam& v) { mParam = reinterpret_cast<const GPUTPCBaseTrackParam&>(v); }
  GPUhd() void SetHitWeight(const int32_t w) { mHitWeight = w; }

 private:
  int32_t mFirstRow; // first TPC row // TODO: We can use smaller data format here!
  int32_t mLastRow;  // last TPC row
  GPUTPCBaseTrackParam mParam; // tracklet parameters
  int32_t mHitWeight;     // Hit Weight of Tracklet
  uint32_t mFirstHit;     // first hit in row hit array
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUTPCTRACKLET_H
