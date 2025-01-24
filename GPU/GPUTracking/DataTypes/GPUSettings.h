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

/// \file GPUSettings.h
/// \author David Rohr

#ifndef GPUSETTINGS_H
#define GPUSETTINGS_H

#include "GPUCommonDef.h"
#include "GPUDataTypes.h"
#ifndef GPUCA_GPUCODE_DEVICE
#include <vector>
#include <string>
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUDisplayFrontendInterface;
class GPUReconstruction;

class GPUSettings
{
 public:
  enum CompressionModes { CompressionTruncate = 1,
                          CompressionDifferences = 2,
                          CompressionTrackModel = 4,
                          CompressionFull = 7 };
  enum CompressionSort { SortTime = 0,
                         SortPad = 1,
                         SortZTimePad = 2,
                         SortZPadTime = 3,
                         SortNoSort = 4 };
  enum CompressionRejection { RejectionNone = 0,
                              RejectionStrategyA = 1,
                              RejectionStrategyB = 2 };

  static constexpr const uint32_t TPC_MAX_TF_TIME_BIN = ((256 * 3564 + 2 * 8 - 2) / 8);
};

// Settings describing the global run parameters
struct GPUSettingsGRP {
  // All new members must be sizeof(int32_t) resp. sizeof(float) for alignment reasons!, default value for newly added members for old data will be 0.
  float solenoidBzNominalGPU = -5.00668f; // solenoid field strength
  int32_t constBz = 0;                    // for test-MC events with constant Bz
  int32_t homemadeEvents = 0;             // Toy-MC events
  int32_t grpContinuousMaxTimeBin = -2;   // 0 for triggered events, -1 for automatic setting, -2 invalid default
  int32_t needsClusterer = 0;             // Set to true if the data requires the clusterizer
  int32_t doCompClusterDecode = 0;        // Set to true if the data contains compressed TPC clusters
  int32_t tpcCutTimeBin = 0;              // Cut TPC clusters and digits >= this cut
};

// Parameters of the current time frame
struct GPUSettingsTF {
  int32_t hasTfStartOrbit = 0;
  int32_t tfStartOrbit = 0;
  int32_t hasRunStartOrbit = 0;
  int32_t runStartOrbit = 0;
  int32_t hasSimStartOrbit = 0;
  int32_t simStartOrbit = 0;
  int32_t hasNHBFPerTF = 0;
  int32_t nHBFPerTF = 0;
};

// Settings defining the setup of the GPUReconstruction processing (basically selecting the device / class instance)
struct GPUSettingsDeviceBackend {
  uint32_t deviceType = GPUDataTypes::DeviceType::CPU;     // Device type, shall use GPUDataTypes::DEVICE_TYPE constants, e.g. CPU / CUDA
  uint8_t forceDeviceType = 1;                             // Fail if device initialization fails, otherwise falls back to CPU
  GPUReconstruction* master = nullptr;                     // GPUReconstruction master object
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#ifdef GPUCA_GPUCODE_DEVICE
#define QCONFIG_GPU
#endif
// See GPUSettingsList.h for a list of all available settings of GPU Reconstruction
#ifndef GPUCA_GPUCODE_GENRTC
#include "utils/qconfig.h"
#endif

#endif
