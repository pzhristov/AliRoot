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

/// \file GPUTPCMCInfo.h
/// \author David Rohr

#ifndef GPUTPCMCINFO_H
#define GPUTPCMCINFO_H

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUTPCMCInfo {
  int32_t charge;
  int8_t prim;
  int8_t primDaughters;
  int32_t pid;
  float x;
  float y;
  float z;
  float pX;
  float pY;
  float pZ;
  float genRadius;
#ifdef GPUCA_TPC_GEOMETRY_O2
  float t0;
#endif
};
struct GPUTPCMCInfoCol {
  uint32_t first;
  uint32_t num;
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
