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

/// \file GPUTPCCompressionKernels.cxx
/// \author David Rohr

#include "GPUO2DataTypes.h"
#include "GPUParam.h"
#include "GPUTPCGeometry.h"
#include "GPUTPCClusterRejection.h"

namespace o2::gpu
{

GPUdii() void GPUTPCCompression_EncodeUnattached(uint8_t nComppressionModes, const o2::tpc::ClusterNative& orgCl, uint32_t& outTime, uint16_t& outPad, const o2::tpc::ClusterNative* orgClPre = nullptr)
{
  if (nComppressionModes & GPUSettings::CompressionDifferences) {
    uint32_t lastTime = 0, lastPad = 0;
    if (orgClPre) {
      lastPad = orgClPre->padPacked;
      lastTime = orgClPre->getTimePacked();
    }

    outPad = orgCl.padPacked - lastPad;
    outTime = (orgCl.getTimePacked() - lastTime) & 0xFFFFFF;
  } else {
    outPad = orgCl.padPacked;
    outTime = orgCl.getTimePacked();
  }
}

} // namespace o2::gpu
