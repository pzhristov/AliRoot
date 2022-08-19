/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliFV0.h"

#include <TSystem.h>
#include <cmath>
#include <TGeoBBox.h>
#include <TGeoCompositeShape.h>
#include <TGeoCone.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMedium.h>
#include <TGeoTube.h>
#include <TGeoVolume.h>
#include <TVirtualMC.h>
#include "AliLog.h"

ClassImp(AliFV0);

AliFV0::AliFV0(EGeoType initType) : 
 mGeometryType(initType)
{
  sDetectorName = "FV0";
  // General Geometry constants
  sEpsilon = 0.01;     ///< Used to make one spatial dimension infinitesimally larger than other
  sDzScintillator = 4; ///< Thickness of the scintillator
  sDzPlastic = 1;      ///< Thickness of the fiber plastics

  sXGlobal = 0;                         ///< Global x-position of the geometrical center of scintillators
  sYGlobal = 0;                         ///< Global y-position of the geometrical center of scintillators
                                                    // FT0 starts at z=328
  sZGlobal = 320 - sDzScintillator / 2; ///< Global z-pos of geometrical center of scintillators
  sDxHalvesSeparation = 0;              ///< Separation between the left and right side of the detector
  sDyHalvesSeparation = 0;              ///< y-position of the right detector part relative to the left part
  sDzHalvesSeparation = 0;              ///< z-position of the right detector part relative to the left part

  /// Look-up tables converting cellId to the ring and sector number
  int sCellToRing_tmp[sNumberOfCells] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
  for (int i=0; i<sNumberOfCells; i++) {
    sCellToRing[i] = sCellToRing_tmp[i];
  }
  int sCellToSector_tmp[sNumberOfCells] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
  for (int i=0; i<sNumberOfCells; i++) {
    sCellToSector[i] = sCellToSector_tmp[i];
  }

  /// Average cell ring radii.
  float sCellRingRadii_tmp[sNumberOfCellRings + 1] = {4.01, 7.3, 12.9, 21.25, 38.7, 72.115};
  for (int i=0; i<sNumberOfCellRings+1; i++) {
    sCellRingRadii[i] = sCellRingRadii_tmp[i];
  }
  char sCellTypes_tmp[sNumberOfCellSectors] = {'a', 'b', 'b', 'a'};
  for (int i=0; i<sNumberOfCellSectors; i++) {
    sCellTypes[i] = sCellTypes_tmp[i];
  }
  
  sDrSeparationScint = 0.03 + 0.04;

  /// Shift of the inner radius origin of the scintillators.
  sXShiftInnerRadiusScintillator = -0.15;
  /// Extension of the scintillator holes for the metal rods
  sDxHoleExtensionScintillator = 0.2;
  sDrHoleSmallScintillator = 0.265; ///< Radius of the small scintillator screw hole
  sDrHoleLargeScintillator = 0.415; ///< Radius of the large scintillator screw hole

  // Container constants
  sDzContainer = 30;                  ///< Depth of the metal container
  sDrContainerHole = 4.05;            ///< Radius of the beam hole in the metal container
  sXShiftContainerHole = -0.15;       ///< x-shift of the beam hole in the metal container
  sDrMaxContainerBack = 83.1;         ///< Outer radius of the container backplate
  sDzContainerBack = 1;               ///< Thickness of the container backplate
  sDrMinContainerFront = 45.7;        ///< Inner radius of the container frontplate
  sDrMaxContainerFront = 83.1;        ///< Outer radius of the container frontplate
  sDzContainerFront = 1;              ///< Thickness of the container frontplate
  sDxContainerStand = 40;             ///< Width of the container stand
  sDyContainerStand = 3;              ///< Height of the container stand at its center in x
  sDrMinContainerCone = 24.3;         ///< Inner radius at bottom of container frontplate cone
  sDzContainerCone = 16.2;            ///< Depth of the container frontplate cone
  sThicknessContainerCone = 0.6;      ///< Thickness of the container frontplate cone
  sXYThicknessContainerCone = 0.975;  ///< Radial thickness in the xy-plane of container cone
  sDrMinContainerOuterShield = 82.5;  ///< Inner radius of outer container shield
  sDrMaxContainerOuterShield = 82.65; ///< Outer radius of outer container shield
  sDrMinContainerInnerShield = 4;     ///< Inner radius of the inner container shield
  sDrMaxContainerInnerShield = 4.05;  ///< Outer radius of inner container shield
  sDxContainerCover = 0.15;           ///< Thickness of the container cover
  sDxContainerStandBottom = 38.5;     ///< Width of the bottom of the container stand
  sDyContainerStandBottom = 2;        ///< Thickness of the bottom of the container stand

  // PMT constants
  // static const int sNumberOfPMTs;                                        ///< Number of PMTs for one half of the detector.
  // static const int sNumberOfPMTsPerSector;                                ///< Number of PMTs for one sector,
  sDrPmt = 3.75;                                           ///< PMT radius
  sDzPmt = 12.199;                                         ///< PMT length
  sMPmt = 376.77;                                          ///< PMT mass
  sDensityPmt = sMPmt / (M_PI * sDrPmt * sDrPmt * sDzPmt); ///< PMT density

  // Fiber constants
  sNumberOfPMTFiberVolumes = 5; ///< Number of different fiber equivalent volumes in front of the PMTs
                                          /// The order of cells from which the fibers arrive to the PMTs in one sector.
  int sPMTFiberCellOrder_tmp[sNumberOfPMTsPerSector] = {2, 5, 4, 3, 5, 1};
  for (int i=0; i<sNumberOfPMTsPerSector; i++) {
    sPMTFiberCellOrder[i] = sPMTFiberCellOrder_tmp[i];
  }

  // Local position constants

  /// x-position of the right half of the scintillator.
  sXScintillator = sDxContainerCover;
  /// z-position of the scintillator cells.
  sZScintillator = 0;
  /// z-position of the plastic cells.
  sZPlastic = sZScintillator + sDzScintillator / 2 + sDzPlastic / 2;
  /// z-position of the container backplate.
  sZContainerBack = sZScintillator - sDzScintillator / 2 - sDzContainerBack / 2;
  /// z-position of the container frontplate.
  sZContainerFront = sZContainerBack - sDzContainerBack / 2 + sDzContainer - sDzContainerFront / 2;
  /// z-position of the center of the container.
  sZContainerMid = (sZContainerBack + sZContainerFront) / 2;
  /// z-position of the fiber volumes.
  sZFiber = (sZPlastic + sZContainerFront) / 2;
  /// z-position of the container frontplate cone.
  sZCone = sZContainerFront + sDzContainerFront / 2 - sDzContainerCone / 2;
  /// x shift of all screw holes.
  sXShiftScrews = sXScintillator;
  /// x-positions of the PMTs in the right half of the detector.
  float sXPmt_tmp[sNumberOfPMTs] = {8.023, 16.612, 24.987, 33.042, 40.671, 47.778, 59.646, 64.73, 68.982, 72.348, 74.783, 76.257, 76.330, 74.931, 72.569, 69.273, 65.088, 60.065, 48.3, 41.238, 33.645, 25.62, 17.265, 8.688};
  for (int i=0; i<sNumberOfPMTs; i++) {
    sXPmt[i] = sXPmt_tmp[i];
  }
  /// y-positions of the PMTs in one half of the detector.
  float sYPmt_tmp[sNumberOfPMTs] = {76.33, 74.931, 72.569, 69.273, 65.088, 60.065, 48.3, 41.238, 33.645, 25.62, 17.265, 8.688, -8.023, -16.612, -24.987, -33.042, -40.671, -47.778, -59.646, -64.73, -68.982, -72.348, -74.783, -76.257};
  for (int i=0; i<sNumberOfPMTs; i++) {
    sYPmt[i] = sYPmt_tmp[i];
  }
  /// z-position of the PMTs.
  sZPmt = sZContainerBack + sDzContainerBack / 2 + sDzPmt / 2;

  // Screw and rod dimensions

  /// Number of the different screw types.
  // static const int sNumberOfScrewTypes;
  /// Radii of the thinner part of the screw types.
  float sDrMinScrewTypes_tmp[sNumberOfScrewTypes] = {0.25, 0.25, 0.4, 0.4, 0.4, 0.4};
  for (int i=0; i<sNumberOfScrewTypes; i++) {
    sDrMinScrewTypes[i] = sDrMinScrewTypes_tmp[i];
  }
  /// Radii of the thicker part of the screw types.
  float sDrMaxScrewTypes_tmp[sNumberOfScrewTypes] = {0, 0.5, 0.6, 0.6, 0.6, 0};
  for (int i=0; i<sNumberOfScrewTypes; i++) {
    sDrMaxScrewTypes[i] = sDrMaxScrewTypes_tmp[i];
  }
  /// Length of the thinner part of the screw types.
  float sDzMaxScrewTypes_tmp[sNumberOfScrewTypes] = {6.02, 13.09, 13.1, 23.1, 28.3, 5};
  for (int i=0; i<sNumberOfScrewTypes; i++) {
    sDzMaxScrewTypes[i] = sDzMaxScrewTypes_tmp[i];
  }
  /// Length of the thicker part of the screw types.
  float sDzMinScrewTypes_tmp[sNumberOfScrewTypes] = {0, 6.78, 6.58, 15.98, 21.48, 0};
  for (int i=0; i<sNumberOfScrewTypes; i++) {
    sDzMinScrewTypes[i] = sDzMinScrewTypes_tmp[i];
  }
  /// z shift of the screws. 0 means they are aligned with the scintillator.
  sZShiftScrew = 0;

  /// Number of the different rod types.
  // static const int sNumberOfRodTypes;
  /// Width of the thinner part of the rod types.
  float sDxMinRodTypes_tmp[sNumberOfRodTypes] = {0.366, 0.344, 0.344, 0.344};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDxMinRodTypes[i] = sDxMinRodTypes_tmp[i];
  }
  /// Width of the thicker part of the rod types.
  float sDxMaxRodTypes_tmp[sNumberOfRodTypes] = {0.536, 0.566, 0.566, 0.716};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDxMaxRodTypes[i] = sDxMaxRodTypes_tmp[i];
  }
  /// Height of the thinner part of the rod types.
  float sDyMinRodTypes_tmp[sNumberOfRodTypes] = {0.5, 0.8, 0.8, 0.8};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDyMinRodTypes[i] = sDyMinRodTypes_tmp[i];
  }
  /// Height of the thicker part of the rod types.
  float sDyMaxRodTypes_tmp[sNumberOfRodTypes] = {0.9, 1.2, 1.2, 1.2};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDyMaxRodTypes[i] = sDyMaxRodTypes_tmp[i];
  }
  /// Length of the thinner part of the rod types.
  float sDzMaxRodTypes[sNumberOfRodTypes] = {12.5, 12.5, 22.5, 27.7};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDzMaxRodTypes[i] = sDzMaxScrewTypes_tmp[i];
  }
  /// Length of the thicker part of the rod types.
  float sDzMinRodTypes[sNumberOfRodTypes] = {7.45, 7.45, 17.45, 22.65};
  for (int i=0; i<sNumberOfRodTypes; i++) {
    sDzMinRodTypes[i] = sDzMinScrewTypes_tmp[i];
  }
  /// z shift of the rods. 0 means they are aligned with the scintillators.
  sZShiftRod = -0.05;

  // Strings for volume names, etc.
  sScintillatorName = "SCINT";
  sPlasticName = "PLAST";
  sSectorName = "SECTOR";
  sCellName = "CELL";
  sScintillatorSectorName = sScintillatorName + sSectorName;
  sScintillatorCellName = sScintillatorName + sCellName;
  sPlasticSectorName = sPlasticName + sSectorName;
  sPlasticCellName = sPlasticName + sCellName;
  sPmtName = "PMT";
  sFiberName = "FIBER";
  sScrewName = "SCREW";
  sScrewHolesCSName = "FV0SCREWHOLES";
  sRodName = "ROD";
  sRodHolesCSName = "FV0RODHOLES";
  sContainerName = "CONTAINER";
  
  initializeCellCenters();
  initializeReadoutCenters();
}

AliFV0::AliFV0(const AliFV0 &geometry) : mGeometryType(geometry.mGeometryType), mLeftTransformation(0x0), mRightTransformation(0x0)
{
  //this->mEnabledComponents = geometry.mEnabledComponents;
  /*for (EGeoComponent i=eScintillator; i<eNComponents; i++) {
    mEnabledComponents[i] = geometry.getEnabledComponent(i);        
  }*/
        
}

AliFV0::~AliFV0()
{
}

int AliFV0::getCurrentCellId(const TVirtualMC *fMC) const
{
  int detectorHalfID = -1;
  int sectorID = -1;
  int ringID = -1;

  fMC->CurrentVolOffID(2, detectorHalfID);
  fMC->CurrentVolOffID(1, sectorID);
  fMC->CurrentVolOffID(0, ringID);

  sectorID += detectorHalfID * sNumberOfCellSectors;

  AliDebug(1, Form("FV0 AliFV0::getCurrentCellId(): \n Half id:     %d\n Half name:   %s\n  Sector id:   %d\n  Sector name: %s\n  Ring id:     %d\n  Ring name:   %s\n   Cell id :    %d\n",
                   detectorHalfID, fMC->CurrentVolOffName(2), sectorID, fMC->CurrentVolOffName(1), ringID, fMC->CurrentVolOffName(0), sectorID + 8 * ringID));

  return sectorID + 8 * ringID;
}

int AliFV0::getCurrentRingId(const TVirtualMC *fMC) const
{
  int ringID = -1;
  fMC->CurrentVolOffID(0, ringID);

  return ringID + 1;
}

void AliFV0::enableComponent(const EGeoComponent component, const bool enable)
{
  mEnabledComponents[component] = enable;
}

void AliFV0::ConstructGeometry()
{
  if (mGeometryType != eUninitialized)
  {
    initializeGeometry();
  }

  TGeoVolume *vALIC = gGeoManager->GetVolume("ALIC");
  if (!vALIC)
  {
    AliFatal("FV0: Could not find the top volume");
  }

  // Top volume of FV0 detector
  TGeoVolumeAssembly *vFV0 = new TGeoVolumeAssembly(sDetectorName.Data());
  AliInfo(Form("FV0: Building geometry. FV0 volume name is '%s'", vFV0->GetName()));

  TGeoVolumeAssembly *vFV0Right = new TGeoVolumeAssembly(Form("%sRIGHT", sDetectorName.Data()));
  TGeoVolumeAssembly *vFV0Left = new TGeoVolumeAssembly(Form("%sLEFT", sDetectorName.Data()));

  vFV0->AddNode(vFV0Right, 0, mRightTransformation);
  vFV0->AddNode(vFV0Left, 1, mLeftTransformation);

  assembleSensVols(vFV0Right, vFV0Left);
  assembleNonSensVols(vFV0Right, vFV0Left);

  vALIC->AddNode(vFV0, 1, new TGeoTranslation(sXGlobal, sYGlobal, sZGlobal));
}

void AliFV0::getGlobalPosition(float &x, float &y, float &z)
{
  x = sXGlobal;
  y = sYGlobal;
  z = sZGlobal;
}

TVector3 &AliFV0::getCellCenter(UInt_t cellId)
{
  //return mCellCenter.at(cellId);
  return mCellCenter[cellId];
}

TVector3 &AliFV0::getReadoutCenter(UInt_t cellId)
{
  //return mReadoutCenter.at(cellId);
  return mReadoutCenter[cellId];
}

bool AliFV0::isRing5(UInt_t cellId)
{
  return cellId >= (sNumberOfCellRings - 1) * sNumberOfCellSectors * 2;
}

void AliFV0::initializeGeometry()
{
  initializeMaps();
  initializeVectors();
  initializeTransformations();
  initializeSensVols();
  initializeNonSensVols();
}

void AliFV0::initializeMaps()
{
  const bool isFull = mGeometryType == eFull;
  const bool isRough = isFull || mGeometryType == eRough;
  const bool hasScint = isRough || mGeometryType == eOnlySensitive;
  mEnabledComponents[eScintillator] = hasScint;
  mEnabledComponents[ePlastics] = isRough;
  mEnabledComponents[ePmts] = isFull;
  mEnabledComponents[eFibers] = isFull;
  mEnabledComponents[eScrews] = isFull;
  mEnabledComponents[eRods] = isFull;
  mEnabledComponents[eContainer] = isRough;
}

void AliFV0::initializeVectors()
{
  initializeCellRingRadii();
  initializeSectorTransformations();
  initializeFiberVolumeRadii();
  initializeFiberMedium();
  initializeScrewAndRodRadii();
  initializeScrewTypeMedium();
  initializeRodTypeMedium();
  initializeScrewAndRodPositionsAndDimensions();
}

void AliFV0::initializeCellRingRadii()
{
  // Index of mRAvgRing is NOT linked directly to any ring number
  mRAvgRing.assign(sCellRingRadii, sCellRingRadii + sNumberOfCellRings + 1);

  // Set real scintillator radii (reduced by paint thickness and separation gap)
  for (int i = 0; i < mRAvgRing.size() - 1; ++i)
  {
    mRMinScintillator.push_back(mRAvgRing[i] + sDrSeparationScint);
    mRMaxScintillator.push_back(mRAvgRing[i + 1] - sDrSeparationScint);
  }
  // Now indices of mRMinScint and mRMaxScint correspond to the same ring
}

void AliFV0::initializeSectorTransformations()
{
  for (int iSector = 0; iSector < sNumberOfCellSectors; ++iSector)
  {
    // iSector = 0 corresponds to the first sector clockwise from the y-axis
    // iSector = 1 corresponds to the next sector in clockwise direction and so on

    TGeoRotation *trans = createAndRegisterRot(Form("%s%s%dTRANS", sDetectorName.Data(), sSectorName.Data(), iSector));

    if (iSector == 2 || iSector == 3)
    {
      // "a" and "b" mirrors.
      // The reference to "a" and "b" can be understood with the CAD drawings of the detector.
      trans->ReflectY(true);
    }

    mSectorTrans.push_back(trans);
  }
}

void AliFV0::initializeFiberVolumeRadii()
{
  for (int i = 0; i < sNumberOfCellRings; i++)
  {
    mRMinFiber.push_back(sCellRingRadii[i] + sEpsilon / 2);
    mRMaxFiber.push_back(sCellRingRadii[i + 1] - sEpsilon / 2);
  }
}

void AliFV0::initializeFiberMedium()
{
  TGeoMedium *medium;
  TString mediumName;

  // one fiber volume per ring
  for (int i = 0; i < mRMinFiber.size(); i++)
  {
    mediumName = Form("FIT_FiberRing%i$", i + 1);
    medium = gGeoManager->GetMedium(mediumName);
    if (!medium)
    {
      AliWarning(Form("FV0 geometry: Fiber medium for ring no. %i (%s) not found!", i + 1, mediumName.Data()));
    }
    mMediumFiberRings.push_back(medium);
  }

  // five fiber volumes in front of the PMTs, one from each scintillator cell (two volumes from cell 5 but they are identical)
  for (int i = 0; i < sNumberOfPMTFiberVolumes; i++)
  {
    mediumName = Form("FIT_FiberPMT%i$", i + 1);
    medium = gGeoManager->GetMedium(mediumName);
    if (!medium)
    {
      AliWarning(Form("FV0 geometry: PMT fiber medium from cell no. %i (%s) not found!", i + 1, mediumName.Data()));
    }
    mMediumFiberPMTs.push_back(medium);
  }
}

void AliFV0::initializeScrewAndRodRadii()
{
  mRScrewAndRod.push_back(mRAvgRing[1]);
  mRScrewAndRod.push_back(mRAvgRing[2]);
  mRScrewAndRod.push_back(mRAvgRing[3]);
  mRScrewAndRod.push_back(mRAvgRing[4]);
  mRScrewAndRod.push_back((mRAvgRing[4] + mRAvgRing[5]) / 2);
  mRScrewAndRod.push_back(mRAvgRing[5]);
}

void AliFV0::initializeScrewTypeMedium()
{
  // There are no further checks if the medium is actually found
  TGeoMedium *medium = gGeoManager->GetMedium("FIT_Titanium$");
  for (int i = 0; i < sNumberOfScrewTypes; ++i)
  {
    mMediumScrewTypes.push_back(medium);
  }
}

void AliFV0::initializeRodTypeMedium()
{
  // There are no further checks if the medium is actually found
  TGeoMedium *medium = gGeoManager->GetMedium("FIT_Aluminium$");
  for (int i = 0; i < sNumberOfRodTypes; ++i)
  {
    mMediumRodTypes.push_back(medium);
  }
}

void AliFV0::addScrewProperties(const int screwTypeID, const int iRing, const float phi)
{
  float r = mRScrewAndRod[iRing];
  mScrewTypeIDs.push_back(screwTypeID);
  mScrewPos.push_back(std::vector<float>{cosf(phi * M_PI / 180) * r,
                                         sinf(phi * M_PI / 180) * r,
                                         sZScintillator - sDzScintillator / 2 + sZShiftScrew + sDzMaxScrewTypes[screwTypeID] / 2});
  mDrMinScrews.push_back(sDrMinScrewTypes[screwTypeID]);
  mDrMaxScrews.push_back(sDrMaxScrewTypes[screwTypeID]);
  mDzMaxScrews.push_back(sDzMaxScrewTypes[screwTypeID]);
  mDzMinScrews.push_back(sDzMinScrewTypes[screwTypeID]);
}

void AliFV0::addRodProperties(const int rodTypeID, const int iRing)
{
  mRodTypeIDs.push_back(rodTypeID);
  mRodPos.push_back(std::vector<float>{sDxMinRodTypes[rodTypeID] / 2,
                                       mRScrewAndRod[iRing],
                                       sZScintillator - sDzScintillator / 2 + sZShiftRod + sDzMaxRodTypes[rodTypeID] / 2});
  mDxMinRods.push_back(sDxMinRodTypes[rodTypeID]);
  mDzMaxRods.push_back(sDxMaxRodTypes[rodTypeID]);
  mDyMinRods.push_back(sDyMinRodTypes[rodTypeID]);
  mDyMaxRods.push_back(sDyMaxRodTypes[rodTypeID]);
  mDzMaxRods.push_back(sDzMaxRodTypes[rodTypeID]);
  mDzMinRods.push_back(sDzMinRodTypes[rodTypeID]);

  mRodTypeIDs.push_back(rodTypeID);
  mRodPos.push_back(std::vector<float>{sDxMinRodTypes[rodTypeID] / 2,
                                       -mRScrewAndRod[iRing],
                                       sZScintillator - sDzScintillator / 2 + sZShiftRod + sDzMaxRodTypes[rodTypeID] / 2});
  mDxMinRods.push_back(sDxMinRodTypes[rodTypeID]);
  mDzMaxRods.push_back(sDxMaxRodTypes[rodTypeID]);
  mDyMinRods.push_back(sDyMinRodTypes[rodTypeID]);
  mDyMaxRods.push_back(sDyMaxRodTypes[rodTypeID]);
  mDzMaxRods.push_back(sDzMaxRodTypes[rodTypeID]);
  mDzMinRods.push_back(sDzMinRodTypes[rodTypeID]);
}

void AliFV0::initializeScrewAndRodPositionsAndDimensions()
{
  for (int iRing = 0; iRing < mRScrewAndRod.size(); ++iRing)
  {
    switch (iRing)
    {
    case 0:
      addRodProperties(0, iRing);
      for (float phi = 45; phi >= -45; phi -= 45)
      {
        addScrewProperties(0, iRing, phi);
      }
      break;
    case 1:
      addRodProperties(0, iRing);
      for (float phi = 45; phi >= -45; phi -= 45)
      {
        addScrewProperties(1, iRing, phi);
      }
      break;
    case 2:
      addRodProperties(1, iRing);
      for (float phi = 67.5; phi >= -67.5; phi -= 22.5)
      {
        addScrewProperties(2, iRing, phi);
      }
      break;
    case 3:
      addRodProperties(2, iRing);
      for (float phi = 67.5; phi >= -67.5; phi -= 22.5)
      {
        addScrewProperties(3, iRing, phi);
      }
      break;
    case 4:
      addRodProperties(3, iRing);
      for (float phi = 45; phi >= -45; phi -= 45)
      {
        addScrewProperties(4, iRing, phi);
      }
      break;
    case 5:
      addRodProperties(3, iRing);
      for (float phi = 67.5; phi >= -67.5; phi -= 22.5)
      {
        addScrewProperties(5, iRing, phi);
      }
      break;
    default:
      break;
    }
  }
}

void AliFV0::initializeTransformations()
{
  TGeoTranslation leftTranslation(-sDxHalvesSeparation / 2, 0, 0);
  TGeoRotation leftRotation;
  leftRotation.ReflectX(true);
  TGeoHMatrix leftTotalTransformation = leftTranslation * leftRotation;

  mLeftTransformation = new TGeoHMatrix(leftTotalTransformation);
  mRightTransformation = new TGeoTranslation(sDxHalvesSeparation / 2, sDyHalvesSeparation, sDzHalvesSeparation);
}

void AliFV0::initializeSensVols()
{
  initializeScintCells();
}

void AliFV0::initializeNonSensVols()
{
  initializeScrewHoles();
  initializeRodHoles();
  initializePlasticCells();
  initializePmts();
  initializeFibers();
  initializeScrews();
  initializeRods();
  initializeMetalContainer();
}

void AliFV0::initializeScrewHoles()
{
  TString boolFormula = "";

  for (int i = 0; i < mScrewPos.size(); ++i)
  {
    TString holeShapeName = sDetectorName + sScrewName + "HOLE" + Form("%d",i);
    TString holeTransName = sDetectorName + sScrewName + "HOLETRANS" + Form("%d",i);

    createScrewShape(holeShapeName, mScrewTypeIDs[i], sEpsilon, sEpsilon);
    createAndRegisterTrans(holeTransName, mScrewPos[i][0] + sXShiftScrews, mScrewPos[i][1], mScrewPos[i][2]);

    boolFormula += ((i != 0) ? "+" : "") + holeShapeName + ":" + holeTransName;
  }

  new TGeoCompositeShape(sScrewHolesCSName.Data(), boolFormula.Data());
}

void AliFV0::initializeRodHoles()
{
  TString boolFormula = "";

  for (int i = 0; i < mRodPos.size(); ++i)
  {
    TString holeShapeName = sDetectorName + sRodName + "HOLE" + Form("%d",i);
    TString holeTransName = sDetectorName + sRodName + "HOLETRANS" + Form("%d",i);

    createRodShape(holeShapeName, mRodTypeIDs[i], sEpsilon, sEpsilon);
    createAndRegisterTrans(holeTransName, mRodPos[i][0] + sXShiftScrews, mRodPos[i][1], mRodPos[i][2]);

    boolFormula += ((i != 0) ? "+" : "") + holeShapeName + ":" + holeTransName;
  }

  new TGeoCompositeShape(sRodHolesCSName.Data(), boolFormula.Data());
}

void AliFV0::initializeCells(TString cellType, const float zThickness, const TGeoMedium *medium,
                             const bool isSensitive)
{
  // Creating the two types of cells, "a" and "b", for each ring.
  // All sectors can be assembled with these cells.
  //
  // The reference to "a" and "b" can be understood with the CAD drawings of the detector.

  const float dxHoleCut = sDxHoleExtensionScintillator; // width of extension of hole 1, 2 and 7 in the "a" cell
  const float xHole = sDrSeparationScint + dxHoleCut;   // x-placement of holes 1, 2 and 7 in the "a" cell

  // Sector separation gap shape
  TString secSepShapeName = "FV0_" + cellType + "SectorSeparation";
  new TGeoBBox(secSepShapeName.Data(), mRMaxScintillator.back() + sEpsilon, sDrSeparationScint, zThickness / 2);

  // Sector separation gap rotations
  TString secSepRot45Name = "FV0_" + cellType + "SecSepRot45";
  TString secSepRot90Name = "FV0_" + cellType + "SecSepRot90";

  createAndRegisterRot(secSepRot45Name, 45, 0, 0);
  createAndRegisterRot(secSepRot90Name, 90, 0, 0);

  // Hole shapes
  TString holeSmallName = "FV0_" + cellType + "HoleSmall";
  TString holeLargeName = "FV0_" + cellType + "HoleLarge";
  TString holeSmallCutName = "FV0_" + cellType + "HoleSmallCut";
  TString holeLargeCutName = "FV0_" + cellType + "HoleLargeCut";

  new TGeoTube(holeSmallName.Data(), 0, sDrHoleSmallScintillator, zThickness / 2);
  new TGeoTube(holeLargeName.Data(), 0, sDrHoleLargeScintillator, zThickness / 2);
  new TGeoBBox(holeSmallCutName.Data(), dxHoleCut, sDrHoleSmallScintillator, zThickness / 2);
  new TGeoBBox(holeLargeCutName.Data(), dxHoleCut, sDrHoleLargeScintillator, zThickness / 2);

  for (int ir = 0; ir < sNumberOfCellRings; ++ir)
  {
    // Radii without separation
    const float rMin = mRAvgRing[ir];
    const float rMax = mRAvgRing[ir + 1];
    const float rMid = rMin + (rMax - rMin) / 2;

    // "a"-type cell
    //
    // Initial placement:
    //
    // y
    // ^
    // |  1******
    // |  ************5
    // |  7*****************
    // |  *********************3
    // |  *******************
    // |  2**************8
    // |      6********
    // |        **4
    // |
    // |
    // |  O
    // ------------------------> x
    //
    // * = cell volume
    // numbers = hole numbers (as numbered in the code below)
    // O = beam pipe

    TString aCellName = createVolumeName(cellType + sCellName + "a", ir);

    // Base shape
    TString aCellShapeName = aCellName + "Shape";

    // The cells in the innermost ring have a slightly shifted inner radius origin.
    if (ir == 0)
    {
      // The innermost "a"-type cell
      TString a1CellShapeFullName = aCellShapeName + "Full";
      TString a1CellShapeHoleCutName = aCellShapeName + "HoleCut";
      TString a1CellShapeHoleCutTransName = a1CellShapeHoleCutName + "Trans";

      new TGeoTubeSeg(a1CellShapeFullName.Data(), 0, mRMaxScintillator[ir], zThickness / 2 - sEpsilon, 45, 90);
      new TGeoTube(a1CellShapeHoleCutName.Data(), 0, mRMinScintillator[ir], zThickness);

      createAndRegisterTrans(a1CellShapeHoleCutTransName, sXShiftInnerRadiusScintillator, 0, 0);

      TString a1BoolFormula = a1CellShapeFullName + "-" + a1CellShapeHoleCutName + ":" + a1CellShapeHoleCutTransName;
      new TGeoCompositeShape(aCellShapeName.Data(), a1BoolFormula.Data());
    }
    else
    {
      // The rest of the "a"-type cells
      new TGeoTubeSeg(aCellShapeName.Data(), mRMinScintillator[ir], mRMaxScintillator[ir], zThickness / 2, 45, 90);
    }

    // Translations for screw holes (inner = rmin, half-length = rmid, outer = rmax)
    //
    // 1 = outer left
    // 2 = inner left
    // 3 = outer right
    // 4 = inner right
    // 5 = outer middle
    // 6 = inner middle
    // 7 = half-length left
    // 8 = half-length right
    //
    // holes 1, 2 and 7 are slightly shifted along the rim of the cell

    TString aHole1TransName = aCellName + "Hole1Trans";
    TString aHole2TransName = aCellName + "Hole2Trans";
    TString aHole3TransName = aCellName + "Hole3Trans";
    TString aHole4TransName = aCellName + "Hole4Trans";
    TString aHole5TransName = aCellName + "Hole5Trans";
    TString aHole6TransName = aCellName + "Hole6Trans";
    TString aHole7TransName = aCellName + "Hole7Trans";
    TString aHole8TransName = aCellName + "Hole8Trans";
    TString aHole1CutTransName = aCellName + "Hole1CutTrans";
    TString aHole2CutTransName = aCellName + "Hole2CutTrans";
    TString aHole7CutTransName = aCellName + "Hole7CutTrans";

    createAndRegisterTrans(aHole1TransName, xHole, cos(asin(xHole / rMax)) * rMax, 0);
    createAndRegisterTrans(aHole2TransName, xHole, cos(asin(xHole / rMin)) * rMin, 0);
    createAndRegisterTrans(aHole3TransName, sin(45 * M_PI / 180) * rMax, cos(45 * M_PI / 180) * rMax, 0);
    createAndRegisterTrans(aHole4TransName, sin(45 * M_PI / 180) * rMin, cos(45 * M_PI / 180) * rMin, 0);
    createAndRegisterTrans(aHole5TransName, sin(22.5 * M_PI / 180) * rMax, cos(22.5 * M_PI / 180) * rMax, 0);
    createAndRegisterTrans(aHole6TransName, sin(22.5 * M_PI / 180) * rMin, cos(22.5 * M_PI / 180) * rMin, 0);
    createAndRegisterTrans(aHole7TransName, xHole, cos(asin(xHole / rMid)) * rMid, 0);
    createAndRegisterTrans(aHole8TransName, sin(45 * M_PI / 180) * rMid, cos(45 * M_PI / 180) * rMid, 0);
    createAndRegisterTrans(aHole1CutTransName, 0, cos(asin(xHole / rMax)) * rMax, 0);
    createAndRegisterTrans(aHole2CutTransName, 0, cos(asin(xHole / rMin)) * rMin, 0);
    createAndRegisterTrans(aHole7CutTransName, 0, cos(asin(xHole / rMid)) * rMid, 0);

    // Composite shape
    TString aBoolFormula = aCellShapeName;

    // sector separation
    aBoolFormula += "-" + secSepShapeName + ":" + secSepRot45Name;
    aBoolFormula += "-" + secSepShapeName + ":" + secSepRot90Name;

    // outer holes
    aBoolFormula += "-" + ((ir < 2) ? holeSmallName : holeLargeName) + ":" + aHole1TransName;
    aBoolFormula += "-" + ((ir < 2) ? holeSmallCutName : holeLargeCutName) + ":" + aHole1CutTransName;
    aBoolFormula += "-" + ((ir < 2) ? holeSmallName : holeLargeName) + ":" + aHole3TransName;

    // inner holes
    if (ir > 0)
    {
      TString screwHoleName = (ir < 3) ? holeSmallName : holeLargeName;
      TString screwHoleCutName = (ir < 3) ? holeSmallCutName : holeLargeCutName;

      aBoolFormula += "-" + screwHoleName + ":" + aHole2TransName;
      aBoolFormula += "-" + screwHoleCutName + ":" + aHole2CutTransName;
      aBoolFormula += "-" + screwHoleName + ":" + aHole4TransName;
    }

    // outer middle hole
    if (ir > 1)
    {
      aBoolFormula += "-" + holeLargeName + ":" + aHole5TransName;
    }

    // inner middle hole
    if (ir > 2)
    {
      aBoolFormula += "-" + holeLargeName + ":" + aHole6TransName;
    }

    // half-length holes
    if (ir == 4)
    {
      aBoolFormula += "-" + holeLargeName + ":" + aHole7TransName;
      aBoolFormula += "-" + holeLargeCutName + ":" + aHole7CutTransName;
      aBoolFormula += "-" + holeLargeName + ":" + aHole8TransName;
    }

    TString aCellCSName = aCellName + "CS";
    const TGeoCompositeShape *aCellCs = new TGeoCompositeShape(aCellCSName.Data(), aBoolFormula.Data());

    // Cell volume
    const TGeoVolume *aCell = new TGeoVolume(aCellName.Data(), aCellCs, medium);

    // "b"-type cell
    //
    // Initial placement:
    //
    // y
    // ^
    // |                    1
    // |                 *****
    // |               *********
    // |            7*************
    // |          ****************5
    // |        2*******************
    // |           6*****************
    // |            *****************
    // |  O          4*******8******3
    // |
    // ------------------------------> x
    //
    // * = cell volume
    // numbers = number of holes (as numbered in the code below)
    // O = beam pipe

    TString bCellName = createVolumeName(cellType + sCellName + "b", ir);

    // Base shape
    TString bCellShapeName = bCellName + "Shape";

    // The cells in the innermost ring are slightly different than the rest
    if (ir == 0)
    {
      // The innermost "b"-type cell
      TString b1CellShapeFullName = bCellShapeName + "Full";
      TString b1CellShapeHoleCutName = bCellShapeName + "Cut";
      TString b1CellShapeHoleCutTransName = b1CellShapeHoleCutName + "Trans";

      new TGeoTubeSeg(b1CellShapeFullName.Data(), 0, mRMaxScintillator[ir], zThickness / 2 - sEpsilon, 0, 45);
      new TGeoTube(b1CellShapeHoleCutName.Data(), 0, mRMinScintillator[ir], zThickness);

      createAndRegisterTrans(b1CellShapeHoleCutTransName, sXShiftInnerRadiusScintillator, 0, 0);

      TString b1BoolFormula = b1CellShapeFullName + "-" + b1CellShapeHoleCutName + ":" + b1CellShapeHoleCutTransName;
      new TGeoCompositeShape(bCellShapeName.Data(), b1BoolFormula.Data());
    }
    else
    {
      // The rest of the "b"-type cells
      new TGeoTubeSeg(bCellShapeName.Data(), mRMinScintillator[ir], mRMaxScintillator[ir], zThickness / 2, 0, 45);
    }

    // Translations for holes
    //
    // 1 = outer left
    // 2 = inner left
    // 3 = outer right
    // 4 = inner right
    // 5 = outer middle
    // 6 = inner middle
    // 7 = half-lenght left
    // 8 = half-length right

    TString bHole1TransName = bCellName + "Hole1Trans";
    TString bHole2TransName = bCellName + "Hole2Trans";
    TString bHole3TransName = bCellName + "Hole3Trans";
    TString bHole4TransName = bCellName + "Hole4Trans";
    TString bHole5TransName = bCellName + "Hole5Trans";
    TString bHole6TransName = bCellName + "Hole6Trans";
    TString bHole7TransName = bCellName + "Hole7Trans";
    TString bHole8TransName = bCellName + "Hole8Trans";

    createAndRegisterTrans(bHole1TransName, sin(45 * M_PI / 180) * rMax, cos(45 * M_PI / 180) * rMax, 0);
    createAndRegisterTrans(bHole2TransName, sin(45 * M_PI / 180) * rMin, cos(45 * M_PI / 180) * rMin, 0);
    createAndRegisterTrans(bHole3TransName, rMax, 0, 0);
    createAndRegisterTrans(bHole4TransName, rMin, 0, 0);
    createAndRegisterTrans(bHole5TransName, cos(22.5 * M_PI / 180) * rMax, sin(22.5 * M_PI / 180) * rMax, 0);
    createAndRegisterTrans(bHole6TransName, cos(22.5 * M_PI / 180) * rMin, sin(22.5 * M_PI / 180) * rMin, 0);
    createAndRegisterTrans(bHole7TransName, sin(45 * M_PI / 180) * rMid, cos(45 * M_PI / 180) * rMid, 0);
    createAndRegisterTrans(bHole8TransName, rMid, 0, 0);

    // Composite shape
    TString bBoolFormula = bCellShapeName;

    // sector separation
    bBoolFormula += "-" + secSepShapeName;
    bBoolFormula += "-" + secSepShapeName + ":" + secSepRot45Name;

    // outer holes
    bBoolFormula += "-" + ((ir < 2) ? holeSmallName : holeLargeName) + ":" + bHole1TransName;
    bBoolFormula += "-" + ((ir < 2) ? holeSmallName : holeLargeName) + ":" + bHole3TransName;

    // inner holes
    if (ir > 0)
    {
      TString holeName = (ir < 3) ? holeSmallName : holeLargeName;

      bBoolFormula += "-" + holeName + ":" + bHole2TransName;
      bBoolFormula += "-" + holeName + ":" + bHole4TransName;
    }

    // outer middle hole
    if (ir > 1)
    {
      bBoolFormula += "-" + holeLargeName + ":" + bHole5TransName;
    }

    // inner middle hole
    if (ir > 2)
    {
      bBoolFormula += "-" + holeLargeName + ":" + bHole6TransName;
    }

    // half-lenght holes
    if (ir == 4)
    {
      bBoolFormula += "-" + holeLargeName + ":" + bHole7TransName;
      bBoolFormula += "-" + holeLargeName + ":" + bHole8TransName;
    }

    TString bCellCSName = bCellName + "CS";
    const TGeoCompositeShape *bCellCs = new TGeoCompositeShape(bCellCSName.Data(), bBoolFormula.Data());

    // Cell volume
    const TGeoVolume *bCell = new TGeoVolume(bCellName.Data(), bCellCs, medium);

    if (isSensitive)
    {
      ///////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //  Posible problem? We are adding two names for each cell ring (ir), while in AliFITv9::init() one expects just one per ring ???
      mSensitiveVolumeNames.push_back(aCell->GetName());
      mSensitiveVolumeNames.push_back(bCell->GetName());
    }
  }
}

void AliFV0::initializeScintCells()
{
  const TGeoMedium *medium = gGeoManager->GetMedium("FIT_Scintillator$");
  initializeCells(sScintillatorName, sDzScintillator, medium, true);
}

void AliFV0::initializePlasticCells()
{
  const TGeoMedium *medium = gGeoManager->GetMedium("FIT_Plastic$");
  initializeCells(sPlasticName, sDzPlastic, medium, false);
}

void AliFV0::initializePmts()
{
  const TGeoMedium *medium = gGeoManager->GetMedium("FIT_PMT$");
  new TGeoVolume(createVolumeName(sPmtName).Data(), new TGeoTube(createVolumeName(sPmtName + "Shape").Data(), 0, sDrPmt, sDzPmt / 2), medium);
}

void AliFV0::initializeFibers()
{
  // depth of the fiber volumes
  const float dzFibers = sDzContainer - sDzContainerBack - sDzContainerFront - sDzScintillator - sDzPlastic - 2 * sEpsilon;

  TString fiberName = sDetectorName + "Fibers";

  TString fiberSepCutName = fiberName + "SepCut";
  TString fiberConeCutName = fiberName + "ConeCut";
  TString fiberHoleCutName = fiberName + "HoleCut";

  TString fiberTransName = fiberName + "Trans";
  TString fiberConeCutTransName = fiberConeCutName + "Trans";
  TString fiberHoleCutTransName = fiberHoleCutName + "Trans";

  new TGeoBBox(fiberSepCutName.Data(), sDrSeparationScint, mRMaxFiber.back() + sEpsilon, dzFibers / 2 + sEpsilon);
  new TGeoConeSeg(fiberConeCutName.Data(), sDzContainerCone / 2 + sEpsilon, 0,
                  sDrMinContainerCone + sXYThicknessContainerCone + sEpsilon, 0, sDrMinContainerFront + sEpsilon, -90, 90);
  new TGeoTube(fiberHoleCutName.Data(), 0, mRMinScintillator.front(), dzFibers / 2 + sEpsilon);

  createAndRegisterTrans(fiberTransName, sXScintillator, 0, sZFiber);
  createAndRegisterTrans(fiberConeCutTransName, sXScintillator, 0, sZCone);
  createAndRegisterTrans(fiberHoleCutTransName, sXScintillator + sXShiftInnerRadiusScintillator, 0, sZFiber);

  for (int i = 0; i < mRMinFiber.size(); ++i)
  {
    TString fiberShapeName = fiberName + Form("%d",i+1);
    new TGeoTubeSeg(fiberShapeName.Data(), mRMinFiber[i], mRMaxFiber[i] - sEpsilon, dzFibers / 2, -90, 90);

    // Composite shape
    TString boolFormula = "";
    boolFormula += fiberShapeName + ":" + fiberTransName;
    boolFormula += "-" + fiberSepCutName + ":" + fiberTransName;
    boolFormula += "-" + fiberConeCutName + ":" + fiberConeCutTransName;

    if (i == 0)
    {
      // Cut out the hole in the innermost fiber volume
      boolFormula += "-" + fiberHoleCutName + ":" + fiberHoleCutTransName;
    }

    // Remove holes for screws and rods
    boolFormula += "-" + sScrewHolesCSName;
    boolFormula += "-" + sRodHolesCSName;

    const TGeoCompositeShape *fiberCS = new TGeoCompositeShape(Form("%sCS", fiberShapeName.Data()), boolFormula.Data());

    new TGeoVolume(createVolumeName(sFiberName, i + 1).Data(), fiberCS, mMediumFiberRings[i]);
  }

  // Volume for fibers in front of PMTs
  for (int i = 0; i < sNumberOfPMTFiberVolumes; i++)
  {
    new TGeoVolume(createVolumeName(sFiberName + sPmtName, i + 1).Data(),
                   new TGeoTube(createVolumeName(sFiberName + sPmtName + "Shape", i + 1).Data(), 0, sDrPmt / 2, sDzPmt / 2),
                   mMediumFiberPMTs[i]);
  }
}

void AliFV0::initializeScrews()
{
  for (int i = 0; i < sNumberOfScrewTypes; ++i)
  {
    TString screwName = createVolumeName(sScrewName, i);
    const TGeoShape *screwShape = createScrewShape(screwName + "Shape", i, 0, 0, 0);

    // If modifying materials, make sure the appropriate initialization is done in initializeXxxMedium() methods
    new TGeoVolume(screwName.Data(), screwShape, mMediumScrewTypes[i]);
  }
}

void AliFV0::initializeRods()
{
  for (int i = 0; i < sNumberOfRodTypes; ++i)
  {
    TString rodName = createVolumeName(sRodName, i);
    const TGeoShape *rodShape = createRodShape(rodName + "Shape", i, -sEpsilon, -sEpsilon);

    // If modifying materials, make sure the appropriate initialization is done in initializeXxxMedium() methods
    new TGeoVolume(rodName.Data(), rodShape, mMediumRodTypes[i]);
  }
}

void AliFV0::initializeMetalContainer()
{
  // The metal container is constructed starting from the backplate. The backplate is positioned first, relative to
  // the scintillator cells. The rest of the container parts are positioned relative to the backplate.
  //
  // Caution: some position variables are in global coords, and some are relative to some other part of the container

  // Backplate

  TString backPlateName = "FV0_BackPlate";                        // the full backplate
  TString backPlateStandName = backPlateName + "Stand";           // the stand part of the backplate
  TString backPlateHoleName = backPlateName + "Hole";             // the hole in the middle of the backplate
  TString backPlateHoleCutName = backPlateHoleName + "Cut";       // extension of the hole
  TString backPlateStandTransName = backPlateStandName + "Trans"; // shift of the backplate stand
  TString backPlateHoleTransName = backPlateHoleName + "Trans";   // shift of the backplate inner radius

  new TGeoTubeSeg(backPlateName.Data(), 0, sDrMaxContainerBack, sDzContainerBack / 2, -90, 90);
  new TGeoBBox(backPlateStandName.Data(), sDxContainerStand / 2, (sDrMaxContainerBack + sDyContainerStand) / 2, sDzContainerBack / 2);
  new TGeoTubeSeg(backPlateHoleName.Data(), 0, sDrContainerHole, sDzContainerBack / 2, -90, 90);
  new TGeoBBox(backPlateHoleCutName.Data(), -sXShiftContainerHole, sDrContainerHole, sDzContainerBack);

  createAndRegisterTrans(backPlateStandTransName, sDxContainerStand / 2, -(sDrMaxContainerBack + sDyContainerStand) / 2, 0);
  createAndRegisterTrans(backPlateHoleTransName, sXShiftContainerHole, 0, 0);

  // Backplate composite shape
  TString backPlateBoolFormula = "";
  backPlateBoolFormula += backPlateName;
  backPlateBoolFormula += "+" + backPlateStandName + ":" + backPlateStandTransName;
  backPlateBoolFormula += "-" + backPlateHoleName + ":" + backPlateHoleTransName;
  backPlateBoolFormula += "-" + backPlateHoleCutName;

  TString backPlateCSName = backPlateName + "CS";
  TString backPlateCSTransName = backPlateCSName + "Trans";

  new TGeoCompositeShape(backPlateCSName.Data(), backPlateBoolFormula.Data());
  createAndRegisterTrans(backPlateCSTransName, 0, 0, sZContainerBack);

  // Frontplate

  // the z-position o the frontplate
  const float zPosFrontPlate = sZContainerFront;
  // the height of the total stand overlapping with the rest of the plate
  const float dyFrontPlateStand = sDyContainerStand + (sDrMaxContainerFront - sDrMinContainerFront) / 2;
  // the y-position of the total stand
  const float yPosFrontPlateStand = -sDrMaxContainerFront - sDyContainerStand + dyFrontPlateStand / 2;

  TString frontPlateName = "FV0_FrontPlate";
  TString frontPlateStandName = frontPlateName + "Stand";
  TString frontPlateTransName = frontPlateName + "Trans";
  TString frontPlateStandTransName = frontPlateStandName + "Trans";

  new TGeoTubeSeg(frontPlateName.Data(), sDrMinContainerFront, sDrMaxContainerFront, sDzContainerFront / 2, -90, 90);
  new TGeoBBox(frontPlateStandName.Data(), sDxContainerStand / 2, dyFrontPlateStand / 2, sDzContainerBack / 2);

  createAndRegisterTrans(frontPlateTransName, 0, 0, zPosFrontPlate);
  createAndRegisterTrans(frontPlateStandTransName, sDxContainerStand / 2, yPosFrontPlateStand, 0);

  // Frontplate cone composite shape
  TString frontPlateBoolFormula = "";
  frontPlateBoolFormula += frontPlateName;
  frontPlateBoolFormula += "+" + frontPlateStandName + ":" + frontPlateStandTransName;

  TString frontPlateCSName = frontPlateName + "CS";

  new TGeoCompositeShape(frontPlateCSName.Data(), frontPlateBoolFormula.Data());

  // Frontplate cone

  // radial thickness of frontplate cone in the xy-plane
  const float thicknessFrontPlateCone = sXYThicknessContainerCone;
  // z-position of the frontplate cone relative to the frontplate
  const float zPosCone = sDzContainerFront / 2 - sDzContainerCone / 2;

  TString frontPlateConeName = "FV0_FrontPlateCone";                // no volume with this name
  TString frontPlateConeShieldName = frontPlateConeName + "Shield"; // the "sides" of the cone
  TString frontPlateConeShieldTransName = frontPlateConeShieldName + "Trans";

  new TGeoConeSeg(frontPlateConeShieldName.Data(), sDzContainerCone / 2, sDrMinContainerCone,
                  sDrMinContainerCone + thicknessFrontPlateCone, sDrMinContainerFront - thicknessFrontPlateCone,
                  sDrMinContainerFront,
                  -90, 90);
  createAndRegisterTrans(frontPlateConeShieldTransName, 0, 0, zPosCone);

  // Frontplate cone "bottom"

  // z-position of the cone bottom relative to the frontplate
  const float zPosConePlate = sDzContainerFront / 2 - sDzContainerCone + thicknessFrontPlateCone / 2;
  // the bottom of the cone
  TString frontPlateConePlateName = frontPlateConeName + "Plate";

  new TGeoTubeSeg(frontPlateConePlateName.Data(), 0, sDrMinContainerCone + thicknessFrontPlateCone,
                  thicknessFrontPlateCone / 2, -90, 90);

  // Frontplate cone bottom composite shape
  TString frontPlateConePlateCSBoolFormula;
  frontPlateConePlateCSBoolFormula += frontPlateConePlateName;
  frontPlateConePlateCSBoolFormula += "-" + backPlateHoleName + ":" + backPlateHoleTransName;

  TString frontPlateConePlateCSName = frontPlateConePlateName + "CS";
  TString frontPlateConePlateCSTransName = frontPlateConePlateCSName + "Trans";
  new TGeoCompositeShape(frontPlateConePlateCSName.Data(), frontPlateConePlateCSBoolFormula.Data());
  createAndRegisterTrans(frontPlateConePlateCSTransName, 0, 0, zPosConePlate);

  // Frontplate cone composite shape
  TString frontPlateConeCSBoolFormula = "";
  frontPlateConeCSBoolFormula += frontPlateConeShieldName + ":" + frontPlateConeShieldTransName;
  frontPlateConeCSBoolFormula += "+" + frontPlateConePlateCSName + ":" + frontPlateConePlateCSTransName;

  TString frontPlateConeCSName = frontPlateConeName + "CS";
  new TGeoCompositeShape(frontPlateConeCSName.Data(), frontPlateConeCSBoolFormula.Data());

  // Shields
  const float dzShieldGap = 0.7;                         // z-distance between the shields and the front- and backplate outer edges (in z-direction)
  const float dzShield = sDzContainer - 2 * dzShieldGap; // depth of the shields

  // Outer shield
  const float zPosOuterShield = (sZContainerBack + sZContainerFront) / 2; // z-position of the outer shield

  TString outerShieldName = "FV0_OuterShield";
  TString outerShieldTransName = outerShieldName + "Trans";

  new TGeoTubeSeg(outerShieldName.Data(), sDrMinContainerOuterShield, sDrMaxContainerOuterShield, dzShield / 2, -90, 90);
  createAndRegisterTrans(outerShieldTransName, 0, 0, zPosOuterShield);

  // Inner shield
  const float dzInnerShield = sDzContainer - sDzContainerCone - dzShieldGap;                              // depth of the inner shield
  const float zPosInnerShield = sZContainerBack - sDzContainerBack / 2 + dzShieldGap + dzInnerShield / 2; // z-position of the inner shield relative to the backplate

  TString innerShieldName = "FV0_InnerShield";
  TString innerShieldCutName = innerShieldName + "Cut";

  new TGeoTubeSeg(innerShieldName.Data(), sDrMinContainerInnerShield, sDrMaxContainerInnerShield, dzInnerShield / 2, -90, 90);
  new TGeoBBox(innerShieldCutName.Data(), fabs(sXShiftContainerHole), sDrMaxContainerInnerShield, dzInnerShield / 2);

  // Inner shield composite shape
  TString innerShieldCSBoolFormula;
  innerShieldCSBoolFormula = innerShieldName;
  innerShieldCSBoolFormula += "-" + innerShieldCutName;

  TString innerShieldCSName = innerShieldName + "CS";
  TString innerShieldCSTransName = innerShieldCSName + "Trans";
  new TGeoCompositeShape(innerShieldCSName.Data(), innerShieldCSBoolFormula.Data());
  createAndRegisterTrans(innerShieldCSTransName, sXShiftContainerHole, 0, zPosInnerShield);

  // Cover
  const float dzCover = sDzContainer;                       // Depth of the covers
  const float zPosCoverConeCut = zPosFrontPlate + zPosCone; // Set the cone cut relative to the frontplate so that the exact position of the aluminium cone part can be used.

  TString coverName = "FV0_Cover";
  TString coverConeCutName = coverName + "ConeCut";
  TString coverHoleCutName = coverName + "HoleCut";

  new TGeoBBox(coverName.Data(), sDxContainerCover / 2, sDrMaxContainerOuterShield, dzCover / 2);
  new TGeoCone(coverConeCutName.Data(), sDzContainerCone / 2, 0, sDrMinContainerCone + thicknessFrontPlateCone, 0,
               sDrMinContainerFront);
  new TGeoTubeSeg(coverHoleCutName.Data(), 0, sDrMinContainerInnerShield, dzCover / 2, 0, 360);

  TString coverTransName = coverName + "Trans";
  TString coverConeCutTransName = coverConeCutName + "Trans";
  TString coverHoleCutTransName = coverHoleCutName + "Trans";

  createAndRegisterTrans(coverTransName, sDxContainerCover / 2, 0, zPosOuterShield);
  createAndRegisterTrans(coverConeCutTransName, 0, 0, zPosCoverConeCut);
  createAndRegisterTrans(coverHoleCutTransName.Data(), sXShiftContainerHole, 0, zPosOuterShield);

  // Cover composite shape
  TString coverCSBoolFormula = "";
  coverCSBoolFormula += coverName + ":" + coverTransName;
  coverCSBoolFormula += "-" + coverConeCutName + ":" + coverConeCutTransName;
  coverCSBoolFormula += "-" + coverHoleCutName + ":" + coverHoleCutTransName;

  TString coverCSName = coverName + "CS";
  new TGeoCompositeShape(coverCSName.Data(), coverCSBoolFormula.Data());

  // Stand bottom
  const float dzStandBottom = sDzContainer - sDzContainerBack - sDzContainerFront;
  const float dyStandBottomGap = 0.5; // This bottom part is not vertically aligned with the "front and backplate stands"
  const float dxStandBottomHole = 9.4;
  const float dzStandBottomHole = 20.4;
  const float dxStandBottomHoleSpacing = 3.1;

  TString standName = "FV0_StandBottom";
  TString standHoleName = standName + "Hole";

  new TGeoBBox(standName.Data(), sDxContainerStandBottom / 2, sDyContainerStandBottom / 2, dzStandBottom / 2);
  new TGeoBBox(standHoleName.Data(), dxStandBottomHole / 2, sDyContainerStandBottom / 2 + sEpsilon, dzStandBottomHole / 2);

  TString standHoleTrans1Name = standHoleName + "Trans1";
  TString standHoleTrans2Name = standHoleName + "Trans2";
  TString standHoleTrans3Name = standHoleName + "Trans3";

  createAndRegisterTrans(standHoleTrans1Name, -dxStandBottomHoleSpacing - dxStandBottomHole, 0, 0);
  createAndRegisterTrans(standHoleTrans2Name, 0, 0, 0);
  createAndRegisterTrans(standHoleTrans3Name, dxStandBottomHoleSpacing + dxStandBottomHole, 0, 0);

  // Stand bottom composite shape
  TString standCSName = standName + "CS";

  TString standBoolFormula = "";
  standBoolFormula += standName;
  standBoolFormula += "-" + standHoleName + ":" + standHoleTrans1Name;
  standBoolFormula += "-" + standHoleName + ":" + standHoleTrans2Name;
  standBoolFormula += "-" + standHoleName + ":" + standHoleTrans3Name;

  new TGeoCompositeShape(standCSName.Data(), standBoolFormula.Data());

  TString standCSTransName = standCSName + "Trans";

  createAndRegisterTrans(standCSTransName.Data(),
                         sDxContainerStand - sDxContainerStandBottom / 2,
                         -(sDrMaxContainerBack + sDyContainerStand) + sDyContainerStandBottom / 2 + dyStandBottomGap,
                         sZContainerMid);

  // Composite shape
  TString boolFormula = "";
  boolFormula += backPlateCSName + ":" + backPlateCSTransName;
  boolFormula += "+" + frontPlateCSName + ":" + frontPlateTransName;
  boolFormula += "+" + frontPlateConeCSName + ":" + frontPlateTransName;
  boolFormula += "+" + outerShieldName + ":" + outerShieldTransName;
  boolFormula += "+" + innerShieldCSName + ":" + innerShieldCSTransName;
  boolFormula += "+" + coverCSName;
  boolFormula += "+" + standCSName + ":" + standCSTransName;
  boolFormula += "-" + sScrewHolesCSName; // Remove holes for screws
  boolFormula += "-" + sRodHolesCSName;   // Remove holes for rods

  TString aluContCSName = "FV0_AluContCS";
  const TGeoCompositeShape *aluContCS = new TGeoCompositeShape(aluContCSName.Data(), boolFormula.Data());

  // Volume
  TString aluContName = createVolumeName(sContainerName);
  const TGeoMedium *medium = gGeoManager->GetMedium("FIT_Aluminium$");
  new TGeoVolume(aluContName.Data(), aluContCS, medium);
}

void AliFV0::assembleSensVols(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  if (mEnabledComponents[eScintillator])
  {
    assembleScintSectors(vFV0Right, vFV0Left);
  }
}

void AliFV0::assembleNonSensVols(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  if (mEnabledComponents[ePlastics])
  {
    assemblePlasticSectors(vFV0Right, vFV0Left);
  }
  if (mEnabledComponents[ePmts])
  {
    assemblePmts(vFV0Right, vFV0Left);
  }
  if (mEnabledComponents[eFibers])
  {
    assembleFibers(vFV0Right, vFV0Left);
  }
  if (mEnabledComponents[eScrews])
  {
    assembleScrews(vFV0Right, vFV0Left);
  }
  if (mEnabledComponents[eRods])
  {
    assembleRods(vFV0Right, vFV0Left);
  }
  if (mEnabledComponents[eContainer])
  {
    assembleMetalContainer(vFV0Right, vFV0Left);
  }
}

void AliFV0::assembleScintSectors(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *sectors = buildSectorAssembly(sScintillatorName);

  // Copy numbers used for cell identification in AliFV0::getCurrentCellId()
  vFV0Right->AddNode(sectors, 0);
  vFV0Left->AddNode(sectors, 1);
}

void AliFV0::assemblePlasticSectors(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *sectors = buildSectorAssembly(sPlasticName);

  // Move the plastic cells next to the scintillator cells
  TGeoTranslation *trans = new TGeoTranslation(0, 0, sZPlastic);

  vFV0Right->AddNode(sectors, 0, trans);
  vFV0Left->AddNode(sectors, 1, trans);
}

void AliFV0::assemblePmts(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *pmts = new TGeoVolumeAssembly(createVolumeName("PMTS").Data());
  TGeoVolume *pmt = gGeoManager->GetVolume(createVolumeName(sPmtName).Data());
  if (!pmt)
  {
    AliWarning("FV0 AliFV0::assemblePmts(): PMT volume not found.");
  }
  else
  {
    for (int i = 0; i < sNumberOfPMTs; i++)
    {
      pmts->AddNode(pmt, i, new TGeoTranslation(sXPmt[i], sYPmt[i], sZPmt));
    }
  }

  vFV0Right->AddNode(pmts, 0);
  vFV0Left->AddNode(pmts, 1);
}

void AliFV0::assembleFibers(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *fibersRight = new TGeoVolumeAssembly(createVolumeName("FIBERSRIGHT").Data());
  TGeoVolumeAssembly *fibersLeft = new TGeoVolumeAssembly(createVolumeName("FIBERSLEFT").Data());
  TGeoVolume *fiber;
  TString volumeName;

  for (int i = 0; i < mRMinFiber.size(); ++i)
  {
    volumeName = createVolumeName(sFiberName, i + 1);
    fiber = gGeoManager->GetVolume(volumeName);
    if (!fiber)
    {
      AliWarning(Form("FV0 geometry: Fiber volume no. %i (%s) not found!", i + 1, volumeName.Data()));
    }
    else
    {
      fibersRight->AddNode(fiber, i);
      fibersLeft->AddNode(fiber, i);
    }
  }

  int iPMTFiberCell = 0;
  for (int i = 0; i < sNumberOfPMTs; i++)
  {
    if (iPMTFiberCell == sNumberOfPMTsPerSector)
    {
      iPMTFiberCell = 0;
    }

    volumeName = createVolumeName(sFiberName + sPmtName, sPMTFiberCellOrder[iPMTFiberCell]);
    fiber = gGeoManager->GetVolume(volumeName);

    if (!fiber)
    {
      AliWarning(Form("FV0 geometry: Volume of fibers from cell %i (%s) not found!", iPMTFiberCell, volumeName.Data()));
    }
    else
    {
      fibersRight->AddNode(fiber, i, new TGeoTranslation(sXPmt[i], sYPmt[i], sZPmt + sDzPmt));
    }

    // The PMT fiber cell order for the left side is not flipped across the x-axis, but rotated clockwise
    volumeName = createVolumeName(sFiberName + sPmtName, sPMTFiberCellOrder[abs(iPMTFiberCell - sNumberOfPMTsPerSector) - 1]);
    fiber = gGeoManager->GetVolume(volumeName);

    if (!fiber)
    {
      AliWarning(Form("FV0 geometry: Volume of fibers from cell %i (%s) not found!", iPMTFiberCell, volumeName.Data()));
    }
    else
    {
      fibersLeft->AddNode(fiber, i, new TGeoTranslation(sXPmt[i], sYPmt[i], sZPmt + sDzPmt));
    }

    iPMTFiberCell++;
  }

  AliDebug(2, Form("FV0 geometry: total weight of fibers = %.4f kg", fibersRight->Weight(1e-5) + fibersLeft->Weight(1e-5)));

  vFV0Right->AddNode(fibersRight, 0);
  vFV0Left->AddNode(fibersLeft, 1);
}

void AliFV0::assembleScrews(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *screws = new TGeoVolumeAssembly(createVolumeName("SCREWS").Data());

  // If modifying something here, make sure screw initialization is OK
  for (int i = 0; i < mScrewPos.size(); ++i)
  {
    TGeoVolume *screw = gGeoManager->GetVolume(createVolumeName(sScrewName, mScrewTypeIDs[i]).Data());
    if (!screw)
    {
      AliWarning(Form("FV0 AliFV0::assembleScrews(): Screw no. %d not found", i));
    }
    else
    {
      screws->AddNode(screw, i, new TGeoTranslation(mScrewPos[i][0] + sXShiftScrews, mScrewPos[i][1], mScrewPos[i][2]));
    }
  }

  vFV0Right->AddNode(screws, 0);
  vFV0Left->AddNode(screws, 1);
}

void AliFV0::assembleRods(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolumeAssembly *rods = new TGeoVolumeAssembly(createVolumeName("RODS").Data());

  // If modifying something here, make sure rod initialization is OK
  for (int i = 0; i < mRodPos.size(); ++i)
  {
    TGeoVolume *rod = gGeoManager->GetVolume(createVolumeName(sRodName, mRodTypeIDs[i]).Data());

    if (!rod)
    {
      AliInfo(Form("FV0 AliFV0::assembleRods(): Rod no. %d not found", i));
    }
    else
    {
      rods->AddNode(rod, i, new TGeoTranslation(mRodPos[i][0] + sXShiftScrews, mRodPos[i][1], mRodPos[i][2]));
    }
  }

  vFV0Right->AddNode(rods, 0);
  vFV0Left->AddNode(rods, 1);
}

void AliFV0::assembleMetalContainer(TGeoVolume *vFV0Right, TGeoVolume *vFV0Left) const
{
  TGeoVolume *container = gGeoManager->GetVolume(createVolumeName(sContainerName).Data());
  if (!container)
  {
    AliWarning("FV0: Could not find container volume");
  }
  else
  {
    vFV0Right->AddNode(container, 0);
    vFV0Left->AddNode(container, 1);
  }
}

TGeoVolumeAssembly *AliFV0::buildSectorAssembly(TString cellName) const
{
  TGeoVolumeAssembly *assembly = new TGeoVolumeAssembly(createVolumeName(cellName).Data());

  for (int iSector = 0; iSector < mSectorTrans.size(); ++iSector)
  {
    TGeoVolumeAssembly *sector = buildSector(cellName, iSector);
    assembly->AddNode(sector, iSector, mSectorTrans[iSector]);
  }

  return assembly;
}

TGeoVolumeAssembly *AliFV0::buildSector(TString cellType, const int iSector) const
{
  TGeoVolumeAssembly *sector = new TGeoVolumeAssembly(createVolumeName(cellType + sSectorName, iSector).Data());

  for (int i = 0; i < sNumberOfCellRings; ++i)
  {
    TGeoVolume *cell = gGeoManager->GetVolume(createVolumeName(cellType + sCellName + sCellTypes[iSector], i).Data());

    if (!cell)
    {
      AliWarning(Form("FV0 AliFV0::buildSector(): Couldn't find cell volume no. %d", i));
    }
    else
    {
      sector->AddNode(cell, i, new TGeoTranslation(sXScintillator, 0, 0));
    }
  }

  return sector;
}

TGeoShape *AliFV0::createScrewShape(TString shapeName, const int screwTypeID, const float xEpsilon,
                                    const float yEpsilon, const float zEpsilon) const
{
  const float xyEpsilon = (fabs(xEpsilon) > fabs(yEpsilon)) ? xEpsilon : yEpsilon;
  const float dzMax = sDzMaxScrewTypes[screwTypeID] / 2 + zEpsilon;
  const float dzMin = sDzMinScrewTypes[screwTypeID] / 2 + zEpsilon;

  TString thinPartName = shapeName + "Thin";
  TString thickPartName = shapeName + "Thick";
  TString thickPartTransName = thickPartName + "Trans";

  if ((screwTypeID == 0) || (screwTypeID == 5))
  { // for screw types 0 and 5 there is no thick part
    return new TGeoTube(shapeName.Data(), 0, sDrMinScrewTypes[screwTypeID] + xyEpsilon, dzMax);
  }
  else
  {
    new TGeoTube(thinPartName.Data(), 0, sDrMinScrewTypes[screwTypeID] + xyEpsilon, dzMax);
    new TGeoTube(thickPartName.Data(), 0, sDrMaxScrewTypes[screwTypeID] + xyEpsilon, dzMin);
    createAndRegisterTrans(thickPartTransName, 0, 0, -dzMax - sZShiftScrew + sDzScintillator + sDzPlastic + dzMin);
    TString boolFormula = thinPartName + "+" + thickPartName + ":" + thickPartTransName;
    return new TGeoCompositeShape(shapeName.Data(), boolFormula.Data());
  }
}

TGeoShape *AliFV0::createRodShape(TString shapeName, const int rodTypeID, const float xEpsilon,
                                  const float yEpsilon, const float zEpsilon) const
{
  const float dxMin = sDxMinRodTypes[rodTypeID] / 2 + xEpsilon;
  const float dxMax = sDxMaxRodTypes[rodTypeID] / 2 + xEpsilon;
  const float dyMin = sDyMinRodTypes[rodTypeID] / 2 + yEpsilon;
  const float dyMax = sDyMaxRodTypes[rodTypeID] / 2 + yEpsilon;
  const float dzMax = sDzMaxRodTypes[rodTypeID] / 2 + zEpsilon;
  const float dzMin = sDzMinRodTypes[rodTypeID] / 2 + zEpsilon;

  TString thinPartName = shapeName + "Thin";
  TString thickPartName = shapeName + "Thick";
  TString thickPartTransName = thickPartName + "Trans";

  new TGeoBBox(thinPartName.Data(), dxMin, dyMin, dzMax);
  new TGeoBBox(thickPartName.Data(), dxMax, dyMax, dzMin);
  createAndRegisterTrans(thickPartTransName, dxMax - dxMin, 0, -dzMax - sZShiftRod + sDzScintillator + sDzPlastic + dzMin);

  TString boolFormula = thinPartName + "+" + thickPartName + ":" + thickPartTransName;

  TGeoCompositeShape *rodShape = new TGeoCompositeShape(shapeName.Data(), boolFormula.Data());
  return rodShape;
}

TGeoTranslation *AliFV0::createAndRegisterTrans(TString name, const double dx, const double dy,
                                                const double dz) const
{
  TGeoTranslation *trans = new TGeoTranslation(name.Data(), dx, dy, dz);
  trans->RegisterYourself();
  return trans;
}

TGeoRotation *AliFV0::createAndRegisterRot(TString name, const double phi, const double theta,
                                           const double psi) const
{
  TGeoRotation *rot = new TGeoRotation(name.Data(), phi, theta, psi);
  rot->RegisterYourself();
  return rot;
}

TString AliFV0::createVolumeName(TString volumeType, const int number) const
{
  return sDetectorName + volumeType + ((number >= 0) ? Form("%d",number) : "");
}

void AliFV0::initializeCellCenters()
{
  const float phi0 = 67.5 * TMath::DegToRad(); // starting phi of one of the sectors
  const float dphi = 45. * TMath::DegToRad();  // phi difference between neighbouring sectors
  const float lutSect2Phi[sNumberOfCellSectors * 2] = {phi0, phi0 - dphi, phi0 - 2 * dphi, phi0 - 3 * dphi, phi0 + dphi, phi0 + 2 * dphi, phi0 + 3 * dphi, phi0 + 4 * dphi};
  for (int cellId = 0; cellId < sNumberOfCells; cellId++)
  {
    float r = 0.5 * (sCellRingRadii[sCellToRing[cellId]] + sCellRingRadii[sCellToRing[cellId] + 1]);
    double x = sXGlobal + r * TMath::Cos(lutSect2Phi[sCellToSector[cellId]]);
    double y = sYGlobal + r * TMath::Sin(lutSect2Phi[sCellToSector[cellId]]);

    /*TVector3 *p = &mCellCenter.at(cellId);
    p->SetXYZ(x, y, sZGlobal);*/
    TVector3 p(x,y,sZGlobal);
    mCellCenter.push_back(p);
  }
}

void AliFV0::initializeReadoutCenters()
{
  for (int channelId = 0; channelId < sNumberOfReadoutChannels; channelId++)
  {
    //TVector3 *p = &mReadoutCenter.at(channelId);
    TVector3 p;          
    if (!isRing5(channelId))
    {
      //p->SetXYZ(getCellCenter(channelId).x(), getCellCenter(channelId).y(), getCellCenter(channelId).z());
      p.SetXYZ(getCellCenter(channelId).x(), getCellCenter(channelId).y(), getCellCenter(channelId).z());
    }
    else
    {
      const int numberOfSectorsR5 = sNumberOfCellSectors * 4; // from both halves of the detector
      const float phi0 = 78.75 * TMath::DegToRad();           // starting phi of one of the sectors
      const float dphi = 22.5 * TMath::DegToRad();            // phi difference between neighbouring sectors
      const float lutReadoutSect2Phi[numberOfSectorsR5] =
          {phi0 - 0 * dphi, phi0 - 1 * dphi, phi0 - 2 * dphi, phi0 - 3 * dphi,
           phi0 - 4 * dphi, phi0 - 5 * dphi, phi0 - 6 * dphi, phi0 - 7 * dphi,
           phi0 + 1 * dphi, phi0 + 2 * dphi, phi0 + 3 * dphi, phi0 + 4 * dphi,
           phi0 + 5 * dphi, phi0 + 6 * dphi, phi0 + 7 * dphi, phi0 + 8 * dphi};

      int iReadoutSector = channelId - ((sNumberOfCellRings - 1) * sNumberOfCellSectors * 2);
      float r = 0.5 * (sCellRingRadii[4] + sCellRingRadii[5]);
      double x = sXGlobal + r * TMath::Cos(lutReadoutSect2Phi[iReadoutSector]);
      double y = sYGlobal + r * TMath::Sin(lutReadoutSect2Phi[iReadoutSector]);
      //p->SetXYZ(x, y, sZGlobal);
      p.SetXYZ(x, y, sZGlobal);
    }
    mReadoutCenter.push_back(p);
  }
}

void AliFV0::addAlignableVolumes() const
{
  //
  // Creates entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path.
  //
  //  First version (mainly ported from AliRoot)
  //

  AliInfo("FV0: Add alignable volumes");

  if (!gGeoManager)
  {
    AliFatal("TGeoManager doesn't exist !");
    return;
  }

  TString volPath, symName;
  for (auto &half : {"RIGHT_0", "LEFT_1"})
  {
    volPath = Form("/ALIC_1/FV0_1/FV0%s", half);
    symName = Form("FV0%s", half);
    AliInfo(Form("FV0: Add alignable volume: %s : %s", symName.Data(), volPath.Data()));
    if (!gGeoManager->SetAlignableEntry(symName.Data(), volPath.Data()))
    {
      AliFatal(Form("FV0: Unable to set alignable entry! %s : %s", symName.Data(), volPath.Data()));
    }
  }
}
