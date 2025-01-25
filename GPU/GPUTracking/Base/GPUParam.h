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

/// \file GPUParam.h
/// \author David Rohr, Sergey Gorbunov

#ifndef GPUPARAM_H
#define GPUPARAM_H

#include "GPUCommonDef.h"
#include "GPUCommonMath.h"
#include "GPUDef.h"
#include "GPUSettings.h"
#include "GPUTPCGeometry.h"
#include "GPUTPCGMPolynomialField.h"

#if !defined(GPUCA_GPUCODE)
namespace o2::base
{
template <typename>
class PropagatorImpl;
using Propagator = PropagatorImpl<float>;
} // namespace o2::base
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUSettingsRec;
struct GPUSettingsGTP;
struct GPURecoStepConfiguration;

struct GPUParamSlice {
  float Alpha;              // slice angle
  float CosAlpha, SinAlpha; // sign and cosine of the slice angle
  float AngleMin, AngleMax; // minimal and maximal angle
  float ZMin, ZMax;         // slice Z range
};

namespace internal
{
template <class T, class S>
struct GPUParam_t {
  T rec;
  S par;

  float bzkG;
  float bzCLight;
  float qptB5Scaler;

  int8_t dodEdxDownscaled;
  int32_t continuousMaxTimeBin;
  int32_t tpcCutTimeBin;

  GPUTPCGeometry tpcGeometry;                       // TPC Geometry
  GPUTPCGMPolynomialField polynomialField;          // Polynomial approx. of magnetic field for TPC GM
  const uint32_t* occupancyMap;                     // Ptr to TPC occupancy map
  uint32_t occupancyTotal;                          // Total occupancy in the TPC (nCl / nHbf)

  GPUParamSlice SliceParam[GPUCA_NSLICES];

 protected:
#ifdef GPUCA_TPC_GEOMETRY_O2
  float ParamErrors[2][4][4]; // cluster error parameterization used during seeding and fit
#else
  float ParamErrorsSeeding0[2][3][4]; // cluster error parameterization used during seeding
  float ParamS0Par[2][3][6];          // cluster error parameterization used during track fit
#endif
};
} // namespace internal

struct GPUParam : public internal::GPUParam_t<GPUSettingsRec, GPUSettingsParam> {

#ifndef GPUCA_GPUCODE
  void SetDefaults(float solenoidBz);
  void SetDefaults(const GPUSettingsGRP* g, const GPUSettingsRec* r = nullptr, const GPUSettingsProcessing* p = nullptr, const GPURecoStepConfiguration* w = nullptr);
  void UpdateSettings(const GPUSettingsGRP* g, const GPUSettingsProcessing* p = nullptr, const GPURecoStepConfiguration* w = nullptr, const GPUSettingsRecDynamic* d = nullptr);
  void UpdateBzOnly(float newSolenoidBz);
  void LoadClusterErrors(bool Print = 0);
  void UpdateRun3ClusterErrors(const float* yErrorParam, const float* zErrorParam);
#endif

  GPUd() float Alpha(int32_t iSlice) const
  {
    if (iSlice >= GPUCA_NSLICES / 2) {
      iSlice -= GPUCA_NSLICES / 2;
    }
    if (iSlice >= GPUCA_NSLICES / 4) {
      iSlice -= GPUCA_NSLICES / 2;
    }
    return 0.174533f + par.dAlpha * iSlice;
  }
  GPUd() float GetClusterErrorSeeding(int32_t yz, int32_t type, float zDiff, float angle2, float unscaledMult) const;
  GPUd() void GetClusterErrorsSeeding2(uint8_t sector, int32_t row, float z, float sinPhi, float DzDs, float time, float& ErrY2, float& ErrZ2) const;
  GPUd() float GetSystematicClusterErrorIFC2(float trackX, float trackY, float z, bool sideC) const;
  GPUd() float GetSystematicClusterErrorC122(float trackX, float trackY, uint8_t sector) const;

  GPUd() float GetClusterError2(int32_t yz, int32_t type, float zDiff, float angle2, float unscaledMult, float scaledAvgInvCharge, float scaledInvCharge) const;
  GPUd() void GetClusterErrors2(uint8_t sector, int32_t row, float z, float sinPhi, float DzDs, float time, float avgInvCharge, float invCharge, float& ErrY2, float& ErrZ2) const;
  GPUd() void UpdateClusterError2ByState(int16_t clusterState, float& ErrY2, float& ErrZ2) const;
  GPUd() float GetUnscaledMult(float time) const;

  GPUd() void Slice2Global(int32_t iSlice, float x, float y, float z, float* X, float* Y, float* Z) const;
  GPUd() void Global2Slice(int32_t iSlice, float x, float y, float z, float* X, float* Y, float* Z) const;

  GPUd() bool rejectEdgeClusterByY(float uncorrectedY, int32_t iRow, float trackSigmaY) const;
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
