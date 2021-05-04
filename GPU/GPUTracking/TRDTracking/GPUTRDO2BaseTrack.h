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

/// \file GPUTRDO2BaseTrack.h
/// \author Ole Schmidt

#ifndef GPUTRDO2BASETRACK_H
#define GPUTRDO2BASETRACK_H

#include "GPUCommonDef.h"
#include "ReconstructionDataFormats/Track.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUTRDO2BaseTrack : public o2::track::TrackParCov
{
 public:
  GPUdDefault() GPUTRDO2BaseTrack() = default;
  GPUd() GPUTRDO2BaseTrack(const o2::track::TrackParCov& t) : o2::track::TrackParCov(t) {}

 private:
  // dummy class to avoid problems, see https://github.com/AliceO2Group/AliceO2/pull/5969#issuecomment-827475822
  ClassDefNV(GPUTRDO2BaseTrack, 1);
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
