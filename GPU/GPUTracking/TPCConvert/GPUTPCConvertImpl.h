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

/// \file GPUTPCConvertImpl.h
/// \author David Rohr

#ifndef O2_GPU_GPUTPCCONVERTIMPL_H
#define O2_GPU_GPUTPCCONVERTIMPL_H

#include "GPUCommonDef.h"
#include "GPUConstantMem.h"
#include "TPCFastTransform.h"
#include "CorrectionMapsHelper.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUTPCConvertImpl
{
 public:
  GPUd() static void convert(const GPUConstantMem& GPUrestrict() cm, int32_t slice, int32_t row, float pad, float time, float& GPUrestrict() x, float& GPUrestrict() y, float& GPUrestrict() z)
  {
    if (cm.param.par.continuousTracking) {
      cm.calibObjects.fastTransformHelper->getCorrMap()->TransformInTimeFrame(slice, row, pad, time, x, y, z, cm.param.continuousMaxTimeBin);
    } else {
      cm.calibObjects.fastTransformHelper->Transform(slice, row, pad, time, x, y, z);
    }
  }
  GPUd() static void convert(const TPCFastTransform& GPUrestrict() transform, const GPUParam& GPUrestrict() param, int32_t slice, int32_t row, float pad, float time, float& GPUrestrict() x, float& GPUrestrict() y, float& GPUrestrict() z)
  {
    if (param.par.continuousTracking) {
      transform.TransformInTimeFrame(slice, row, pad, time, x, y, z, param.continuousMaxTimeBin);
    } else {
      transform.Transform(slice, row, pad, time, x, y, z);
    }
  }
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
