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

/// \file GPUDisplayColors.cxx
/// \author David Rohr

#include "GPUDisplay.h"

using namespace GPUCA_NAMESPACE::gpu;

inline void GPUDisplay::ActivateColor()
{
  mBackend->ActivateColor(mDrawColor);
}

inline void GPUDisplay::SetColorClusters()
{
  if (mCfgL.colorCollisions) {
    return;
  }
  if (mCfgL.invertColors) {
    mDrawColor = {0, 0.3, 0.7, 1.f};
  } else {
    mDrawColor = {0, 0.7, 1.0, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorTRD()
{
  if (mCfgL.colorCollisions) {
    return;
  }
  if (mCfgL.invertColors) {
    mDrawColor = {0.7, 0.3, 0, 1.f};
  } else {
    mDrawColor = {1.0, 0.7, 0, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorITS()
{
  if (mCfgL.colorCollisions) {
    return;
  }
  if (mCfgL.invertColors) {
    mDrawColor = {1.00, 0.1, 0.1, 1.f};
  } else {
    mDrawColor = {1.00, 0.3, 0.3, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorTOF()
{
  if (mCfgL.colorCollisions) {
    return;
  }
  if (mCfgL.invertColors) {
    mDrawColor = {0.1, 1.0, 0.1, 1.f};
  } else {
    mDrawColor = {0.5, 1.0, 0.5, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorInitLinks()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.42, 0.4, 0.1, 1.f};
  } else {
    mDrawColor = {0.42, 0.4, 0.1, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorLinks()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.6, 0.1, 0.1, 1.f};
  } else {
    mDrawColor = {0.8, 0.2, 0.2, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorSeeds()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.6, 0.0, 0.65, 1.f};
  } else {
    mDrawColor = {0.8, 0.1, 0.85, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorTracklets()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0, 0, 0, 1.f};
  } else {
    mDrawColor = {1, 1, 1, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorTracks()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.6, 0, 0.1, 1.f};
  } else {
    mDrawColor = {0.8, 1., 0.15, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorGlobalTracks()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.8, 0.2, 0, 1.f};
  } else {
    mDrawColor = {1.0, 0.4, 0, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorFinal()
{
  if (mCfgL.colorCollisions) {
    return;
  }
  if (mCfgL.invertColors) {
    mDrawColor = {0, 0.6, 0.1, 1.f};
  } else {
    mDrawColor = {0, 0.7, 0.2, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorGrid()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.5, 0.5, 0.0, 1.f};
  } else {
    mDrawColor = {0.7, 0.7, 0.0, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorGridTRD()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.5, 0.5, 0.5, 1.f};
  } else {
    mDrawColor = {0.7, 0.7, 0.5, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetColorMarked()
{
  if (mCfgL.invertColors) {
    mDrawColor = {0.8, 0, 0, 1.f};
  } else {
    mDrawColor = {1.0, 0.0, 0.0, 1.f};
  }
  ActivateColor();
}
inline void GPUDisplay::SetCollisionColor(int32_t col)
{
  int32_t red = (col * 2) % 5;
  int32_t blue = (2 + col * 3) % 7;
  int32_t green = (4 + col * 5) % 6;
  if (mCfgL.invertColors && red == 4 && blue == 5 && green == 6) {
    red = 0;
  }
  if (!mCfgL.invertColors && red == 0 && blue == 0 && green == 0) {
    red = 4;
  }
  mDrawColor = {red / 4.f, green / 5.f, blue / 6.f, 1.f};
  ActivateColor();
}
