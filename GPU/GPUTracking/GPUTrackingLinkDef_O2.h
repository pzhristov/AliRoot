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

/// \file GPUTrackingLinkDef_O2.h
/// \author David Rohr

#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class o2::gpu::GPUTPCGMMergedTrack + ;
#pragma link C++ class o2::gpu::GPUTPCGMSliceTrack + ;
#pragma link C++ class o2::gpu::GPUTPCGMBorderTrack + ;
#pragma link C++ class o2::gpu::GPUTPCGMTrackParam + ;
#pragma link C++ class o2::gpu::GPUTPCTrack + ;
#pragma link C++ struct o2::gpu::GPUTPCBaseTrackParam + ;
#pragma link C++ struct o2::gpu::GPUTPCGMSliceTrack::sliceTrackParam + ;
#pragma link C++ class o2::gpu::trackInterface < o2::gpu::GPUTPCGMTrackParam> + ;
#pragma link C++ class o2::gpu::GPUTRDTrack_t < o2::gpu::trackInterface < o2::gpu::GPUTPCGMTrackParam>> + ;
#pragma link C++ class o2::gpu::gputpcgmmergertypes::GPUTPCOuterParam + ;
#pragma link C++ class o2::gpu::gputpcgmmergertypes::InterpolationErrorHit + ;

#endif
