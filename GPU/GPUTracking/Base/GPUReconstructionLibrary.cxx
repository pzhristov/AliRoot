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

/// \file GPUReconstruction.cxx
/// \author David Rohr

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <conio.h>
#else
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef WITH_OPENMP
#include <omp.h>
#endif

#include "GPUReconstruction.h"
#include "GPUReconstructionAvailableBackends.h"

#include "utils/qlibload.h"

#include "GPULogging.h"

using namespace GPUCA_NAMESPACE::gpu;

GPUReconstruction* GPUReconstruction::CreateInstance(DeviceType type, bool forceType, GPUReconstruction* master)
{
  GPUSettingsDeviceBackend cfg;
  new (&cfg) GPUSettingsDeviceBackend;
  cfg.deviceType = type;
  cfg.forceDeviceType = forceType;
  cfg.master = master;
  return CreateInstance(cfg);
}

GPUReconstruction* GPUReconstruction::CreateInstance(const GPUSettingsDeviceBackend& cfg)
{
  GPUReconstruction* retVal = nullptr;
  DeviceType type = (DeviceType)cfg.deviceType;
#ifdef DEBUG_STREAMER
  if (type != DeviceType::CPU) {
    GPUError("Cannot create GPUReconstruction for a non-CPU device if DEBUG_STREAMER are enabled");
    return nullptr;
  }
#endif
  if (type == DeviceType::CPU) {
    retVal = GPUReconstruction_Create_CPU(cfg);
  } else {
    auto* loader = GetLibraryInstance(type, true);
    if (loader && (retVal = (*loader)->GetPtr(cfg))) {
      retVal->mMyLib = *loader;
    }
  }

  if (retVal == nullptr) {
    if (cfg.forceDeviceType) {
      GPUError("Error: Could not load GPUReconstruction for specified device: %s (%u)", GPUDataTypes::DEVICE_TYPE_NAMES[type], cfg.deviceType);
    } else if (type != DeviceType::CPU) {
      GPUError("Could not load GPUReconstruction for device type %s (%u), falling back to CPU version", GPUDataTypes::DEVICE_TYPE_NAMES[type], cfg.deviceType);
      GPUSettingsDeviceBackend cfg2 = cfg;
      cfg2.deviceType = DeviceType::CPU;
      retVal = CreateInstance(cfg2);
    }
  } else {
    GPUInfo("Created GPUReconstruction instance for device type %s (%u)%s", GPUDataTypes::DEVICE_TYPE_NAMES[type], cfg.deviceType, cfg.master ? " (slave)" : "");
  }

  return retVal;
}

bool GPUReconstruction::CheckInstanceAvailable(DeviceType type, bool verbose)
{
  if (type == DeviceType::CPU) {
    return true;
  } else {
    auto* loader = GetLibraryInstance(type, verbose);
    return loader != nullptr && (*loader)->LoadLibrary() == 0;
  }
}

std::shared_ptr<GPUReconstruction::LibraryLoader>* GPUReconstruction::GetLibraryInstance(DeviceType type, bool verbose)
{
  if (type == DeviceType::CPU) {
    return nullptr;
  } else if (type == DeviceType::CUDA) {
#ifdef CUDA_ENABLED
    return &sLibCUDA;
#endif
  } else if (type == DeviceType::HIP) {
#ifdef HIP_ENABLED
    return &sLibHIP;
#endif
  } else if (type == DeviceType::OCL) {
#ifdef OPENCL_ENABLED
    return &sLibOCL;
#endif
  } else {
    GPUError("Error: Invalid device type %u", (uint32_t)type);
    return nullptr;
  }
  if (verbose) {
    GPUInfo("%s Support not compiled in for device type %u (%s)", GPUDataTypes::DEVICE_TYPE_NAMES[type], (uint32_t)type, GPUDataTypes::DEVICE_TYPE_NAMES[type]);
  }
  return nullptr;
}

GPUReconstruction* GPUReconstruction::CreateInstance(const char* type, bool forceType, GPUReconstruction* master)
{
  DeviceType t = GPUDataTypes::GetDeviceType(type);
  if (t == DeviceType::INVALID_DEVICE) {
    GPUError("Invalid device type: %s", type);
    return nullptr;
  }
  return CreateInstance(t, forceType, master);
}

std::shared_ptr<GPUReconstruction::LibraryLoader> GPUReconstruction::sLibCUDA(new GPUReconstruction::LibraryLoader("lib" LIBRARY_PREFIX "GPUTrackingCUDA" LIBRARY_EXTENSION, "GPUReconstruction_Create_CUDA"));
std::shared_ptr<GPUReconstruction::LibraryLoader> GPUReconstruction::sLibHIP(new GPUReconstruction::LibraryLoader("lib" LIBRARY_PREFIX "GPUTrackingHIP" LIBRARY_EXTENSION, "GPUReconstruction_Create_HIP"));
std::shared_ptr<GPUReconstruction::LibraryLoader> GPUReconstruction::sLibOCL(new GPUReconstruction::LibraryLoader("lib" LIBRARY_PREFIX "GPUTrackingOCL" LIBRARY_EXTENSION, "GPUReconstruction_Create_OCL"));

GPUReconstruction::LibraryLoader::LibraryLoader(const char* lib, const char* func) : mLibName(lib), mFuncName(func), mGPULib(nullptr), mGPUEntry(nullptr) {}

GPUReconstruction::LibraryLoader::~LibraryLoader() { CloseLibrary(); }

int32_t GPUReconstruction::LibraryLoader::LoadLibrary()
{
  static std::mutex mut;
  std::lock_guard<std::mutex> lock(mut);

  if (mGPUEntry) {
    return 0;
  }

  LIBRARY_TYPE hGPULib;
  hGPULib = LIBRARY_LOAD(mLibName);
  if (hGPULib == nullptr) {
#ifndef _WIN32
    GPUImportant("The following error occured during dlopen: %s", dlerror());
#endif
    GPUError("Error Opening cagpu library for GPU Tracker (%s)", mLibName);
    return 1;
  } else {
    void* createFunc = LIBRARY_FUNCTION(hGPULib, mFuncName);
    if (createFunc == nullptr) {
      GPUError("Error fetching entry function in GPU library\n");
      LIBRARY_CLOSE(hGPULib);
      return 1;
    } else {
      mGPULib = (void*)(size_t)hGPULib;
      mGPUEntry = createFunc;
      GPUInfo("GPU Tracker library loaded and GPU tracker object created sucessfully");
    }
  }
  return 0;
}

GPUReconstruction* GPUReconstruction::LibraryLoader::GetPtr(const GPUSettingsDeviceBackend& cfg)
{
  if (LoadLibrary()) {
    return nullptr;
  }
  if (mGPUEntry == nullptr) {
    return nullptr;
  }
  GPUReconstruction* (*tmp)(const GPUSettingsDeviceBackend& cfg) = (GPUReconstruction * (*)(const GPUSettingsDeviceBackend& cfg)) mGPUEntry;
  return tmp(cfg);
}

int32_t GPUReconstruction::LibraryLoader::CloseLibrary()
{
  if (mGPUEntry == nullptr) {
    return 1;
  }
  LIBRARY_CLOSE((LIBRARY_TYPE)(size_t)mGPULib);
  mGPULib = nullptr;
  mGPUEntry = nullptr;
  return 0;
}
