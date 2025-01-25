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

/// \file GPUConfigDump.cxx
/// \author David Rohr

#include "GPUConfigDump.h"
#include "GPUDataTypes.h"
#include "GPUSettings.h"

#include <functional>
#include <iostream>
#include <cstdio>

#include "utils/qconfig_helpers.h"

using namespace GPUCA_NAMESPACE::gpu;

namespace
{
GPUSettingsStandalone configStandalone;
std::vector<std::function<void()>> qprint_global;
#define QCONFIG_PRINT
#include "utils/qconfig.h"
#undef QCONFIG_PRINT
} // namespace

void GPUConfigDump::dumpConfig(const GPUSettingsRec* rec, const GPUSettingsProcessing* proc, const GPUSettingsQA* qa, const GPUSettingsDisplay* display, const GPUSettingsDeviceBackend* device, const GPURecoStepConfiguration* workflow)
{
  if (rec) {
    qConfigPrint(*rec, "rec.");
  }
  if (proc) {
    qConfigPrint(*proc, "proc.");
  }
  if (qa) {
    qConfigPrint(*qa, "QA.");
  }
  if (display) {
    qConfigPrint(*display, "display.");
  }
  if (device) {
    std::cout << "\n\tGPUSettingsDeviceBackend:\n"
              << "\tdeviceType = " << (int32_t)device->deviceType << "\n"
              << "\tforceDeviceType = " << (device->forceDeviceType ? "true" : "false") << "\n"
              << "\tslave = " << (device->master ? "true" : "false") << "\n";
  }
  if (workflow) {
    printf("\n\tReconstruction steps / inputs / outputs:\n\tReco Steps = 0x%08x\n\tReco Steps GPU = 0x%08x\n\tInputs = 0x%08x\n\tOutputs = 0x%08x\n", (uint32_t)workflow->steps, (uint32_t)workflow->stepsGPUMask, (uint32_t)workflow->inputs, (uint32_t)workflow->outputs);
  }
}
