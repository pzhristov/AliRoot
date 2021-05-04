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

/// \file GPUROOTDump.h
/// \author David Rohr

#ifndef GPUROOTDUMP_H
#define GPUROOTDUMP_H

#include "GPUCommonDef.h"
#if !defined(GPUCA_NO_ROOT) && !defined(GPUCA_GPUCODE)
#include "GPUROOTDumpCore.h"
#include <TTree.h>
#include <TNtuple.h>
#include <memory>
#include <stdexcept>
#else
class TNtuple;
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{
#if !defined(GPUCA_NO_ROOT) && !defined(GPUCA_GPUCODE)
namespace
{
template <class S>
struct internal_Branch {
  template <typename... Args>
  static void Branch(S* p, Args... args)
  {
  }
};
template <>
struct internal_Branch<TTree> {
  template <typename... Args>
  static void Branch(TTree* p, Args... args)
  {
    p->Branch(args...);
  }
};
} // namespace

template <class T>
class GPUROOTDump : public GPUROOTDumpBase
{
 public:
  static GPUROOTDump<T>& get(const char* name) // return always the same instance, identified by template
  {
    static GPUROOTDump<T> instance(name);
    return instance;
  }
  static GPUROOTDump<T> getNew(const char* name) // return new individual instance
  {
    return GPUROOTDump<T>(name);
  }

  void write() override { mTree->Write(); }

  void Fill(const T& o)
  {
    mObj = o;
    mTree->Fill();
  }

 private:
  GPUROOTDump(const char* name)
  {
    mTree = new TTree(name, name);
    mTree->Branch(name, &mObj);
  }
  TTree* mTree = nullptr;
  T mObj;
};

template <>
class GPUROOTDump<TNtuple> : public GPUROOTDumpBase
{
 public:
  static GPUROOTDump<TNtuple>& get(const char* name, const char* options)
  {
    static GPUROOTDump<TNtuple> instance(name, options);
    return instance;
  }
  static GPUROOTDump<TNtuple> getNew(const char* name, const char* options)
  {
    return GPUROOTDump<TNtuple>(name, options);
  }

  void write() override { mNTuple->Write(); }

  template <typename... Args>
  void Fill(Args... args)
  {
    mNTuple->Fill(args...);
  }

 private:
  GPUROOTDump(const char* name, const char* options)
  {
    mNTuple = new TNtuple(name, name, options);
  }
  TNtuple* mNTuple;
};
#else
template <class T>
class GPUROOTDump
{
 public:
  template <typename... Args>
  GPUd() void Fill(Args... args) const
  {
  }
  template <typename... Args>
  GPUd() static GPUROOTDump<T>& get(Args... args)
  {
    return *(GPUROOTDump<T>*)(size_t)(1024); // Will never be used, return just some reference
  }
};
#endif
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
