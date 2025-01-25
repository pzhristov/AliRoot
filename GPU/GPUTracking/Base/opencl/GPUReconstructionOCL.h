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

/// \file GPUReconstructionOCL.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONOCL_H
#define GPURECONSTRUCTIONOCL_H

#include "GPUReconstructionDeviceBase.h"

#ifdef _WIN32
extern "C" __declspec(dllexport) GPUCA_NAMESPACE::gpu::GPUReconstruction* GPUReconstruction_Create_OCL(const GPUCA_NAMESPACE::gpu::GPUSettingsDeviceBackend& cfg);
#else
extern "C" GPUCA_NAMESPACE::gpu::GPUReconstruction* GPUReconstruction_Create_OCL(const GPUCA_NAMESPACE::gpu::GPUSettingsDeviceBackend& cfg);
#endif

namespace GPUCA_NAMESPACE::gpu
{
struct GPUReconstructionOCLInternals;

class GPUReconstructionOCLBackend : public GPUReconstructionDeviceBase
{
 public:
  ~GPUReconstructionOCLBackend() override;

 protected:
  GPUReconstructionOCLBackend(const GPUSettingsDeviceBackend& cfg);

  int32_t InitDevice_Runtime() override;
  int32_t ExitDevice_Runtime() override;
  void UpdateAutomaticProcessingSettings() override;

  int32_t GPUFailedMsgAI(const int64_t error, const char* file, int32_t line);
  void GPUFailedMsgA(const int64_t error, const char* file, int32_t line);

  void SynchronizeGPU() override;
  int32_t DoStuckProtection(int32_t stream, deviceEvent event) override;
  int32_t GPUDebug(const char* state = "UNKNOWN", int32_t stream = -1, bool force = false) override;
  void SynchronizeStream(int32_t stream) override;
  void SynchronizeEvents(deviceEvent* evList, int32_t nEvents = 1) override;
  void StreamWaitForEvents(int32_t stream, deviceEvent* evList, int32_t nEvents = 1) override;
  bool IsEventDone(deviceEvent* evList, int32_t nEvents = 1) override;

  size_t WriteToConstantMemory(size_t offset, const void* src, size_t size, int32_t stream = -1, deviceEvent* ev = nullptr) override;
  size_t GPUMemCpy(void* dst, const void* src, size_t size, int32_t stream, int32_t toGPU, deviceEvent* ev = nullptr, deviceEvent* evList = nullptr, int32_t nEvents = 1) override;
  void ReleaseEvent(deviceEvent ev) override;
  void RecordMarker(deviceEvent* ev, int32_t stream) override;

  virtual bool ContextForAllPlatforms() { return false; }

  template <class T, int32_t I = 0>
  int32_t AddKernel(bool multi = false);
  template <class T, int32_t I = 0>
  uint32_t FindKernel(int32_t num);
  template <typename K, typename... Args>
  int32_t runKernelBackendInternal(const krnlSetupTime& _xyz, K& k, const Args&... args);
  template <class T, int32_t I = 0>
  gpu_reconstruction_kernels::krnlProperties getKernelPropertiesBackend();

  GPUReconstructionOCLInternals* mInternals;

  template <class T, int32_t I = 0, typename... Args>
  int32_t runKernelBackend(const krnlSetupArgs<T, I, Args...>& args);
  template <class S, class T, int32_t I, bool MULTI>
  S& getKernelObject();

  int32_t GetOCLPrograms();
  bool CheckPlatform(uint32_t i);
};

using GPUReconstructionOCL = GPUReconstructionKernels<GPUReconstructionOCLBackend>;
} // namespace GPUCA_NAMESPACE::gpu

#endif
