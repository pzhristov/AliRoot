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

/// \file GPUCommonDef.h
/// \author David Rohr

// This is the base header to be included by all files that should feature GPU suppurt.
// Incompatible code that cannot compile on GPU must be protected by one of the checks below.
// The usual approach would be to protect with GPUCA_GPUCODE. This will be sufficient for all functions. If header includes still show errors, use GPUCA_ALIGPUCODE

// The following checks are increasingly more strict hiding the code in more and more cases:
// #ifndef __OPENCL__ : Hide from OpenCL kernel code. All system headers and usage thereof must be protected like this, or stronger.
// #ifndef GPUCA_GPUCODE_DEVICE : Hide from kernel code on all GPU architectures. This includes the __OPENCL__ case and bodies of all GPU device functions (GPUd(), etc.)
// #ifndef GPUCA_GPUCODE : Hide from compilation with GPU compiler. This includes the kernel case of GPUCA_GPUCODE_DEVICE but also all host code compiled by the GPU compiler, e.g. for management.
// #ifndef GPUCA_ALIGPUCODE : Code is completely invisible to the GPUCATracking library, irrespective of GPU or CPU compilation or which compiler.

#ifndef GPUCOMMONDEF_H
#define GPUCOMMONDEF_H

// clang-format off

//Some GPU configuration settings, must be included first
#include "GPUCommonDefSettings.h"

#if !(defined(__CLING__) || defined(__ROOTCLING__) || defined(G__ROOT)) // No GPU code for ROOT
  #if defined(__CUDACC__) || defined(__OPENCL__) || defined(__HIPCC__) || defined(__OPENCL_HOST__)
    #define GPUCA_GPUCODE // Compiled by GPU compiler
  #endif

  #if defined(__CUDA_ARCH__) || defined(__OPENCL__) || defined(__HIP_DEVICE_COMPILE__)
    #define GPUCA_GPUCODE_DEVICE // Executed on device
  #endif
#endif

// Set AliRoot / O2 namespace
#if defined(GPUCA_STANDALONE) || (defined(GPUCA_O2_LIB) && !defined(GPUCA_O2_INTERFACE)) || defined(GPUCA_ALIROOT_LIB) || defined (GPUCA_GPUCODE)
  #define GPUCA_ALIGPUCODE
#endif
#ifdef GPUCA_ALIROOT_LIB
  #define GPUCA_NAMESPACE AliGPU
#else
  #define GPUCA_NAMESPACE o2
#endif

#if (defined(__CUDACC__) && defined(GPUCA_CUDA_NO_CONSTANT_MEMORY)) || (defined(__HIPCC__) && defined(GPUCA_HIP_NO_CONSTANT_MEMORY)) || (defined(__OPENCL__) && defined(GPUCA_OPENCL_NO_CONSTANT_MEMORY))
  #define GPUCA_NO_CONSTANT_MEMORY
#elif defined(__CUDACC__) || defined(__HIPCC__)
  #define GPUCA_HAS_GLOBAL_SYMBOL_CONSTANT_MEM
#endif
#if !defined(GPUCA_HAVE_O2HEADERS) && (defined(GPUCA_O2_LIB) || (!defined(GPUCA_ALIROOT_LIB) && !defined(GPUCA_STANDALONE)))
  #define GPUCA_HAVE_O2HEADERS
#endif

#if defined(GPUCA_HAVE_O2HEADERS) && !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE) && defined(DEBUG_STREAMER)
#define GPUCA_DEBUG_STREAMER_CHECK(...) __VA_ARGS__
#else
#define GPUCA_DEBUG_STREAMER_CHECK(...)
#endif

#ifndef GPUCA_RTC_SPECIAL_CODE
#define GPUCA_RTC_SPECIAL_CODE(...)
#endif

// API Definitions for GPU Compilation
#include "GPUCommonDefAPI.h"

// clang-format on

#endif
