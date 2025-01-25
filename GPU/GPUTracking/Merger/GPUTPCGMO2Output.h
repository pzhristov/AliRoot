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

/// \file GPUTPCGMO2Output.h
/// \author David Rohr

#ifndef GPUTPCGMO2OUTPUT_H
#define GPUTPCGMO2OUTPUT_H

#include "GPUTPCDef.h"
#include "GPUTPCGMMergerGPU.h"

namespace o2
{
namespace gpu
{

class GPUTPCGMO2Output : public GPUTPCGMMergerGeneral
{
 public:
  enum K { prepare = 0,
           sort = 1,
           output = 2,
           mc = 3 };
  template <int32_t iKernel = defaultKernel>
  GPUd() static void Thread(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& smem, processorType& merger);
};

} // namespace gpu
} // namespace o2

#endif
