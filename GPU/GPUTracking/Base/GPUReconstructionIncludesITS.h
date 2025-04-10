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

/// \file GPUReconstructionIncludesITS.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONINCLDUESITS_H
#define GPURECONSTRUCTIONINCLDUESITS_H

#if defined(GPUCA_HAVE_O2HEADERS) && !defined(GPUCA_STANDALONE)
#include "ITStracking/TrackerTraits.h"
#include "ITStracking/VertexerTraits.h"
#include "ITStracking/TimeFrame.h"
#if defined(__CUDACC__) || defined(__HIPCC__)
#include "ITStrackingGPU/TrackerTraitsGPU.h"
#include "ITStrackingGPU/VertexerTraitsGPU.h"
#include "ITStrackingGPU/TimeFrameGPU.h"
#endif
#else
namespace o2::its
{
class VertexerTraits
{
};
class TrackerTraits
{
};
class TimeFrame
{
};
class VertexerTraitsGPU : public VertexerTraits
{
};
template <int32_t NLayers = 7>
class TrackerTraitsGPU : public TrackerTraits
{
};
namespace gpu
{
template <int32_t NLayers = 7>
class TimeFrameGPU : public TimeFrame
{
};
} // namespace gpu
} // namespace o2::its
#endif

#endif
