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

/// \file GPUTPCClusterFilter.cxx
/// \author David Rohr

#include "GPUTPCClusterFilter.h"
#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/ClusterNative.h"

using namespace o2::gpu;

GPUTPCClusterFilter::GPUTPCClusterFilter(const o2::tpc::ClusterNativeAccess& clusters)
{
  // Could initialize private variables based on the clusters here
}

bool GPUTPCClusterFilter::filter(uint32_t sector, uint32_t row, o2::tpc::ClusterNative& cl)
{
  // Return true to keep the cluster, false to drop it.
  // May change cluster properties by modifying the cl reference.
  // Note that this function might be called multiple times for the same cluster, in which case the final modified cl reference goes into the output clusters.
  return true;
}
#endif
