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

/// \file GPUTrackParamConvert.h
/// \author David Rohr

#ifndef O2_GPU_TRACKPARAMCONVERT_H
#define O2_GPU_TRACKPARAMCONVERT_H

#include "GPUO2DataTypes.h"
#include "GPUTPCGMTrackParam.h"
#include "GPUTPCGMMergedTrack.h"
#include "GPUTPCGMPropagator.h"
#include "ReconstructionDataFormats/Track.h"
#include "DetectorsBase/Propagator.h"
#include "DataFormatsTPC/TrackTPC.h"

namespace o2::gpu
{

GPUdi() static void convertTrackParam(GPUTPCGMTrackParam& trk, const o2::track::TrackParCov& trkX)
{
  for (int i = 0; i < 5; i++) {
    trk.Par()[i] = trkX.getParams()[i];
  }
  for (int i = 0; i < 15; i++) {
    trk.Cov()[i] = trkX.getCov()[i];
  }
  trk.X() = trkX.getX();
}
GPUdi() static void convertTrackParam(o2::track::TrackParCov& trk, const GPUTPCGMTrackParam& trkX)
{
  for (int i = 0; i < 5; i++) {
    trk.setParam(trkX.GetPar()[i], i);
  }
  for (int i = 0; i < 15; i++) {
    trk.setCov(trkX.GetCov()[i], i);
  }
  trk.setX(trkX.GetX());
}

} // namespace o2::gpu

#endif
