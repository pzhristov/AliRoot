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

/// \file GPUDisplayKeys.cxx
/// \author David Rohr

#include "GPUDisplay.h"

using namespace GPUCA_NAMESPACE::gpu;

const char* HelpText[] = {
  "[ESC]                         Quit",
  "[n]                           Next event",
  "[r]                           Reset Display Settings",
  "[l] / [k] / [J]               Draw single slice (next  / previous slice), draw related slices (same plane in phi)",
  "[;] / [:]                     Show splitting of TPC in slices by extruding volume, [:] resets",
  "[#]                           Invert colors",
  "[y] / [Y] / [X] / [M]         Start Animation, Add / remove Animation point, Reset Points, Cycle animation camera mode (resets)",
  "[>] / [<]                     Toggle config interpolation during Animation / change Animation interval (via movement)",
  "[g]                           Draw Grid",
  "[i]                           Project onto XY-plane",
  "[x]                           Exclude Clusters used in the tracking steps enabled for visualization ([1]-[8])",
  "[.]                           Exclude rejected tracks",
  "[c]                           Mark flagged clusters (splitPad = 0x1, splitTime = 0x2, edge = 0x4, singlePad = 0x8, rejectDistance = 0x10, rejectErr = 0x20",
  "[z]                           Mark fake attached clusters",
  "[B]                           Mark clusters attached as adjacent",
  "[L] / [K]                     Draw single collisions (next / previous)",
  "[C]                           Colorcode clusters of different collisions",
  "[v]                           Hide rejected clusters from tracks",
  "[j]                           Show tracks segments propagated to adjacent sector in different color / splt CE tracks",
  "[u]                           Cycle through track filter",
  "[E] / [G]                     Extrapolate tracks / loopers",
  "[t] / [T]                     Take Screenshot / Record Animation to pictures",
  "[Z]                           Change screenshot resolution (scaling factor)",
  "[S] / [A] / [D]               Enable or disable smoothing of points / smoothing of lines / depth buffer",
  "[W] / [U] / [V]               Toggle anti-aliasing (MSAA at raster level / change downsampling FSAA factor) / toggle VSync",
  "[F] / [_] / [R]               Switch mFullScreen / Maximized window / FPS rate limiter",
  "[I]                           Enable / disable GL indirect draw",
  "[o] / [p] / [O] / [P]         Save / restore current camera position / Animation path",
  "[h]                           Print Help",
  "[H]                           Show info texts",
  "[q]                           Start / Stop Qt GUI",
  "[w] / [s] / [a] / [d]         Zoom / Strafe Left and Right",
  "[pgup] / [pgdn]               Strafe up / down",
  "[e] / [f]                     Rotate left / right",
  "[+] / [-]                     Increase / decrease point size (Hold SHIFT for lines)",
  "[b]                           Change FOV (field of view)",
  "[']                           Switch between OpenGL core / compat code path",
  "[MOUSE 1]                     Look around",
  "[MOUSE 2]                     Strafe camera",
  "[MOUSE 1+2]                   Zoom / Rotate",
  "[SHIFT]                       Slow Zoom / Move / Rotate",
  "[ALT] / [CTRL] / [ENTER]      Focus camera on origin / orient y-axis upwards (combine with [SHIFT] to lock) / Cycle through modes",
  "[RCTRL] / [RALT]              Rotate model instead of camera / rotate TPC around beamline",
  "[1] ... [8] / [N]             Enable display of clusters, preseeds, seeds, starthits, tracklets, tracks, global tracks, merged tracks / Show assigned clusters in colors",
  "[F1] / [F2] / [F3] / [F4]     Enable / disable drawing of TPC / TRD / TOF / ITS",
  "[SHIFT] + [F1] to [F4]        Enable / disable track detector filter",
  "[SHIFT] + [F12]               Switch track detector filter between AND and OR mode"
  // FREE: [m] [SPACE] [Q]
  // Test setting: ^
};

void GPUDisplay::PrintHelp()
{
  mInfoHelpTimer.ResetStart();
  for (uint32_t i = 0; i < sizeof(HelpText) / sizeof(HelpText[0]); i++) {
    GPUInfo("%s", HelpText[i]);
  }
}

void GPUDisplay::HandleKey(uint8_t key)
{
  GPUSettingsDisplayHeavy oldCfgH = mCfgH;
  GPUSettingsDisplayLight oldCfgL = mCfgL;
  GPUSettingsDisplayRenderer oldCfgR = mCfgR;
  if (key == 'n') {
    mFrontend->mDisplayControl = 1;
    SetInfo("Showing next event", 1);
  } else if (key == 27 || key == mFrontend->KEY_ESCAPE) {
    mFrontend->mDisplayControl = 2;
    SetInfo("Exiting", 1);
  } else if (key == 'r') {
    mResetScene = 1;
    SetInfo("View reset", 1);
  } else if (key == mFrontend->KEY_ALT && mFrontend->mKeysShift[mFrontend->KEY_ALT]) {
    mCfgR.camLookOrigin ^= 1;
    mCfgR.cameraMode = mCfgR.camLookOrigin + 2 * mCfgR.camYUp;
    SetInfo("Camera locked on origin: %s", mCfgR.camLookOrigin ? "enabled" : "disabled");
  } else if (key == mFrontend->KEY_CTRL && mFrontend->mKeysShift[mFrontend->KEY_CTRL]) {
    mCfgR.camYUp ^= 1;
    mCfgR.cameraMode = mCfgR.camLookOrigin + 2 * mCfgR.camYUp;
    SetInfo("Camera locked on y-axis facing upwards: %s", mCfgR.camYUp ? "enabled" : "disabled");
  } else if (key == mFrontend->KEY_ENTER) {
    mCfgR.cameraMode++;
    if (mCfgR.cameraMode == 4) {
      mCfgR.cameraMode = 0;
    }
    mCfgR.camLookOrigin = mCfgR.cameraMode & 1;
    mCfgR.camYUp = mCfgR.cameraMode & 2;
    const char* modeText[] = {"Descent (free movement)", "Focus locked on origin (y-axis forced upwards)", "Spectator (y-axis forced upwards)", "Focus locked on origin (with free rotation)"};
    SetInfo("Camera mode %d: %s", mCfgR.cameraMode, modeText[mCfgR.cameraMode]);
  } else if (key == mFrontend->KEY_ALT) {
    mFrontend->mKeys[mFrontend->KEY_CTRL] = false; // Release CTRL with alt, to avoid orienting along y automatically!
  } else if (key == 'l') {
    if (mCfgL.drawSlice >= (mCfgL.drawRelatedSlices ? (NSLICES / 4 - 1) : (NSLICES - 1))) {
      mCfgL.drawSlice = -1;
      SetInfo("Showing all slices", 1);
    } else {
      mCfgL.drawSlice++;
      SetInfo("Showing slice %d", mCfgL.drawSlice);
    }
  } else if (key == 'k') {
    if (mCfgL.drawSlice <= -1) {
      mCfgL.drawSlice = mCfgL.drawRelatedSlices ? (NSLICES / 4 - 1) : (NSLICES - 1);
    } else {
      mCfgL.drawSlice--;
    }
    if (mCfgL.drawSlice == -1) {
      SetInfo("Showing all slices", 1);
    } else {
      SetInfo("Showing slice %d", mCfgL.drawSlice);
    }
  } else if (key == 'J') {
    mCfgL.drawRelatedSlices ^= 1;
    SetInfo("Drawing of related slices %s", mCfgL.drawRelatedSlices ? "enabled" : "disabled");
  } else if (key == 'L') {
    if (mCfgL.showCollision >= mNCollissions - 1) {
      mCfgL.showCollision = -1;
      SetInfo("Showing all collisions", 1);
    } else {
      mCfgL.showCollision++;
      SetInfo("Showing collision %d / %d", mCfgL.showCollision, mNCollissions);
    }
  } else if (key == 'K') {
    if (mCfgL.showCollision <= -1) {
      mCfgL.showCollision = mNCollissions - 1;
    } else {
      mCfgL.showCollision--;
    }
    if (mCfgL.showCollision == -1) {
      SetInfo("Showing all collisions", 1);
    } else {
      SetInfo("Showing collision %d / %d", mCfgL.showCollision, mNCollissions);
    }
  } else if (key == 'F') {
    mCfgR.fullScreen ^= 1;
    SetInfo("Toggling full screen (%d)", (int32_t)mCfgR.fullScreen);
  } else if (key == '_') {
    mCfgR.maximized ^= 1;
    SetInfo("Toggling Maximized window (%d)", (int32_t)mCfgR.maximized);
  } else if (key == 'R') {
    mCfgR.maxFPSRate ^= 1;
    SetInfo("FPS rate %s", mCfgR.maxFPSRate ? "not limited" : "limited");
  } else if (key == 'H') {
    mPrintInfoText += 1;
    mPrintInfoText &= 3;
    SetInfo("Info text display - console: %s, onscreen %s", (mPrintInfoText & 2) ? "enabled" : "disabled", (mPrintInfoText & 1) ? "enabled" : "disabled");
  } else if (key == 'j') {
    if (mCfgH.separateGlobalTracks) {
      mCfgH.splitCETracks ^= 1;
    }
    mCfgH.separateGlobalTracks ^= 1;
    SetInfo("Seperated display of tracks propagated to adjacent sectors %s / of CE tracks %s", mCfgH.separateGlobalTracks ? "enabled" : "disabled", mCfgH.splitCETracks ? "enabled" : "disabled");
  } else if (key == 'c') {
    if (mCfgH.markClusters == 0) {
      mCfgH.markClusters = 1;
    } else if (mCfgH.markClusters >= 0x20) {
      mCfgH.markClusters = 0;
    } else {
      mCfgH.markClusters <<= 1;
    }
    SetInfo("Cluster flag highlight mask set to %d (%s)", mCfgH.markClusters,
            mCfgH.markClusters == 0 ? "off" : mCfgH.markClusters == 1 ? "split pad" : mCfgH.markClusters == 2 ? "split time" : mCfgH.markClusters == 4 ? "edge" : mCfgH.markClusters == 8 ? "singlePad" : mCfgH.markClusters == 0x10 ? "reject distance" : "reject error");
  } else if (key == 'z') {
    mCfgH.markFakeClusters ^= 1;
    SetInfo("Marking fake clusters: %s", mCfgH.markFakeClusters ? "on" : "off");
  } else if (key == 'b') {
    if ((mCfgR.fov += 5) > 175) {
      mCfgR.fov = 5;
    }
    SetInfo("Set FOV to %f", mCfgR.fov);
  } else if (key == 39) { // character = "'"
    if (backend()->backendType() == GPUDisplayBackend::TYPE_OPENGL) {
#ifdef GPUCA_DISPLAY_OPENGL_CORE
      SetInfo("OpenGL compat profile not available, using core profile", 1);
#else
      mCfgR.openGLCore ^= 1;
      SetInfo("Using renderer path for OpenGL %s profile", mCfgR.openGLCore ? "core" : "compat");
#endif
    } else {
      SetInfo("OpenGL options only available with OpenGL backend");
    }
  } else if (key == 'B') {
    mCfgH.markAdjacentClusters++;
    if (mCfgH.markAdjacentClusters == 5) {
      mCfgH.markAdjacentClusters = 7;
    }
    if (mCfgH.markAdjacentClusters == 9) {
      mCfgH.markAdjacentClusters = 15;
    }
    if (mCfgH.markAdjacentClusters == 17) {
      mCfgH.markAdjacentClusters = 31;
    }
    if (mCfgH.markAdjacentClusters == 34) {
      mCfgH.markAdjacentClusters = 0;
    }
    if (mCfgH.markAdjacentClusters == 33) {
      SetInfo("Marking protected clusters (%d)", mCfgH.markAdjacentClusters);
    } else if (mCfgH.markAdjacentClusters == 32) {
      SetInfo("Marking removable clusters (%d)", mCfgH.markAdjacentClusters);
    } else {
      SetInfo("Marking adjacent clusters (%d): rejected %s, tube %s, looper leg %s, low Pt %s, high incl %s", mCfgH.markAdjacentClusters, (mCfgH.markAdjacentClusters & 1) ? "yes" : " no", (mCfgH.markAdjacentClusters & 2) ? "yes" : " no", (mCfgH.markAdjacentClusters & 4) ? "yes" : " no", (mCfgH.markAdjacentClusters & 8) ? "yes" : " no", (mCfgH.markAdjacentClusters & 16) ? "yes" : " no");
    }
  } else if (key == 'C') {
    mCfgL.colorCollisions ^= 1;
    SetInfo("Color coding of collisions %s", mCfgL.colorCollisions ? "enabled" : "disabled");
  } else if (key == 'N') {
    mCfgL.colorClusters ^= 1;
    SetInfo("Color coding for seed / trrack attachmend %s", mCfgL.colorClusters ? "enabled" : "disabled");
  } else if (key == 'E') {
    mCfgL.propagateTracks += 1;
    if (mCfgL.propagateTracks == 4) {
      mCfgL.propagateTracks = 0;
    }
    const char* infoText[] = {"Hits connected", "Hits connected and propagated to vertex", "Reconstructed track propagated inwards and outwards", "Monte Carlo track"};
    SetInfo("Display of propagated tracks: %s", infoText[mCfgL.propagateTracks]);
  } else if (key == 'G') {
    mCfgH.propagateLoopers ^= 1;
    SetInfo("Propagation of loopers %s", mCfgH.propagateLoopers ? "enabled" : "disabled");
  } else if (key == 'v') {
    mCfgH.hideRejectedClusters ^= 1;
    SetInfo("Rejected clusters are %s", mCfgH.hideRejectedClusters ? "hidden" : "shown");
  } else if (key == 'i') {
    mCfgH.projectXY ^= 1;
    SetInfo("Projection onto xy plane %s", mCfgH.projectXY ? "enabled" : "disabled");
  } else if (key == 'S') {
    mCfgL.smoothPoints ^= true;
    SetInfo("Smoothing of points %s", mCfgL.smoothPoints ? "enabled" : "disabled");
  } else if (key == 'A') {
    mCfgL.smoothLines ^= true;
    SetInfo("Smoothing of lines %s", mCfgL.smoothLines ? "enabled" : "disabled");
  } else if (key == 'D') {
    mCfgL.depthBuffer ^= true;
    SetInfo("Depth buffer (z-buffer, %u bits) %s", mBackend->DepthBits(), mCfgL.depthBuffer ? "enabled" : "disabled");
    mBackend->setDepthBuffer();
  } else if (key == 'W') {
    mCfgR.drawQualityMSAA *= 2;
    if (mCfgR.drawQualityMSAA < 2) {
      mCfgR.drawQualityMSAA = 2;
    }
    if (mCfgR.drawQualityMSAA > 16 || mCfgR.drawQualityMSAA > mBackend->getMaxMSAA()) {
      mCfgR.drawQualityMSAA = 0;
    }
    SetInfo("Multisampling anti-aliasing factor set to %d", mCfgR.drawQualityMSAA);
  } else if (key == 'U') {
    mCfgR.drawQualityDownsampleFSAA++;
    if (mCfgR.drawQualityDownsampleFSAA == 1) {
      mCfgR.drawQualityDownsampleFSAA = 2;
    }
    if (mCfgR.drawQualityDownsampleFSAA == 5) {
      mCfgR.drawQualityDownsampleFSAA = 0;
    }
    SetInfo("Downsampling anti-aliasing factor set to %d", mCfgR.drawQualityDownsampleFSAA);
  } else if (key == 'V') {
    mCfgR.drawQualityVSync ^= true;
    SetInfo("VSync: %s", mCfgR.drawQualityVSync ? "enabled" : "disabled");
  } else if (key == 'I') {
    mCfgR.useGLIndirectDraw ^= true;
    SetInfo("OpenGL Indirect Draw %s", mCfgR.useGLIndirectDraw ? "enabled" : "disabled");
  } else if (key == ';') {
    mCfgH.xAdd += 60;
    mCfgH.zAdd += 60;
    SetInfo("TPC sector separation: %f %f", mCfgH.xAdd, mCfgH.zAdd);
  } else if (key == ':') {
    mCfgH.xAdd -= 60;
    mCfgH.zAdd -= 60;
    if (mCfgH.zAdd < 0 || mCfgH.xAdd < 0) {
      mCfgH.zAdd = mCfgH.xAdd = 0;
    }
    SetInfo("TPC sector separation: %f %f", mCfgH.xAdd, mCfgH.zAdd);
  } else if (key == '#') {
    mCfgL.invertColors ^= 1;
  } else if (key == 'g') {
    mCfgL.drawGrid ^= 1;
    SetInfo("Fast Cluster Search Grid %s", mCfgL.drawGrid ? "shown" : "hidden");
  } else if (key == 'x') {
    mCfgL.excludeClusters = (mCfgL.excludeClusters + 1) % 3;
    SetInfo(mCfgL.excludeClusters ? "Clusters of selected category are excluded from display (%d)" : "Clusters are shown", mCfgL.excludeClusters);
  } else if (key == '.') {
    mCfgH.hideRejectedTracks ^= 1;
    SetInfo("Rejected tracks are %s", mCfgH.hideRejectedTracks ? "hidden" : "shown");
  } else if (key == '1') {
    mCfgL.drawClusters ^= 1;
  } else if (key == '2') {
    mCfgL.drawInitLinks ^= 1;
  } else if (key == '3') {
    mCfgL.drawLinks ^= 1;
  } else if (key == '4') {
    mCfgL.drawSeeds ^= 1;
  } else if (key == '5') {
    mCfgL.drawTracklets ^= 1;
  } else if (key == '6') {
    mCfgL.drawTracks ^= 1;
  } else if (key == '7') {
    mCfgL.drawGlobalTracks ^= 1;
  } else if (key == '8') {
    mCfgL.drawFinal ^= 1;
  } else if (key == mFrontend->KEY_F1) {
    if (mFrontend->mKeysShift[mFrontend->KEY_F1]) {
      mCfgH.drawTPCTracks ^= 1;
      SetInfo("Track Filter Mask: TPC:%d TRD:%d TOF:%d ITS:%d", (int32_t)mCfgH.drawTPCTracks, (int32_t)mCfgH.drawTRDTracks, (int32_t)mCfgH.drawTOFTracks, (int32_t)mCfgH.drawITSTracks);
    } else {
      mCfgL.drawTPC ^= 1;
      SetInfo("Showing TPC Clusters: %d", (int32_t)mCfgL.drawTPC);
    }
  } else if (key == mFrontend->KEY_F2) {
    if (mFrontend->mKeysShift[mFrontend->KEY_F2]) {
      mCfgH.drawTRDTracks ^= 1;
      SetInfo("Track Filter Mask: TPC:%d TRD:%d TOF:%d ITS:%d", (int32_t)mCfgH.drawTPCTracks, (int32_t)mCfgH.drawTRDTracks, (int32_t)mCfgH.drawTOFTracks, (int32_t)mCfgH.drawITSTracks);
    } else {
      mCfgL.drawTRD ^= 1;
      SetInfo("Showing TRD Tracklets: %d", (int32_t)mCfgL.drawTRD);
    }
  } else if (key == mFrontend->KEY_F3) {
    if (mFrontend->mKeysShift[mFrontend->KEY_F3]) {
      mCfgH.drawTOFTracks ^= 1;
      SetInfo("Track Filter Mask: TPC:%d TRD:%d TOF:%d ITS:%d", (int32_t)mCfgH.drawTPCTracks, (int32_t)mCfgH.drawTRDTracks, (int32_t)mCfgH.drawTOFTracks, (int32_t)mCfgH.drawITSTracks);
    } else {
      mCfgL.drawTOF ^= 1;
      SetInfo("Showing TOF Hits: %d", (int32_t)mCfgL.drawTOF);
    }
  } else if (key == mFrontend->KEY_F4) {
    if (mFrontend->mKeysShift[mFrontend->KEY_F4]) {
      mCfgH.drawITSTracks ^= 1;
      SetInfo("Track Filter Mask: TPC:%d TRD:%d TOF:%d ITS:%d", (int32_t)mCfgH.drawTPCTracks, (int32_t)mCfgH.drawTRDTracks, (int32_t)mCfgH.drawTOFTracks, (int32_t)mCfgH.drawITSTracks);
    } else {
      mCfgL.drawITS ^= 1;
      SetInfo("Showing ITS Clusters: %d", (int32_t)mCfgL.drawITS);
    }
  } else if (key == mFrontend->KEY_F12 && mFrontend->mKeysShift[mFrontend->KEY_F12]) {
    mCfgH.drawTracksAndFilter ^= 1;
    SetInfo("Track filter: %s", mCfgH.drawTracksAndFilter ? "AND" : "OR");
  } else if (key == 't') {
    static int32_t nScreenshot = 1;
    char fname[32];
    snprintf(fname, 32, "screenshot%d.bmp", nScreenshot++);
    mRequestScreenshot = true;
    mScreenshotFile = fname;
    SetInfo("Taking screenshot (%s)", fname);
  } else if (key == 'Z') {
    mCfgR.screenshotScaleFactor += 1;
    if (mCfgR.screenshotScaleFactor == 5) {
      mCfgR.screenshotScaleFactor = 1;
    }
    SetInfo("Screenshot scaling factor set to %d", mCfgR.screenshotScaleFactor);
  } else if (key == 'y' || key == 'T') {
    if (mAnimateVectors[0].size() > 1) {
      if ((mAnimateScreenshot = (key == 'T'))) {
        mAnimationExport++;
      }
      startAnimation();
      SetInfo("Starting Animation", 1);
    } else {
      SetInfo("Insufficient Animation points to start Animation", 1);
    }
  } else if (key == '>') {
    mAnimationChangeConfig ^= 1;
    SetInfo("Interpolating visualization settings during Animation %s", mAnimationChangeConfig ? "enabled" : "disabled");
  } else if (key == 'Y') {
    setAnimationPoint();
    SetInfo("Added Animation point (%d points, %6.2f seconds)", (int32_t)mAnimateVectors[0].size(), mAnimateVectors[0].back());
  } else if (key == 'X') {
    resetAnimation();
    SetInfo("Reset Animation points", 1);
  } else if (key == '\'') {
    removeAnimationPoint();
    SetInfo("Removed Animation point", 1);
  } else if (key == 'M') {
    mCfgL.animationMode++;
    if (mCfgL.animationMode == 7) {
      mCfgL.animationMode = 0;
    }
    resetAnimation();
    if (mCfgL.animationMode == 6) {
      SetInfo("Animation mode %d - Centered on origin", mCfgL.animationMode);
    } else {
      SetInfo("Animation mode %d - Position: %s, Direction: %s", mCfgL.animationMode, (mCfgL.animationMode & 2) ? "Spherical (spherical rotation)" : (mCfgL.animationMode & 4) ? "Spherical (Euler angles)" : "Cartesian", (mCfgL.animationMode & 1) ? "Euler angles" : "Quaternion");
    }
  } else if (key == 'u') {
    mCfgH.trackFilter = (mCfgH.trackFilter + 1) % (mConfig.filterMacros.size() + 1);
    mUpdateTrackFilter = true;
    SetInfo("Track filter: %s", mCfgH.trackFilter == 0 ? "None" : mConfig.filterMacros[mCfgH.trackFilter - 1].c_str());
  } else if (key == 'o') {
    FILE* ftmp = fopen("glpos.tmp", "w+b");
    if (ftmp) {
      int32_t retval = fwrite(&mViewMatrix, sizeof(mViewMatrix), 1, ftmp);
      if (retval != 1) {
        GPUError("Error writing position to file");
      } else {
        GPUInfo("Position stored to file");
      }
      fclose(ftmp);
    } else {
      GPUError("Error opening file");
    }
    SetInfo("Camera position stored to file", 1);
  } else if (key == 'p') {
    FILE* ftmp = fopen("glpos.tmp", "rb");
    if (ftmp) {
      int32_t retval = fread(&mViewMatrix, 1, sizeof(mViewMatrix), ftmp);
      if (retval == sizeof(mViewMatrix)) {
        GPUInfo("Position read from file");
      } else {
        GPUError("Error reading position from file");
      }
      fclose(ftmp);
    } else {
      GPUError("Error opening file");
    }
    SetInfo("Camera position loaded from file", 1);
  } else if (key == 'O') {
    FILE* ftmp = fopen("glanimation.tmp", "w+b");
    if (ftmp) {
      fwrite(&mCfgL, sizeof(mCfgL), 1, ftmp);
      int32_t size = mAnimateVectors[0].size();
      fwrite(&size, sizeof(size), 1, ftmp);
      for (int32_t i = 0; i < 9; i++) {
        fwrite(mAnimateVectors[i].data(), sizeof(mAnimateVectors[i][0]), size, ftmp);
      }
      fwrite(mAnimateConfig.data(), sizeof(mAnimateConfig[0]), size, ftmp);
      fclose(ftmp);
    } else {
      GPUError("Error opening file");
    }
    SetInfo("Animation path stored to file %s", "glanimation.tmp");
  } else if (key == 'P') {
    FILE* ftmp = fopen("glanimation.tmp", "rb");
    if (ftmp) {
      int32_t retval = fread(&mCfgL, sizeof(mCfgL), 1, ftmp);
      int32_t size;
      retval += fread(&size, sizeof(size), 1, ftmp);
      for (int32_t i = 0; i < 9; i++) {
        mAnimateVectors[i].resize(size);
        retval += fread(mAnimateVectors[i].data(), sizeof(mAnimateVectors[i][0]), size, ftmp);
      }
      mAnimateConfig.resize(size);
      retval += fread(mAnimateConfig.data(), sizeof(mAnimateConfig[0]), size, ftmp);
      (void)retval; // disable unused warning
      fclose(ftmp);
      updateConfig();
    } else {
      GPUError("Error opening file");
    }
    SetInfo("Animation path loaded from file %s", "glanimation.tmp");
  } else if (key == 'h') {
    if (mPrintInfoTextAlways) {
      mPrintInfoTextAlways = false;
      SetInfo("Showing help text disabled", 1);
    } else if (mInfoHelpTimer.IsRunning()) {
      mPrintInfoTextAlways = true;
      mInfoHelpTimer.Reset();
      SetInfo("Showing help text until disabled", 1);
    } else {
      PrintHelp();
      SetInfo("Showing help text", 1);
    }
  } else if (key == 'q') {
    if (mFrontend->isGUIRunning()) {
      SetInfo("Stopping GUI", 1);
      mFrontend->stopGUI();
    } else {
      SetInfo("Starting GUI", 1);
      mFrontend->startGUI();
    }
  }
  /*
  else if (key == '^')
  {
    mTestSetting++;
    SetInfo("Debug test variable set to %d", mTestSetting);
  }
  */

  if (memcmp((void*)&oldCfgH, (void*)&mCfgH, sizeof(mCfgH)) != 0) {
    mUpdateEventData = true;
  }
  if (memcmp((void*)&oldCfgL, (void*)&mCfgL, sizeof(mCfgL)) != 0 || memcmp((void*)&oldCfgR, (void*)&mCfgR, sizeof(mCfgR)) != 0) {
    mUpdateDrawCommands = true;
  }
  if (oldCfgR.drawQualityMSAA != mCfgR.drawQualityMSAA || oldCfgR.drawQualityDownsampleFSAA != mCfgR.drawQualityDownsampleFSAA || oldCfgL.depthBuffer != mCfgL.depthBuffer || oldCfgR.screenshotScaleFactor != mCfgR.screenshotScaleFactor) {
    mUpdateRenderPipeline = true;
  }

  if (oldCfgR.drawQualityVSync != mCfgR.drawQualityVSync) {
    mFrontend->SetVSync(mCfgR.drawQualityVSync);
    mBackend->SetVSync(mCfgR.drawQualityVSync);
  }
  if (oldCfgR.fullScreen != mCfgR.fullScreen) {
    mFrontend->SwitchFullscreen(mCfgR.fullScreen);
  }
  if (oldCfgR.maximized != mCfgR.maximized) {
    mFrontend->ToggleMaximized(mCfgR.maximized);
  }
  if (oldCfgR.maxFPSRate != mCfgR.maxFPSRate) {
    mFrontend->mMaxFPSRate = mCfgR.maxFPSRate;
  }
  if (oldCfgR.useGLIndirectDraw != mCfgR.useGLIndirectDraw) {
    mUpdateEventData = true;
  }
}

void GPUDisplay::HandleSendKey(int32_t key)
{
  // GPUError("key %d '%c'", key, (char) key);

  bool shifted = key >= 'A' && key <= 'Z';
  int32_t press = key;
  if (press >= 'a' && press <= 'z') {
    press += 'A' - 'a';
  }
  bool oldShift = mFrontend->mKeysShift[press];
  mFrontend->mKeysShift[press] = shifted;
  HandleKey(key);
  mFrontend->mKeysShift[press] = oldShift;
}

void GPUDisplay::PrintGLHelpText(float colorValue)
{
  for (uint32_t i = 0; i < sizeof(HelpText) / sizeof(HelpText[0]); i++) {
    OpenGLPrint(HelpText[i], 40.f, 35 + std::max(20, mDrawTextFontSize + 4) * (1 + i), colorValue, colorValue, colorValue, mInfoHelpTimer.GetCurrentElapsedTime() >= 5 ? (6 - mInfoHelpTimer.GetCurrentElapsedTime()) : 1, false);
  }
}
