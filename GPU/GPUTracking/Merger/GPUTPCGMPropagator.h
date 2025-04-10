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

/// \file GPUTPCGMPropagator.h
/// \author Sergey Gorbunov, David Rohr

#ifndef GPUTPCGMPROPAGATOR_H
#define GPUTPCGMPROPAGATOR_H

#include "GPUCommonDef.h"
#include "GPUTPCGMPhysicalTrackModel.h"
#include "GPUTPCGMPolynomialField.h"
#include "GPUCommonMath.h"

namespace o2
{
namespace base
{
struct MatBudget;
class MatLayerCylSet;
} // namespace base
} // namespace o2

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUTPCGMTrackParam;
struct GPUParam;
namespace gputpcgmmergertypes
{
struct InterpolationErrorHit;
}

/**
 * @class GPUTPCGMPropagator
 *
 */

class GPUTPCGMPropagator
{
 public:
  /// Enumeration of field regions
  enum FieldRegion {
    TPC = 0, ///< TPC
    ITS = 1, ///< ITS
    TRD = 2  ///< outer TPC -> outer TRD
  };
  enum UpdateRetVal {
    updateErrorFitFailed = -1,
    updateErrorClusterRejected = 2,
    updateErrorEdgeCluster = 3
  };
  enum RejectChi2Mode {
    rejectDirect = 1,
    rejectInterFill = 2,
    rejectInterReject = 3
  };

  GPUdDefault() GPUTPCGMPropagator() = default;

  struct MaterialCorrection {
    GPUhd() MaterialCorrection() : radLen(28811.7f), rho(1.025e-3f), radLenInv(1.f / radLen), DLMax(0.f), EP2(0.f), sigmadE2(0.f), k22(0.f), k33(0.f), k43(0.f), k44(0.f) {}
    float radLen;                                              // [cm]
    float rho;                                                 // [g/cm^3]
    float radLenInv, DLMax, EP2, sigmadE2, k22, k33, k43, k44; // precalculated values for MS and EnergyLoss correction
  };

  struct DebugStreamerVals {
    int32_t retVal = -100;
    float err2Y = -1e6f, err2Z = -1e6f;
  };

  GPUd() void SetMaterial(float radLen, float rho);
  GPUd() void SetMaterialTPC() { SetMaterial(28811.7f, 1.025e-3f); }

  GPUd() void UpdateMaterial(const GPUTPCGMPhysicalTrackModel& GPUrestrict() t0e);
  GPUd() o2::base::MatBudget getMatBudget(const float* p1, const float* p2);

  GPUd() void SetPolynomialField(const GPUTPCGMPolynomialField* field) { mField = field; }

  GPUd() void SelectFieldRegion(FieldRegion region) { mFieldRegion = region; }

  GPUd() void SetFitInProjections(bool Flag) { mFitInProjections = Flag; }
  GPUd() void SetPropagateBzOnly(bool Flag) { mPropagateBzOnly = Flag; }
  GPUd() void SetToyMCEventsFlag(bool Flag) { mToyMCEvents = Flag; }
  GPUd() void SetSeedingErrors(bool Flag) { mSeedingErrors = Flag; }
  GPUd() void SetMatLUT(const o2::base::MatLayerCylSet* lut) { mMatLUT = lut; }

  GPUd() void SetMaxSinPhi(float maxSinPhi) { mMaxSinPhi = maxSinPhi; }

  GPUd() void SetTrack(GPUTPCGMTrackParam* track, float Alpha);
  GPUd() void ResetT0()
  {
    if (!mT) {
      return;
    }
    mT0.Set(*mT);
  }

  GPUd() int32_t RotateToAlpha(float newAlpha);

  GPUd() int32_t PropagateToXAlpha(float posX, float posAlpha, bool inFlyDirection);

  GPUd() int32_t PropagateToXAlphaBz(float posX, float posAlpha, bool inFlyDirection);

  GPUd() int32_t Update(float posY, float posZ, int32_t iRow, const GPUParam& param, int16_t clusterState, int8_t rejectChi2, gputpcgmmergertypes::InterpolationErrorHit* inter, bool refit, int8_t sideC, float time, float avgInvCharge, float invCharge GPUCA_DEBUG_STREAMER_CHECK(, DebugStreamerVals* debugVals = nullptr));
  GPUd() int32_t Update(float posY, float posZ, int16_t clusterState, bool rejectChi2, float err2Y, float err2Z, const GPUParam* param = nullptr);
  GPUd() int32_t InterpolateReject(const GPUParam& param, float posY, float posZ, int16_t clusterState, int8_t rejectChi2, gputpcgmmergertypes::InterpolationErrorHit* inter, float err2Y, float err2Z);
  GPUd() float PredictChi2(float posY, float posZ, int32_t iRow, const GPUParam& param, int16_t clusterState, int8_t sideC, float time, float avgCharge, float charge) const;
  GPUd() float PredictChi2(float posY, float posZ, float err2Y, float err2Z) const;
  GPUd() int32_t RejectCluster(float chiY, float chiZ, uint8_t clusterState)
  {
    if (chiY > 9.f || chiZ > 9.f) {
      return 2;
    }
    if ((chiY > 6.25f || chiZ > 6.25f) && (clusterState & (GPUTPCGMMergedTrackHit::flagSplit | GPUTPCGMMergedTrackHit::flagShared))) {
      return 2;
    }
    if ((chiY > 1.f || chiZ > 6.25f) && (clusterState & (GPUTPCGMMergedTrackHit::flagEdge | GPUTPCGMMergedTrackHit::flagSingle))) {
      return 2;
    }
    return 0;
  }

  /// Bz in local coordinates rotated to mAlpha
  GPUd() float GetBz(float X, float Y, float Z) const;
  /// Bx,By,Bz in local coordinates rotated to mAlpha
  GPUd() void GetBxByBz(float X, float Y, float Z, float B[3]) const;

  /// Bz in local coordinates rotated to Alpha
  GPUd() float GetBz(float Alpha, float X, float Y, float Z) const;
  /// Bx,By,Bz in local coordinates rotated to Alpha
  GPUd() void GetBxByBz(float Alpha, float X, float Y, float Z, float B[3]) const;

  GPUd() void GetErr2(float& err2Y, float& err2Z, const GPUParam& param, float posZ, int32_t iRow, int16_t clusterState, int8_t sector, float time, float avgCharge, float charge) const;
  GPUd() static void GetErr2(float& err2Y, float& err2Z, const GPUParam& param, float snp, float tgl, float posZ, float trackX, float trackY, int32_t iRow, int16_t clusterState, int8_t sector, float time, float avgCharge, float charge, bool seedingErrors);

  GPUd() float GetAlpha() const { return mAlpha; }
  GPUd() void SetAlpha(float v) { mAlpha = v; }
  GPUd() float GetQPt0() const { return mT0.GetQPt(); }
  GPUd() float GetSinPhi0() const { return mT0.GetSinPhi(); }
  GPUd() float GetCosPhi0() const { return mT0.GetCosPhi(); }
  GPUd() void Mirror(bool inFlyDirection);
  GPUd() void Rotate180();
  GPUd() void ChangeDirection();
  GPUd() float GetMirroredYModel() const;
  GPUd() float GetMirroredYTrack() const;
  GPUd() int32_t GetPropagatedYZ(float x, float& projY, float& projZ);
  GPUd() bool GetFitInProjections() const { return mFitInProjections; }

  GPUd() GPUTPCGMPhysicalTrackModel& Model()
  {
    return mT0;
  }
  GPUd() void CalculateMaterialCorrection();

 private:
  GPUd() static float ApproximateBetheBloch(float beta2);
  GPUd() int32_t FollowLinearization(const GPUTPCGMPhysicalTrackModel& t0e, float Bz, float dLp, bool inFlyDirection);

  /// Bz in local coordinates rotated to cosAlpha, sinAlpha
  GPUd() float GetBzBase(float cosAlpha, float sinAlpha, float X, float Y, float Z) const;
  /// Bx,By,Bz in local coordinates rotated to cosAlpha, sinAlpha
  GPUd() void GetBxByBzBase(float cosAlpha, float sinAlpha, float X, float Y, float Z, float B[3]) const;
  // X in global coordinates
  GPUd() float getGlobalX(float cosAlpha, float sinAlpha, float X, float Y) const;
  // Y in global coordinates
  GPUd() float getGlobalY(float cosAlpha, float sinAlpha, float X, float Y) const;
  // X in global coordinates
  GPUd() float getGlobalX(float X, float Y) const;
  // Y in global coordinates
  GPUd() float getGlobalY(float X, float Y) const;

  const GPUTPCGMPolynomialField* mField = nullptr;
  const o2::base::MatLayerCylSet* mMatLUT = nullptr;
  GPUTPCGMTrackParam* mT = nullptr;
  float mAlpha = 0.f;    // rotation angle of the track coordinate system
  float mCosAlpha = 1.f; // cos of the rotation angle
  float mSinAlpha = 0.f; // sin of the rotation angle
  float mMaxSinPhi = GPUCA_MAX_SIN_PHI;
  GPUTPCGMPhysicalTrackModel mT0;
  MaterialCorrection mMaterial;
  FieldRegion mFieldRegion = TPC;
  bool mSeedingErrors = 0;
  bool mFitInProjections = 1; // fit (Y,SinPhi,QPt) and (Z,DzDs) paramteres separatelly
  bool mPropagateBzOnly = 0;  // Use Bz only in propagation
  bool mToyMCEvents = 0;      // events are simulated with simple home-made simulation
};

GPUdi() void GPUTPCGMPropagator::GetBxByBz(float Alpha, float X, float Y, float Z, float B[3]) const
{
  float c, s;
  CAMath::SinCos(Alpha, s, c);
  GetBxByBzBase(c, s, X, Y, Z, B);
}

GPUdi() float GPUTPCGMPropagator::GetBz(float Alpha, float X, float Y, float Z) const
{
  float c, s;
  CAMath::SinCos(Alpha, s, c);
  return GetBzBase(c, s, X, Y, Z);
}

GPUdi() void GPUTPCGMPropagator::GetBxByBz(float X, float Y, float Z, float B[3]) const
{
  GetBxByBzBase(mCosAlpha, mSinAlpha, X, Y, Z, B);
}

GPUdi() float GPUTPCGMPropagator::GetBz(float X, float Y, float Z) const
{
  return GetBzBase(mCosAlpha, mSinAlpha, X, Y, Z);
}

GPUdi() void GPUTPCGMPropagator::SetMaterial(float radLen, float rho)
{
  mMaterial.rho = rho;
  mMaterial.radLen = radLen;
  mMaterial.radLenInv = (radLen > 1.e-4f) ? 1.f / radLen : 0.f;
  CalculateMaterialCorrection();
}

GPUdi() void GPUTPCGMPropagator::SetTrack(GPUTPCGMTrackParam* GPUrestrict() track, float Alpha)
{
  mT = track;
  if (!mT) {
    return;
  }
  mT0.Set(*mT);
  mAlpha = Alpha;
  CAMath::SinCos(mAlpha, mSinAlpha, mCosAlpha);
  CalculateMaterialCorrection();
}

GPUdi() float GPUTPCGMPropagator::GetMirroredYModel() const
{
  float Bz = GetBz(mT0.GetX(), mT0.GetY(), mT0.GetZ());
  return mT0.GetMirroredY(Bz);
}

GPUdi() float GPUTPCGMPropagator::GetMirroredYTrack() const
{
  if (!mT) {
    return -1.E10f;
  }
  float Bz = GetBz(mT->GetX(), mT->GetY(), mT->GetZ());
  return mT->GetMirroredY(Bz);
}

GPUdi() float GPUTPCGMPropagator::getGlobalX(float cosAlpha, float sinAlpha, float X, float Y) const
{
  return X * cosAlpha - Y * sinAlpha;
}

GPUdi() float GPUTPCGMPropagator::getGlobalY(float cosAlpha, float sinAlpha, float X, float Y) const
{
  return X * sinAlpha + Y * cosAlpha;
}

GPUdi() float GPUTPCGMPropagator::getGlobalX(float X, float Y) const
{
  return getGlobalX(mCosAlpha, mSinAlpha, X, Y);
}

GPUdi() float GPUTPCGMPropagator::getGlobalY(float X, float Y) const
{
  return getGlobalY(mCosAlpha, mSinAlpha, X, Y);
}

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
