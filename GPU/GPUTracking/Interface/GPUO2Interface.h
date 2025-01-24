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

/// \file GPUO2Interface.h
/// \author David Rohr

#ifndef GPUO2INTERFACE_H
#define GPUO2INTERFACE_H

// Some defines denoting that we are compiling for O2
#ifndef GPUCA_HAVE_O2HEADERS
#define GPUCA_HAVE_O2HEADERS
#endif
#ifndef GPUCA_TPC_GEOMETRY_O2
#define GPUCA_TPC_GEOMETRY_O2
#endif
#ifndef GPUCA_O2_INTERFACE
#define GPUCA_O2_INTERFACE
#endif

#include <memory>
#include <array>
#include <vector>
#include "GPUCommonDef.h"
#include "GPUDataTypes.h"

namespace o2::base
{
template <typename value_T>
class PropagatorImpl;
using Propagator = PropagatorImpl<float>;
} // namespace o2::base
namespace o2::tpc
{
struct ClusterNativeAccess;
struct ClusterNative;
} // namespace o2::tpc

namespace o2::its
{
class TrackerTraits;
class VertexerTraits;
class TimeFrame;
} // namespace o2::its

namespace o2::gpu
{
class GPUReconstruction;
class GPUChainTracking;
class GPUChainITS;
struct GPUO2InterfaceConfiguration;
struct GPUInterfaceOutputs;
struct GPUInterfaceInputUpdate;
struct GPUTrackingOutputs;
struct GPUConstantMem;
struct GPUNewCalibValues;

struct GPUO2Interface_processingContext;
struct GPUO2Interface_Internals;

class GPUO2Interface
{
 public:
  GPUO2Interface();
  ~GPUO2Interface();

  int32_t Initialize(const GPUO2InterfaceConfiguration& config);
  void Deinitialize();

  int32_t RunTracking(GPUTrackingInOutPointers* data, GPUInterfaceOutputs* outputs = nullptr, uint32_t iThread = 0, GPUInterfaceInputUpdate* inputUpdateCallback = nullptr);
  void Clear(bool clearOutputs, uint32_t iThread = 0);
  void DumpEvent(int32_t nEvent, GPUTrackingInOutPointers* data);
  void DumpSettings();

  void GetITSTraits(o2::its::TrackerTraits*& trackerTraits, o2::its::VertexerTraits*& vertexerTraits, o2::its::TimeFrame*& timeFrame);
  const o2::base::Propagator* GetDeviceO2Propagator(int32_t iThread = 0) const;
  void UseGPUPolynomialFieldInPropagator(o2::base::Propagator* prop) const;

  // Updates all calibration objects that are != nullptr in newCalib
  int32_t UpdateCalibration(const GPUCalibObjectsConst& newCalib, const GPUNewCalibValues& newVals, uint32_t iThread = 0);

  int32_t registerMemoryForGPU(const void* ptr, size_t size);
  int32_t unregisterMemoryForGPU(const void* ptr);
  void setErrorCodeOutput(std::vector<std::array<uint32_t, 4>>* v);

  const GPUO2InterfaceConfiguration& getConfig() const { return *mConfig; }

 private:
  GPUO2Interface(const GPUO2Interface&);
  GPUO2Interface& operator=(const GPUO2Interface&);

  bool mContinuous = false;

  uint32_t mNContexts = 0;
  std::unique_ptr<GPUO2Interface_processingContext[]> mCtx;

  std::unique_ptr<GPUO2InterfaceConfiguration> mConfig;
  GPUChainITS* mChainITS = nullptr;
  std::unique_ptr<GPUO2Interface_Internals> mInternals;
};
} // namespace o2::gpu

#endif
