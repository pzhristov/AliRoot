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

/// \file GPUTPCGMPolynomialField.h
/// \author Sergey Gorbunov, David Rohr

#ifndef GPUTPCGMPOLYNOMIALFIELD_H
#define GPUTPCGMPOLYNOMIALFIELD_H

#include "GPUCommonDef.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
/**
 * @class GPUTPCGMPolynomialField
 *
 */

class GPUTPCGMPolynomialField
{
 public:
  GPUTPCGMPolynomialField() : mNominalBz(0.f)
  {
    Reset();
  }

  void Reset();

  void SetFieldNominal(float nominalBz);

  void SetFieldTpc(const float* Bx, const float* By, const float* Bz);
  void SetFieldTrd(const float* Bx, const float* By, const float* Bz);
  void SetFieldIts(const float* Bx, const float* By, const float* Bz);

  GPUdi() float GetNominalBz() const { return mNominalBz; }

  GPUd() void GetField(float x, float y, float z, float* B) const;
  GPUd() float GetFieldBz(float x, float y, float z) const;

  GPUd() void GetFieldTrd(float x, float y, float z, float* B) const;
  GPUd() float GetFieldTrdBz(float x, float y, float z) const;

  GPUd() void GetFieldIts(float x, float y, float z, float* B) const;
  GPUd() float GetFieldItsBz(float x, float y, float z) const;

  void Print() const;

  static constexpr const int32_t NTPCM = 10; // number of coefficients
  static constexpr const int32_t NTRDM = 20; // number of coefficients for the TRD field
  static constexpr const int32_t NITSM = 10; // number of coefficients for the ITS field

  GPUd() static void GetPolynomsTpc(float x, float y, float z, float f[NTPCM]);
  GPUd() static void GetPolynomsTrd(float x, float y, float z, float f[NTRDM]);
  GPUd() static void GetPolynomsIts(float x, float y, float z, float f[NITSM]);

  const float* GetCoefmTpcBx() const { return mTpcBx; }
  const float* GetCoefmTpcBy() const { return mTpcBy; }
  const float* GetCoefmTpcBz() const { return mTpcBz; }

  const float* GetCoefmTrdBx() const { return mTrdBx; }
  const float* GetCoefmTrdBy() const { return mTrdBy; }
  const float* GetCoefmTrdBz() const { return mTrdBz; }

  const float* GetCoefmItsBx() const { return mItsBx; }
  const float* GetCoefmItsBy() const { return mItsBy; }
  const float* GetCoefmItsBz() const { return mItsBz; }

 private:
  float mNominalBz;    // nominal constant field value in [kG * 2.99792458E-4 GeV/c/cm]
  float mTpcBx[NTPCM]; // polynomial coefficients
  float mTpcBy[NTPCM];
  float mTpcBz[NTPCM];
  float mTrdBx[NTRDM]; // polynomial coefficients
  float mTrdBy[NTRDM];
  float mTrdBz[NTRDM];
  float mItsBx[NITSM]; // polynomial coefficients
  float mItsBy[NITSM];
  float mItsBz[NITSM];
};

inline void GPUTPCGMPolynomialField::Reset()
{
  mNominalBz = 0.f;
  for (int32_t i = 0; i < NTPCM; i++) {
    mTpcBx[i] = 0.f;
    mTpcBy[i] = 0.f;
    mTpcBz[i] = 0.f;
  }
  for (int32_t i = 0; i < NTRDM; i++) {
    mTrdBx[i] = 0.f;
    mTrdBy[i] = 0.f;
    mTrdBz[i] = 0.f;
  }
  for (int32_t i = 0; i < NITSM; i++) {
    mItsBx[i] = 0.f;
    mItsBy[i] = 0.f;
    mItsBz[i] = 0.f;
  }
}

inline void GPUTPCGMPolynomialField::SetFieldNominal(float nominalBz) { mNominalBz = nominalBz; }

inline void GPUTPCGMPolynomialField::SetFieldTpc(const float* Bx, const float* By, const float* Bz)
{
  if (Bx && By && Bz) {
    for (int32_t i = 0; i < NTPCM; i++) {
      mTpcBx[i] = Bx[i];
      mTpcBy[i] = By[i];
      mTpcBz[i] = Bz[i];
    }
  }
}

inline void GPUTPCGMPolynomialField::SetFieldTrd(const float* Bx, const float* By, const float* Bz)
{
  if (Bx && By && Bz) {
    for (int32_t i = 0; i < NTRDM; i++) {
      mTrdBx[i] = Bx[i];
      mTrdBy[i] = By[i];
      mTrdBz[i] = Bz[i];
    }
  }
}

inline void GPUTPCGMPolynomialField::SetFieldIts(const float* Bx, const float* By, const float* Bz)
{
  if (Bx && By && Bz) {
    for (int32_t i = 0; i < NITSM; i++) {
      mItsBx[i] = Bx[i];
      mItsBy[i] = By[i];
      mItsBz[i] = Bz[i];
    }
  }
}

GPUdi() void GPUTPCGMPolynomialField::GetPolynomsTpc(float x, float y, float z, float f[NTPCM])
{
  f[0] = 1.f;
  f[1] = x;
  f[2] = y;
  f[3] = z;
  f[4] = x * x;
  f[5] = x * y;
  f[6] = x * z;
  f[7] = y * y;
  f[8] = y * z;
  f[9] = z * z;
}

GPUdi() void GPUTPCGMPolynomialField::GetField(float x, float y, float z, float* B) const
{
  const float* fBxS = &mTpcBx[1];
  const float* fByS = &mTpcBy[1];
  const float* fBzS = &mTpcBz[1];

  const float f[NTPCM - 1] = {x, y, z, x * x, x * y, x * z, y * y, y * z, z * z};
  float bx = mTpcBx[0], by = mTpcBy[0], bz = mTpcBz[0];
  for (int32_t i = NTPCM - 1; i--;) {
    // for (int32_t i=0;i<NTPCM-1; i++){
    bx += fBxS[i] * f[i];
    by += fByS[i] * f[i];
    bz += fBzS[i] * f[i];
  }
  B[0] = bx;
  B[1] = by;
  B[2] = bz;
}

GPUdi() float GPUTPCGMPolynomialField::GetFieldBz(float x, float y, float z) const
{
  const float* fBzS = &mTpcBz[1];

  const float f[NTPCM - 1] = {x, y, z, x * x, x * y, x * z, y * y, y * z, z * z};
  float bz = mTpcBz[0];
  for (int32_t i = NTPCM - 1; i--;) {
    bz += fBzS[i] * f[i];
  }
  return bz;
}

GPUdi() void GPUTPCGMPolynomialField::GetPolynomsTrd(float x, float y, float z, float f[NTRDM])
{
  float xx = x * x, xy = x * y, xz = x * z, yy = y * y, yz = y * z, zz = z * z;
  f[0] = 1.f;
  f[1] = x;
  f[2] = y;
  f[3] = z;
  f[4] = xx;
  f[5] = xy;
  f[6] = xz;
  f[7] = yy;
  f[8] = yz;
  f[9] = zz;
  f[10] = x * xx;
  f[11] = x * xy;
  f[12] = x * xz;
  f[13] = x * yy;
  f[14] = x * yz;
  f[15] = x * zz;
  f[16] = y * yy;
  f[17] = y * yz;
  f[18] = y * zz;
  f[19] = z * zz;
}

GPUdi() void GPUTPCGMPolynomialField::GetFieldTrd(float x, float y, float z, float* B) const
{
  float f[NTRDM];
  GetPolynomsTrd(x, y, z, f);
  float bx = 0.f, by = 0.f, bz = 0.f;
  for (int32_t i = 0; i < NTRDM; i++) {
    bx += mTrdBx[i] * f[i];
    by += mTrdBy[i] * f[i];
    bz += mTrdBz[i] * f[i];
  }
  B[0] = bx;
  B[1] = by;
  B[2] = bz;
}

GPUdi() float GPUTPCGMPolynomialField::GetFieldTrdBz(float x, float y, float z) const
{
  float f[NTRDM];
  GetPolynomsTrd(x, y, z, f);
  float bz = 0.f;
  for (int32_t i = 0; i < NTRDM; i++) {
    bz += mTrdBz[i] * f[i];
  }
  return bz;
}

GPUdi() void GPUTPCGMPolynomialField::GetPolynomsIts(float x, float y, float z, float f[NITSM])
{
  float xx = x * x, xy = x * y, xz = x * z, yy = y * y, yz = y * z, zz = z * z;
  f[0] = 1.f;
  f[1] = x;
  f[2] = y;
  f[3] = z;
  f[4] = xx;
  f[5] = xy;
  f[6] = xz;
  f[7] = yy;
  f[8] = yz;
  f[9] = zz;
  /*
        f[10]=x*xx; f[11]=x*xy; f[12]=x*xz; f[13]=x*yy; f[14]=x*yz; f[15]=x*zz;
        f[16]=y*yy; f[17]=y*yz; f[18]=y*zz;
        f[19]=z*zz;
   */
}

GPUdi() void GPUTPCGMPolynomialField::GetFieldIts(float x, float y, float z, float* B) const
{
  const float* fBxS = &mItsBx[1];
  const float* fByS = &mItsBy[1];
  const float* fBzS = &mItsBz[1];

  const float f[NITSM - 1] = {x, y, z, x * x, x * y, x * z, y * y, y * z, z * z};
  float bx = mItsBx[0], by = mItsBy[0], bz = mItsBz[0];
  for (int32_t i = NITSM - 1; i--;) {
    bx += fBxS[i] * f[i];
    by += fByS[i] * f[i];
    bz += fBzS[i] * f[i];
  }
  B[0] = bx;
  B[1] = by;
  B[2] = bz;
}

GPUdi() float GPUTPCGMPolynomialField::GetFieldItsBz(float x, float y, float z) const
{
  const float* fBzS = &mItsBz[1];

  const float f[NITSM - 1] = {x, y, z, x * x, x * y, x * z, y * y, y * z, z * z};
  float bz = mItsBz[0];
  for (int32_t i = NITSM - 1; i--;) {
    bz += fBzS[i] * f[i];
  }
  return bz;
}

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
