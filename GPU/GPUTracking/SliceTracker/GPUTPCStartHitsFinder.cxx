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

/// \file GPUTPCStartHitsFinder.cxx
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#include "GPUTPCStartHitsFinder.h"
#include "GPUTPCTracker.h"
#include "GPUCommonMath.h"

using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUdii() void GPUTPCStartHitsFinder::Thread<0>(int32_t /*nBlocks*/, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& s, processorType& GPUrestrict() tracker)
{
  // find start hits for tracklets
  if (iThread == 0) {
    s.mIRow = iBlock + 1;
    s.mNRowStartHits = 0;
    if (s.mIRow <= GPUCA_ROW_COUNT - 4) {
      s.mNHits = tracker.mData.mRows[s.mIRow].mNHits;
    } else {
      s.mNHits = -1;
    }
  }
  GPUbarrier();
  GPUglobalref() const GPUTPCRow& GPUrestrict() row = tracker.mData.mRows[s.mIRow];
  GPUglobalref() const GPUTPCRow& GPUrestrict() rowUp = tracker.mData.mRows[s.mIRow + 2];
  for (int32_t ih = iThread; ih < s.mNHits; ih += nThreads) {
    int64_t lHitNumberOffset = row.mHitNumberOffset;
    uint32_t linkUpData = tracker.mData.mLinkUpData[lHitNumberOffset + ih];

    if (tracker.mData.mLinkDownData[lHitNumberOffset + ih] == CALINK_INVAL && linkUpData != CALINK_INVAL && tracker.mData.mLinkUpData[rowUp.mHitNumberOffset + linkUpData] != CALINK_INVAL) {
#ifdef GPUCA_SORT_STARTHITS
      GPUglobalref() GPUTPCHitId* const GPUrestrict() startHits = tracker.mTrackletTmpStartHits + s.mIRow * tracker.mNMaxRowStartHits;
      uint32_t nextRowStartHits = CAMath::AtomicAddShared(&s.mNRowStartHits, 1u);
      if (nextRowStartHits >= tracker.mNMaxRowStartHits) {
        tracker.raiseError(GPUErrors::ERROR_ROWSTARTHIT_OVERFLOW, tracker.ISlice() * 1000 + s.mIRow, nextRowStartHits, tracker.mNMaxRowStartHits);
        CAMath::AtomicExchShared(&s.mNRowStartHits, tracker.mNMaxRowStartHits);
        break;
      }
#else
      GPUglobalref() GPUTPCHitId* const GPUrestrict() startHits = tracker.mTrackletStartHits;
      uint32_t nextRowStartHits = CAMath::AtomicAdd(&tracker.mCommonMem->nStartHits, 1u);
      if (nextRowStartHits >= tracker.mNMaxStartHits) {
        tracker.raiseError(GPUErrors::ERROR_STARTHIT_OVERFLOW, tracker.ISlice() * 1000 + s.mIRow, nextRowStartHits, tracker.mNMaxStartHits);
        CAMath::AtomicExch(&tracker.mCommonMem->nStartHits, tracker.mNMaxStartHits);
        break;
      }
#endif
      startHits[nextRowStartHits].Set(s.mIRow, ih);
    }
  }
  GPUbarrier();

#ifdef GPUCA_SORT_STARTHITS
  if (iThread == 0) {
    uint32_t nOffset = CAMath::AtomicAdd(&tracker.mCommonMem->nStartHits, s.mNRowStartHits);
    tracker.mRowStartHitCountOffset[s.mIRow] = s.mNRowStartHits;
    if (nOffset + s.mNRowStartHits > tracker.mNMaxStartHits) {
      tracker.raiseError(GPUErrors::ERROR_STARTHIT_OVERFLOW, tracker.ISlice() * 1000 + s.mIRow, nOffset + s.mNRowStartHits, tracker.mNMaxStartHits);
      CAMath::AtomicExch(&tracker.mCommonMem->nStartHits, tracker.mNMaxStartHits);
    }
  }
#endif
}
