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

/// \file GPUChainTrackingTRD.cxx
/// \author David Rohr

#include "GPUChainTracking.h"
#include "GPULogging.h"
#include "GPUO2DataTypes.h"
#include "GPUTRDTrackletWord.h"
#include "GPUTRDTrackletLabels.h"
#include "GPUTRDTrack.h"
#include "GPUTRDTracker.h"
#include "utils/strtag.h"

using namespace GPUCA_NAMESPACE::gpu;
using namespace o2::trd;

int GPUChainTracking::RunTRDTracking()
{
  if (!processors()->trdTrackerGPU.IsInitialized()) {
    return 1;
  }

  GPUTRDTrackerGPU& Tracker = processors()->trdTrackerGPU;
  Tracker.Reset();
  if (mIOPtrs.nTRDTracklets == 0) {
    return 0;
  }
  Tracker.SetGenerateSpacePoints(mIOPtrs.trdSpacePoints == nullptr);

  mRec->PushNonPersistentMemory(qStr2Tag("TRDTRACK"));
  SetupGPUProcessor(&Tracker, true);

  for (unsigned int i = 0; i < mIOPtrs.nMergedTracks; i++) {
    const GPUTPCGMMergedTrack& trk = mIOPtrs.mergedTracks[i];
    if (!Tracker.PreCheckTrackTRDCandidate(trk)) {
      continue;
    }
    const GPUTRDTrackGPU& trktrd = param().rec.NWaysOuter ? (GPUTRDTrackGPU)trk.OuterParam() : (GPUTRDTrackGPU)trk;
    if (!Tracker.CheckTrackTRDCandidate(trktrd)) {
      continue;
    }

    if (Tracker.LoadTrack(trktrd, -1, nullptr, -1, i, false)) {
      return 1;
    }
  }

  Tracker.DoTracking(this);

  mIOPtrs.nTRDTracks = Tracker.NTracks();
  mIOPtrs.trdTracks = Tracker.Tracks();
  mRec->PopNonPersistentMemory(RecoStep::TRDTracking, qStr2Tag("TRDTRACK"));

  return 0;
}

int GPUChainTracking::DoTRDGPUTracking()
{
#ifdef HAVE_O2HEADERS
  bool doGPU = GetRecoStepsGPU() & RecoStep::TRDTracking;
  GPUTRDTrackerGPU& Tracker = processors()->trdTrackerGPU;
  GPUTRDTrackerGPU& TrackerShadow = doGPU ? processorsShadow()->trdTrackerGPU : Tracker;

  const auto& threadContext = GetThreadContext();
  SetupGPUProcessor(&Tracker, false);
  TrackerShadow.OverrideGPUGeometry(reinterpret_cast<GPUTRDGeometry*>(mFlatObjectsDevice.mCalibObjects.trdGeometry));

  WriteToConstantMemory(RecoStep::TRDTracking, (char*)&processors()->trdTrackerGPU - (char*)processors(), &TrackerShadow, sizeof(TrackerShadow), 0);
  TransferMemoryResourcesToGPU(RecoStep::TRDTracking, &Tracker, 0);

  runKernel<GPUTRDTrackerKernels>(GetGridAuto(0), krnlRunRangeNone);
  TransferMemoryResourcesToHost(RecoStep::TRDTracking, &Tracker, 0);
  SynchronizeStream(0);

  if (GetProcessingSettings().debugLevel >= 2) {
    GPUInfo("GPU TRD tracker Finished");
  }
#endif
  return (0);
}
