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

/// \file GPUTPCClusterFilter.h
/// \author David Rohr

#ifndef GPUTPCCLUSTERFILTER_H
#define GPUTPCCLUSTERFILTER_H

#include <cstdint>

namespace o2::tpc
{
struct ClusterNativeAccess;
struct ClusterNative;
} // namespace o2::tpc

namespace o2::gpu
{
class GPUTPCClusterFilter
{
 public:
  GPUTPCClusterFilter(const o2::tpc::ClusterNativeAccess& clusters);
  bool filter(uint32_t sector, uint32_t row, o2::tpc::ClusterNative& cl);
};
} // namespace o2::gpu

#endif
