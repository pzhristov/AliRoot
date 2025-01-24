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

/// \file GPUErrors.h
/// \author David Rohr

#ifndef GPUERRORS_H
#define GPUERRORS_H

#include "GPUCommonDef.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUErrors
{
 public:
  enum errorNumbers {
#define GPUCA_ERROR_CODE(num, name, ...) name = num,
#include "GPUErrorCodes.h"
#undef GPUCA_ERROR_CODE
  };

  GPUd() void raiseError(uint32_t code, uint32_t param1 = 0, uint32_t param2 = 0, uint32_t param3 = 0) const;
  GPUd() bool hasError() { return *mErrors > 0; }
  void setMemory(GPUglobalref() uint32_t* m) { mErrors = m; }
  void clear();
  void printErrors(bool silent = false);
  uint32_t getNErrors() const;
  const uint32_t* getErrorPtr() const;
  static uint32_t getMaxErrors();

 private:
  GPUglobalref() uint32_t* mErrors;
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
