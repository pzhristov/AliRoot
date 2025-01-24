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

/// \file GPUTRDSpacePoint.h
/// \author Ole Schmidt, ole.schmidt@cern.ch
/// \brief Struct to hold the position/direction information of the tracklets transformed in sector coordinates

#ifndef GPUTRDSPACEPOINT_H
#define GPUTRDSPACEPOINT_H

#ifndef GPUCA_TPC_GEOMETRY_O2 // compatibility to Run 2 data types

namespace GPUCA_NAMESPACE
{
namespace gpu
{

// class to hold the information on the space points
class GPUTRDSpacePoint
{
 public:
  GPUd() GPUTRDSpacePoint(float x = 0, float y = 0, float z = 0, float dy = 0) : mX(x), mY(y), mZ(z), mDy(dy) {}
  GPUd() float getX() const { return mX; }
  GPUd() float getY() const { return mY; }
  GPUd() float getZ() const { return mZ; }
  GPUd() float getDy() const { return mDy; }
  GPUd() void setX(float x) { mX = x; }
  GPUd() void setY(float y) { mY = y; }
  GPUd() void setZ(float z) { mZ = z; }
  GPUd() void setDy(float dy) { mDy = dy; }

 private:
  float mX;  // x position (3.5 mm above anode wires) - radial offset due to t0 mis-calibration, measured -1 mm for run 245353
  float mY;  // y position (sector coordinates)
  float mZ;  // z position (sector coordinates)
  float mDy; // deflection over drift length
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#else // compatibility with Run 3 data types

#include "DataFormatsTRD/CalibratedTracklet.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUTRDSpacePoint : public o2::trd::CalibratedTracklet
{
};

static_assert(sizeof(GPUTRDSpacePoint) == sizeof(o2::trd::CalibratedTracklet), "Incorrect memory layout");

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUCA_TPC_GEOMETRY_O2

#endif // GPUTRDSPACEPOINT_H
