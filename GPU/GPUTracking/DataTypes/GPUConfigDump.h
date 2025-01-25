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

/// \file GPUConfigDump.h
/// \author David Rohr

#ifndef GPUCONFIGDUMP_H
#define GPUCONFIGDUMP_H

#include "GPUCommonDef.h"

namespace GPUCA_NAMESPACE::gpu
{
struct GPUSettingsRec;
struct GPUSettingsProcessing;
struct GPUSettingsQA;
struct GPUSettingsDisplay;
struct GPUSettingsDeviceBackend;
struct GPURecoStepConfiguration;

class GPUConfigDump
{
 public:
  static void dumpConfig(const GPUSettingsRec* rec, const GPUSettingsProcessing* proc, const GPUSettingsQA* qa, const GPUSettingsDisplay* display, const GPUSettingsDeviceBackend* device, const GPURecoStepConfiguration* workflow);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
