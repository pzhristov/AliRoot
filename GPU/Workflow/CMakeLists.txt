# Copyright 2019-2020 CERN and copyright holders of ALICE O2.
# See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
# All rights not expressly granted are reserved.
#
# This software is distributed under the terms of the GNU General Public
# License v3 (GPL Version 3), copied verbatim in the file "COPYING".
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization
# or submit itself to any jurisdiction.

o2_add_library(GPUWorkflow
               SOURCES src/GPUWorkflowSpec.cxx
                       src/GPUWorkflowTPC.cxx
                       src/GPUWorkflowITS.cxx
                       src/GPUWorkflowPipeline.cxx
               TARGETVARNAME targetName
               PUBLIC_LINK_LIBRARIES O2::Framework
                                     O2::DataFormatsTPC
                                     O2::DPLUtils
                                     O2::TPCReconstruction
                                     O2::TPCCalibration
                                     O2::TPCSimulation
                                     O2::TPCQC
                                     O2::DetectorsCalibration
                                     O2::TPCReaderWorkflow
                                     O2::DataFormatsGlobalTracking
                                     O2::DataFormatsTRD
               PRIVATE_LINK_LIBRARIES O2::GPUTracking
                                      O2::ITSTrackingInterface)

o2_add_executable(reco-workflow
       COMPONENT_NAME gpu
       TARGETVARNAME targetName
       SOURCES src/gpu-reco-workflow.cxx
       PUBLIC_LINK_LIBRARIES O2::GPUWorkflow
                             O2::GlobalTrackingWorkflowHelpers)

if(ALIGPU_BUILD_TYPE STREQUAL "O2"
  AND OPENGL_FOUND
  AND GLFW_FOUND
  AND TARGET AliceO2::DebugGUI
  AND OPENGL_GLU_FOUND
  AND NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin"
)
  o2_add_executable(display
          COMPONENT_NAME gpu
          TARGETVARNAME targetName
          SOURCES src/O2GPUDPLDisplay.cxx
          PUBLIC_LINK_LIBRARIES O2::GPUO2Interface
                                O2::GPUWorkflowHelper
                                O2::DataFormatsGlobalTracking
                                O2::TPCCalibration
                                O2::TPCFastTransformation
                                O2::TRDBase
                                O2::TOFBase
                                O2::TPCReconstruction
                                O2::GlobalTrackingWorkflowHelpers)
  target_include_directories(${targetName} PUBLIC "include")
endif()

add_subdirectory(helper)
