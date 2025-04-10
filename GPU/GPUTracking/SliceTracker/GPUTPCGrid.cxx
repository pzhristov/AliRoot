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

/// \file GPUTPCGrid.cxx
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#include "GPUTPCGrid.h"
#include "GPUCommonMath.h"
using namespace GPUCA_NAMESPACE::gpu;

#if !defined(assert) && !defined(GPUCA_GPUCODE)
#include <cassert>
#endif

GPUd() void GPUTPCGrid::CreateEmpty()
{
  // Create an empty grid
  mYMin = 0.f;
  mYMax = 1.f;
  mZMin = 0.f;
  mZMax = 1.f;

  mNy = 1;
  mNz = 1;
  mN = 1;

  mStepYInv = 1.f;
  mStepZInv = 1.f;
}

GPUd() void GPUTPCGrid::Create(float yMin, float yMax, float zMin, float zMax, int32_t ny, int32_t nz)
{
  //* Create the grid
  mYMin = yMin;
  mZMin = zMin;

  float sy = CAMath::Max((yMax + 0.1f - yMin) / ny, GPUCA_MIN_BIN_SIZE);
  float sz = CAMath::Max((zMax + 0.1f - zMin) / nz, GPUCA_MIN_BIN_SIZE);

  mStepYInv = 1.f / sy;
  mStepZInv = 1.f / sz;

  mNy = ny;
  mNz = nz;

  mN = mNy * mNz;

  mYMax = mYMin + mNy * sy;
  mZMax = mZMin + mNz * sz;
}

GPUd() int32_t GPUTPCGrid::GetBin(float Y, float Z) const
{
  //* get the bin pointer
  const int32_t yBin = static_cast<int32_t>((Y - mYMin) * mStepYInv);
  const int32_t zBin = static_cast<int32_t>((Z - mZMin) * mStepZInv);
  const int32_t bin = zBin * mNy + yBin;
#ifndef GPUCA_GPUCODE
  assert(bin >= 0);
  assert(bin < static_cast<int32_t>(mN));
#endif
  return bin;
}

GPUd() int32_t GPUTPCGrid::GetBinBounded(float Y, float Z) const
{
  //* get the bin pointer
  const int32_t yBin = static_cast<int32_t>((Y - mYMin) * mStepYInv);
  const int32_t zBin = static_cast<int32_t>((Z - mZMin) * mStepZInv);
  int32_t bin = zBin * mNy + yBin;
  if (bin >= static_cast<int32_t>(mN)) {
    bin = mN - 1;
  }
  if (bin < 0) {
    bin = 0;
  }
  return bin;
}

GPUd() void GPUTPCGrid::GetBin(float Y, float Z, int32_t* const bY, int32_t* const bZ) const
{
  //* get the bin pointer

  int32_t bbY = (int32_t)((Y - mYMin) * mStepYInv);
  int32_t bbZ = (int32_t)((Z - mZMin) * mStepZInv);

  if (bbY >= (int32_t)mNy) {
    bbY = mNy - 1;
  }
  if (bbY < 0) {
    bbY = 0;
  }
  if (bbZ >= (int32_t)mNz) {
    bbZ = mNz - 1;
  }
  if (bbZ < 0) {
    bbZ = 0;
  }

  *bY = (uint32_t)bbY;
  *bZ = (uint32_t)bbZ;
}

GPUd() void GPUTPCGrid::GetBinArea(float Y, float Z, float dy, float dz, int32_t& bin, int32_t& ny, int32_t& nz) const
{
  Y -= mYMin;
  int32_t by = (int32_t)((Y - dy) * mStepYInv);
  ny = (int32_t)((Y + dy) * mStepYInv) - by;
  Z -= mZMin;
  int32_t bz = (int32_t)((Z - dz) * mStepZInv);
  nz = (int32_t)((Z + dz) * mStepZInv) - bz;
  if (by >= (int32_t)mNy) {
    by = mNy - 1;
  }
  if (by < 0) {
    by = 0;
  }
  if (bz >= (int32_t)mNz) {
    bz = mNz - 1;
  }
  if (bz < 0) {
    bz = 0;
  }
  if (by + ny >= (int32_t)mNy) {
    ny = mNy - 1 - by;
  }
  if (bz + nz >= (int32_t)mNz) {
    nz = mNz - 1 - bz;
  }
  bin = bz * mNy + by;
}
