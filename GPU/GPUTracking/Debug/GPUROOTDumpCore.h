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

/// \file GPUROOTDumpCore.h
/// \author David Rohr

#ifndef GPUROOTDUMPCORE_H
#define GPUROOTDUMPCORE_H

#include "GPUCommonDef.h"
#include <memory>
#include <vector>

class TFile;

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUROOTDumpCore;

class GPUROOTDumpBase
{
 public:
  virtual void write() = 0;

 protected:
  GPUROOTDumpBase();
  std::weak_ptr<GPUROOTDumpCore> mCore;
};

class GPUROOTDumpCore
{
#if !defined(GPUCA_NO_ROOT) && !defined(GPUCA_GPUCODE)
  friend class GPUReconstruction;
  friend class GPUROOTDumpBase;

 private:
  struct GPUROOTDumpCorePrivate {
  };

 public:
  GPUROOTDumpCore(const GPUROOTDumpCore&) = delete;
  GPUROOTDumpCore operator=(const GPUROOTDumpCore&) = delete;
  GPUROOTDumpCore(GPUROOTDumpCorePrivate); // Cannot be declared private directly since used with new
  ~GPUROOTDumpCore();

 private:
  static std::shared_ptr<GPUROOTDumpCore> getAndCreate();
  static std::weak_ptr<GPUROOTDumpCore> get() { return sInstance; }
  static std::weak_ptr<GPUROOTDumpCore> sInstance;
  std::unique_ptr<TFile> mFile;
  std::vector<GPUROOTDumpBase*> mBranches;
#endif
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
