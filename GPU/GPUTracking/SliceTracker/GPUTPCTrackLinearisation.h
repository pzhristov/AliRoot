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

/// \file GPUTPCTrackLinearisation.h
/// \author Sergey Gorbunov, David Rohr

#ifndef GPUTPCTRACKLINEARISATION_H
#define GPUTPCTRACKLINEARISATION_H

#include "GPUTPCTrackParam.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
/**
 * @class GPUTPCTrackLinearisation
 *
 * GPUTPCTrackLinearisation class describes the parameters which are used
 * to linearise the transport equations for the track trajectory.
 *
 * The class is used during track (re)fit, when the AliHLTTPCTrackParam track is only
 * partially fitted, and there is some apriory knowledge about trajectory.
 * This apriory knowledge is used to linearise the transport equations.
 *
 * In case the track is fully fitted, the best linearisation point is
 * the track trajectory itself (GPUTPCTrackLinearisation = AliHLTTPCTrackParam ).
 *
 */
class GPUTPCTrackLinearisation
{
 public:
  GPUTPCTrackLinearisation() : mSinPhi(0), mCosPhi(1), mDzDs(0), mQPt(0) {}
  GPUTPCTrackLinearisation(float SinPhi1, float CosPhi1, float DzDs1, float QPt1) : mSinPhi(SinPhi1), mCosPhi(CosPhi1), mDzDs(DzDs1), mQPt(QPt1) {}

  GPUd() GPUTPCTrackLinearisation(const GPUTPCTrackParam& t);

  GPUd() void Set(float SinPhi1, float CosPhi1, float DzDs1, float QPt1);

  GPUd() float SinPhi() const { return mSinPhi; }
  GPUd() float CosPhi() const { return mCosPhi; }
  GPUd() float DzDs() const { return mDzDs; }
  GPUd() float QPt() const { return mQPt; }

  GPUd() float GetSinPhi() const { return mSinPhi; }
  GPUd() float GetCosPhi() const { return mCosPhi; }
  GPUd() float GetDzDs() const { return mDzDs; }
  GPUd() float GetQPt() const { return mQPt; }

  GPUd() void SetSinPhi(float v) { mSinPhi = v; }
  GPUd() void SetCosPhi(float v) { mCosPhi = v; }
  GPUd() void SetDzDs(float v) { mDzDs = v; }
  GPUd() void SetQPt(float v) { mQPt = v; }

 private:
  float mSinPhi; // SinPhi
  float mCosPhi; // CosPhi
  float mDzDs;   // DzDs
  float mQPt;    // QPt
};

GPUdi() GPUTPCTrackLinearisation::GPUTPCTrackLinearisation(const GPUTPCTrackParam& GPUrestrict() t) : mSinPhi(t.SinPhi()), mCosPhi(0), mDzDs(t.DzDs()), mQPt(t.QPt())
{
  if (mSinPhi > GPUCA_MAX_SIN_PHI) {
    mSinPhi = GPUCA_MAX_SIN_PHI;
  } else if (mSinPhi < -GPUCA_MAX_SIN_PHI) {
    mSinPhi = -GPUCA_MAX_SIN_PHI;
  }
  mCosPhi = CAMath::Sqrt(1 - mSinPhi * mSinPhi);
  if (t.SignCosPhi() < 0) {
    mCosPhi = -mCosPhi;
  }
}

GPUdi() void GPUTPCTrackLinearisation::Set(float SinPhi1, float CosPhi1, float DzDs1, float QPt1)
{
  SetSinPhi(SinPhi1);
  SetCosPhi(CosPhi1);
  SetDzDs(DzDs1);
  SetQPt(QPt1);
}
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUTPCTRACKLINEARISATION_H
