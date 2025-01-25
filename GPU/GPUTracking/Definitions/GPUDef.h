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

/// \file GPUDef.h
/// \author David Rohr, Sergey Gorbunov

// clang-format off
#ifndef GPUDEF_H
#define GPUDEF_H

#include "GPUCommonDef.h"
#include "GPUDefConstantsAndSettings.h"
#include "GPUDefGPUParameters.h"
#include "GPUCommonRtypes.h"

// Macros for masking ptrs in OpenCL kernel calls as uint64_t (The API only allows us to pass buffer objects)
#ifdef __OPENCL__
  #define GPUPtr1(a, b) uint64_t b
  #ifdef __OPENCL__
    #define GPUPtr2(a, b) ((__generic a) (a) b)
  #else
    #define GPUPtr2(a, b) ((__global a) (a) b)
  #endif
#else
  #define GPUPtr1(a, b) a b
  #define GPUPtr2(a, b) b
#endif

#ifdef GPUCA_FULL_CLUSTERDATA
  #define GPUCA_EVDUMP_FILE "event_full"
#else
  #define GPUCA_EVDUMP_FILE "event"
#endif

#ifdef GPUCA_GPUCODE
  #define CA_MAKE_SHARED_REF(vartype, varname, varglobal, varshared) const GPUsharedref() vartype& __restrict__ varname = varshared;
  #define CA_SHARED_STORAGE(storage) storage
  #define CA_SHARED_CACHE(target, src, size) \
    static_assert((size) % sizeof(int32_t) == 0, "Invalid shared cache size"); \
    for (uint32_t i_shared_cache = get_local_id(0); i_shared_cache < (size) / sizeof(int32_t); i_shared_cache += get_local_size(0)) { \
      reinterpret_cast<GPUsharedref() int32_t*>(target)[i_shared_cache] = reinterpret_cast<GPUglobalref() const int32_t*>(src)[i_shared_cache]; \
    }
  #define CA_SHARED_CACHE_REF(target, src, size, reftype, ref) \
    CA_SHARED_CACHE(target, src, size) \
    GPUsharedref() const reftype* __restrict__ ref = (target)
#else
  #define CA_MAKE_SHARED_REF(vartype, varname, varglobal, varshared) const GPUglobalref() vartype & __restrict__ varname = varglobal;
  #define CA_SHARED_STORAGE(storage)
  #define CA_SHARED_CACHE(target, src, size)
  #define CA_SHARED_CACHE_REF(target, src, size, reftype, ref) GPUglobalref() const reftype* __restrict__ ref = src
#endif

#ifdef GPUCA_TEXTURE_FETCH_CONSTRUCTOR
  #define CA_TEXTURE_FETCH(type, texture, address, entry) tex1Dfetch(texture, ((char*) address - tracker.Data().GPUTextureBase()) / sizeof(type) + entry);
#else
  #define CA_TEXTURE_FETCH(type, texture, address, entry) address[entry];
#endif

#endif //GPUTPCDEF_H

#ifdef GPUCA_CADEBUG
  #ifdef CADEBUG
    #undef CADEBUG
  #endif
  #ifdef GPUCA_CADEBUG_ENABLED
    #undef GPUCA_CADEBUG_ENABLED
  #endif
  #if GPUCA_CADEBUG == 1 && !defined(GPUCA_GPUCODE)
    #define CADEBUG(...) __VA_ARGS__
    #define CADEBUG2(cmd, ...) {__VA_ARGS__; cmd;}
    #define GPUCA_CADEBUG_ENABLED
  #endif
  #undef GPUCA_CADEBUG
#endif

#ifndef CADEBUG
  #define CADEBUG(...)
  #define CADEBUG2(cmd, ...) {cmd;}
#endif
// clang-format on
