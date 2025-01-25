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

/// \file GPUDisplayHelpers.cxx
/// \author David Rohr

#include "GPUDisplay.h"

#ifdef WITH_OPENMP
#include <omp.h>
#endif
#ifndef _WIN32
#include "bitmapfile.h"
#endif

using namespace GPUCA_NAMESPACE::gpu;

int32_t GPUDisplay::getNumThreads()
{
  if (mChain) {
    return mChain->GetProcessingSettings().ompThreads;
  } else {
#ifdef WITH_OPENMP
    return omp_get_max_threads();
#else
    return 1;
#endif
  }
}

void GPUDisplay::updateOptions()
{
  if (mCfgH.splitCETracks == -1 && mParam) {
    mCfgH.splitCETracks = mParam->continuousMaxTimeBin != 0;
  }
}

void GPUDisplay::disableUnsupportedOptions()
{
  if (!mIOPtrs->mergedTrackHitAttachment) {
    mCfgH.markAdjacentClusters = 0;
  }
  if (!mQA) {
    mCfgH.markFakeClusters = 0;
  }
  if (!mChain) {
    mCfgL.excludeClusters = mCfgL.drawInitLinks = mCfgL.drawLinks = mCfgL.drawSeeds = mCfgL.drawTracklets = mCfgL.drawTracks = mCfgL.drawGlobalTracks = 0;
  }
  if (mConfig.showTPCTracksFromO2Format && mParam->par.earlyTpcTransform) {
    throw std::runtime_error("Cannot run GPU display with early Transform when input is O2 tracks");
  }
}

void GPUDisplay::DoScreenshot(const char* filename, std::vector<char>& pixels, float animateTime)
{
  size_t screenshot_x = mBackend->mScreenWidth * mCfgR.screenshotScaleFactor;
  size_t screenshot_y = mBackend->mScreenHeight * mCfgR.screenshotScaleFactor;
  size_t size = 4 * screenshot_x * screenshot_y;
  if (size != pixels.size()) {
    GPUError("Pixel array of incorrect size obtained");
    filename = nullptr;
  }

  if (filename) {
    FILE* fp = fopen(filename, "w+b");
    if (fp == nullptr) {
      GPUError("Error opening screenshot file %s", filename);
      return;
    }

    BITMAPFILEHEADER bmpFH;
    BITMAPINFOHEADER bmpIH;
    memset(&bmpFH, 0, sizeof(bmpFH));
    memset(&bmpIH, 0, sizeof(bmpIH));

    bmpFH.bfType = 19778; //"BM"
    bmpFH.bfSize = sizeof(bmpFH) + sizeof(bmpIH) + size;
    bmpFH.bfOffBits = sizeof(bmpFH) + sizeof(bmpIH);

    bmpIH.biSize = sizeof(bmpIH);
    bmpIH.biWidth = screenshot_x;
    bmpIH.biHeight = screenshot_y;
    bmpIH.biPlanes = 1;
    bmpIH.biBitCount = 32;
    bmpIH.biCompression = BI_RGB;
    bmpIH.biSizeImage = size;
    bmpIH.biXPelsPerMeter = 5670;
    bmpIH.biYPelsPerMeter = 5670;

    fwrite(&bmpFH, 1, sizeof(bmpFH), fp);
    fwrite(&bmpIH, 1, sizeof(bmpIH), fp);
    fwrite(pixels.data(), 1, size, fp);
    fclose(fp);
  }
}

void GPUDisplay::showInfo(const char* info)
{
  mBackend->prepareText();
  float colorValue = mCfgL.invertColors ? 0.f : 1.f;
  OpenGLPrint(info, 40.f, 20.f + std::max(20, mDrawTextFontSize + 4), colorValue, colorValue, colorValue, 1);
  if (mInfoText2Timer.IsRunning()) {
    if (mInfoText2Timer.GetCurrentElapsedTime() >= 6) {
      mInfoText2Timer.Reset();
    } else {
      OpenGLPrint(mInfoText2, 40.f, 20.f, colorValue, colorValue, colorValue, 6 - mInfoText2Timer.GetCurrentElapsedTime());
    }
  }
  if (mInfoHelpTimer.IsRunning() || mPrintInfoTextAlways) {
    if (mInfoHelpTimer.GetCurrentElapsedTime() >= 6) {
      mInfoHelpTimer.Reset();
    } else {
      PrintGLHelpText(colorValue);
    }
  }
  mBackend->finishText();
}

void GPUDisplay::OpenGLPrint(const char* s, float x, float y, float r, float g, float b, float a, bool fromBotton)
{
  if (mBackend->mFreetypeInitialized) {
    if (!fromBotton) {
      y = mBackend->mScreenHeight - y;
    }
    float color[4] = {r, g, b, a};
    mBackend->OpenGLPrint(s, x, y, color, 1.0f);
  } else if (mFrontend->mCanDrawText) {
    mFrontend->OpenGLPrint(s, x, y, r, g, b, a, fromBotton);
  }
}
