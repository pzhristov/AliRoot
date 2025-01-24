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

/// \file GPUTRDTrack.cxx
/// \author Ole Schmidt, Sergey Gorbunov

#include "GPUTRDTrack.h"
#include "GPUTRDInterfaces.h"

using namespace GPUCA_NAMESPACE::gpu;
#include "GPUTRDTrack.inc"

#if !defined(GPUCA_GPUCODE)
namespace GPUCA_NAMESPACE
{
namespace gpu
{
#ifdef GPUCA_ALIROOT_LIB // Instantiate AliRoot track version
template class GPUTRDTrack_t<trackInterface<AliExternalTrackParam>>;
#endif
#if defined(GPUCA_HAVE_O2HEADERS) && !defined(GPUCA_O2_LIB) // Instantiate O2 track version, for O2 this happens in GPUTRDTrackO2.cxx
template class GPUTRDTrack_t<trackInterface<o2::track::TrackParCov>>;
#endif
template class GPUTRDTrack_t<trackInterface<GPUTPCGMTrackParam>>; // Always instatiate GM track version
} // namespace gpu
} // namespace GPUCA_NAMESPACE
#endif
