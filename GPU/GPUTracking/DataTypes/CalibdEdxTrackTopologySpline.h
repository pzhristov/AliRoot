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

/// \file  dEdxCalibrationSplines.h
/// \brief Definition of dEdxCalibrationSplines class
///
/// \author  Matthias Kleiner <matthias.kleiner@cern.ch>

#ifndef CalibdEdxTrackTopologySpline_H
#define CalibdEdxTrackTopologySpline_H

#include "FlatObject.h"
#include "Spline.h"
#include "GPUCommonRtypes.h"
#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/Defs.h"
#endif

#if !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE) // code invisible on GPU and in the standalone compilation
#include <fmt/format.h>
#endif

namespace o2::tpc
{

///
/// The dEdxCalibrationSplines class represents the calibration of the dEdx of mostly geometrical effects
///
/// The splines which are used to correct the dE/dx for detector effects like different pad sizes and
/// track topologies (local dip angle: z angle - dz/dx and diffusion from electron drift) are created by
/// the Random Forest ML algorithm. The training data is created by the O2 simulation with artificial tracks
/// (no Landau distribution, constant dE/dx, fixed GEM amplification of 2000).
/// For each region and for qMax/qTot an individual spline is created which leads to a total of 20 splines.
/// After applying the correction with the splines the dE/dx for qMax and qTot should be normalized such that
/// the MIP position is roughly equal for both.
/// The absolute return values of the splines is arbitrary (only the relative differences between the values matter)
///
/// It is planned to make further topological corrections also with the splines: relative pad and time position
/// of a track, local inclination angle: y angle = dy/dx (improves dE/dx at low momenta) and threshold effects.
///
///
/// Definitions:
///
/// angleZ
/// snp2 = mP[2] * mP[2]
/// tgl2 = mP[3] * mP[3]
/// sec2 = 1.f / (1.f - snp2)
/// angleZ = sqrt(tgl2 * sec2)
///
/// angleY
/// angleY = sqrt( snp2 * sec2 )
///
/// relative pad position
/// relPadPos = COG_Pad - int32_t(COG_Pad + 0.5f);
/// -0.5f<relPadPos<0.5f
///
/// relative time position
/// relTimePos = COG_Time - int32_t(COG_Time + 0.5f);
/// -0.5f<relTimePos<0.5f
///

class CalibdEdxTrackTopologySpline : public o2::gpu::FlatObject
{
 public:
  typedef o2::gpu::Spline<float, 3, 1> SplineType;

  /// _____________  Constructors / destructors __________________________

#if !defined(GPUCA_GPUCODE)
  /// Default constructor
  CalibdEdxTrackTopologySpline() = default;

  /// constructor with initialization of the splines from file
  /// \param dEdxSplinesFile path to root file containing the splines
  CalibdEdxTrackTopologySpline(const char* dEdxSplinesFile, const char* name = "CalibdEdxTrackTopologySpline");

  /// Copy constructor
  CalibdEdxTrackTopologySpline(const CalibdEdxTrackTopologySpline&);

  /// Assignment operator
  CalibdEdxTrackTopologySpline& operator=(const CalibdEdxTrackTopologySpline&);

  void recreate(const int32_t nKnots[]);
#else
  /// Disable constructors for the GPU implementation

  CalibdEdxTrackTopologySpline() = delete;
  CalibdEdxTrackTopologySpline(const CalibdEdxTrackTopologySpline&) = delete;
  CalibdEdxTrackTopologySpline& operator=(const CalibdEdxTrackTopologySpline&) = delete;
#endif

  /// Destructor
  ~CalibdEdxTrackTopologySpline() = default;

  /// _____________  FlatObject functionality, see FlatObject class for description  ____________

  using FlatObject::getBufferAlignmentBytes;
  using FlatObject::getClassAlignmentBytes;

#if !defined(GPUCA_GPUCODE)
  void cloneFromObject(const CalibdEdxTrackTopologySpline& obj, char* newFlatBufferPtr);
  void moveBufferTo(char* newBufferPtr);
#endif

  using FlatObject::releaseInternalBuffer;

  void destroy();
  void setActualBufferAddress(char* actualFlatBufferPtr);
  void setFutureBufferAddress(char* futureFlatBufferPtr);

  /// ______________
#if !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE)
  /// sets the splines from an input file
  void setSplinesFromFile(TFile& inpf);

  /// sets maximum TanTheta and maximum snp from file
  void setRangesFromFile(TFile& inpf);

  /// set default splines: the return value of the splines will be 1 (no correction will be applied)
  void setDefaultSplines();

  /// set the the scaling factors for the splines for qTot
  /// \param factor scaling factor
  /// \param region region of the scaling factor
  void setScalingFactorqTot(const float factor, const int32_t region) { mScalingFactorsqTot[region] = factor; };

  /// set the the scaling factors for the splines for qMax
  /// \param factor scaling factor
  /// \param region region of the scaling factor
  void setScalingFactorqMax(const float factor, const int32_t region) { mScalingFactorsqMax[region] = factor; };

  /// set the maximum tanTheta for which the splines are valid
  /// \param maxTanTheta maximum tanTheta
  void setMaxTanTheta(const float maxTanTheta) { mMaxTanTheta = maxTanTheta; };

  /// set the maximum sinPhi for which the splines are valid
  /// \param maxSinPhi maximum sinPhi
  void setMaxSinPhi(const float maxSinPhi) { mMaxSinPhi = maxSinPhi; };
#endif

  /// returns the number of splines stored in the calibration object
  GPUd() uint32_t getFSplines() const { return FSplines; };

  /// returns the maximum TanTheta for which the splines are valid
  GPUd() float getMaxTanTheta() const { return mMaxTanTheta; };

  /// returns the maximum SinPhi for which the splines are valid
  GPUd() float getMaxSinPhi() const { return mMaxSinPhi; };

  /// \return returns the the scaling factors for the splines for qTot
  /// \param region region of the scaling factor
  GPUd() float getScalingFactorqTot(const int32_t region) const { return mScalingFactorsqTot[region]; };

  /// \return returns the the scaling factors for the splines for qMax
  /// \param region region of the scaling factor
  GPUd() float getScalingFactorqMax(const int32_t region) const { return mScalingFactorsqMax[region]; };

  /// \param region index of the spline (region)
  /// \param tanTheta local dip angle: z angle - dz/dx
  /// \param sinPhi track parameter sinphi
  /// \param z drift length
  /// \return returns the spline (correction) value for qMax
  GPUd() float interpolateqMax(const int32_t region, const float tanTheta, const float sinPhi, const float z) const
  {
    const float x[FDimX] = {z, tanTheta, sinPhi};
    return mScalingFactorsqMax[region] * mCalibSplinesqMax[region].interpolate(x);
  };

  /// \param region index of the spline (region)
  /// \param tanTheta local dip angle: z angle - dz/dx
  /// \param sinPhi track parameter sinphi
  /// \param z drift length
  /// \return returns the spline (correction) value for qMax
  GPUd() float interpolateqTot(const int32_t region, const float tanTheta, const float sinPhi, const float z) const
  {
    const float x[FDimX] = {z, tanTheta, sinPhi};
    return mScalingFactorsqTot[region] * mCalibSplinesqTot[region].interpolate(x);
  };

#ifdef GPUCA_HAVE_O2HEADERS
  /// \return returns the track topology correction
  /// \param region region of the TPC
  /// \param charge correction for maximum or total charge
  /// \param tanTheta local dip angle: z angle - dz/dx
  /// \param sinPhi track parameter sinphi
  /// \param z drift length
  GPUd() float getCorrection(const int32_t region, const ChargeType charge, const float tanTheta, const float sinPhi, const float z) const { return (charge == ChargeType::Max) ? interpolateqMax(region, tanTheta, sinPhi, z) : interpolateqTot(region, tanTheta, sinPhi, z); }

  /// \return returns the track topology correction
  /// \param region region of the TPC
  /// \param charge correction for maximum or total charge
  /// \param x coordinates where the correction is evaluated
  GPUd() float getCorrection(const int32_t region, const ChargeType charge, const float x[/*inpXdim*/]) const { return (charge == ChargeType::Tot) ? mCalibSplinesqTot[region].interpolate(x) : mCalibSplinesqMax[region].interpolate(x); }
#endif

  /// \param region index of the spline (region)
  /// \return returns the spline for qMax
  GPUd() SplineType& getSplineqMax(const int32_t region) { return mCalibSplinesqMax[region]; };

  /// \param region index of the spline (region)
  /// \return returns the spline for qTot
  GPUd() SplineType& getSplineqTot(const int32_t region) { return mCalibSplinesqTot[region]; };

  /// _______________  IO   ________________________
#if !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE)
  /// write a class object to the file
  int32_t writeToFile(TFile& outf, const char* name = "CalibdEdxTrackTopologySpline");

  /// set a class object from the file
  /// \param inpf input file containing the object which was stored using writeTofile
  /// \param name name of the object
  void setFromFile(TFile& inpf, const char* name);

  /// read a class object from the file
  static CalibdEdxTrackTopologySpline* readFromFile(TFile& inpf, const char* name);

  /// \return returns the name of the spline object which are read in in the setSplinesFromFile() function
  /// \param region region of the TPC
  /// \param charge correction for maximum or total charge
  static std::string getSplineName(const int32_t region, const ChargeType charge);
#endif

 private:
  constexpr static uint32_t FSplines = 10;                           ///< number of splines stored for each type
  constexpr static int32_t FDimX = 3;                                ///< dimensionality of the splines
  SplineType mCalibSplinesqMax[FSplines];                            ///< spline objects storage for the splines for qMax
  SplineType mCalibSplinesqTot[FSplines];                            ///< spline objects storage for the splines for qTot
  float mMaxTanTheta{2.f};                                           ///< max tanTheta for which the correction is stored
  float mMaxSinPhi{0.99f};                                           ///< max snp for which the correction is stored
  float mScalingFactorsqTot[FSplines]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; ///< value which is used to scale the result of the splines for qTot (can be used for normalization)
  float mScalingFactorsqMax[FSplines]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; ///< value which is used to scale the result of the splines for qMax (can be used for normalization)

  ClassDefNV(CalibdEdxTrackTopologySpline, 1);
};

#if !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE)

inline void CalibdEdxTrackTopologySpline::setSplinesFromFile(TFile& inpf)
{
  FlatObject::startConstruction();

  int32_t buffSize = 0;
  int32_t offsets1[FSplines];
  int32_t offsets2[FSplines];

  for (uint32_t ireg = 0; ireg < FSplines; ++ireg) {
    std::string splinename = getSplineName(ireg, ChargeType::Max);
    SplineType* splineTmpqMax = SplineType::readFromFile(inpf, splinename.data());
    mCalibSplinesqMax[ireg] = *splineTmpqMax;
    buffSize = alignSize(buffSize, mCalibSplinesqMax[ireg].getBufferAlignmentBytes());
    offsets1[ireg] = buffSize;
    buffSize += mCalibSplinesqMax[ireg].getFlatBufferSize();
    delete splineTmpqMax;
  }

  for (uint32_t ireg = 0; ireg < FSplines; ++ireg) {
    std::string splinename = getSplineName(ireg, ChargeType::Tot);
    SplineType* splineTmpqTot = SplineType::readFromFile(inpf, splinename.data());
    mCalibSplinesqTot[ireg] = *splineTmpqTot;
    buffSize = alignSize(buffSize, mCalibSplinesqTot[ireg].getBufferAlignmentBytes());
    offsets2[ireg] = buffSize;
    buffSize += mCalibSplinesqTot[ireg].getFlatBufferSize();
    delete splineTmpqTot;
  }

  FlatObject::finishConstruction(buffSize);

  for (uint32_t i = 0; i < FSplines; i++) {
    mCalibSplinesqMax[i].moveBufferTo(mFlatBufferPtr + offsets1[i]);
  }
  for (uint32_t i = 0; i < FSplines; i++) {
    mCalibSplinesqTot[i].moveBufferTo(mFlatBufferPtr + offsets2[i]);
  }
}

#endif

} // namespace o2::tpc

#endif
