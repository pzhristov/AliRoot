#ifndef ALIFT0_H
#define ALIFT0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
// Full geomrtry  hits classes for detector: FIT    //
////////////////////////////////////////////////

#include <Rtypes.h>
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGraph.h"
#include <TGeoMatrix.h>
#include <TVirtualMC.h>
#include <TArrayI.h>
#include <TVector3.h>
#include <TString.h>
//#include <sstream>
//#include <string>
//#include <vector>

class AliFT0 : public TNamed
{
public:
  enum MedConstants
  {
    kAir = 1,
    kVac = 3,
    kCeramic = 4,
    kGlass = 6,
    kAl = 15,
    kOpGlass = 16,
    kOptAl = 17,
    kOpGlassCathode = 19,
    kCable = 23,
    kMCPwalls = 25
  }; // materials

  enum Constants
  {
    NCellsA = 24, // number of radiatiors on A side
    NCellsC = 28  // number of radiatiors on C side
  };
  ///
  /// Default constructor.
  /// It must be kept public for root persistency purposes,
  /// but should never be called by the outside world
  AliFT0();
  AliFT0(const AliFT0 &geom);
  AliFT0 &operator=(const AliFT0 &source); // assignment operator
  ~AliFT0() {};
  
  ///
  TVector3 tiltMCP(int imcp) { return mAngles[imcp]; }

  //   const int Nchannels = o2::ft0::Constants::sNCHANNELS_PM; // number of PM channels
  const int Nchannels = 208;                     // number of sensors
  const int Nsensors = 208;                      // number of sensors
  const float ZdetA = 335.5;                     // Z position of center volume  on A side
  const float ZdetC = 82;                        //  Z position of center volume on C side
  const float ChannelWidth = 13.02;              // channel width in ps
  const float ChannelWidthInverse = 0.076804916; // channel width in ps inverse


  /// Initialization of the detector is done here
  void InitializeO2Detector();

  /// Base class to create the detector geometry
  void ConstructGeometry();
  void ConstructOpGeometry();
  void SetOneMCP(TGeoVolume *stl);

  void SetCablesA(TGeoVolume *stl);
  TGeoVolume *SetCablesSize(int mod);

  void DefineSim2LUTindex();
  /// Add alignable  volumes
  void addAlignableVolumes() const;

  int getMediumID(MedConstants mediumID)
  {
    return fIdtmed[mediumID];
  }

  void setMediumArray(TArrayI *mediumIDs)
  {
    fIdtmed = mediumIDs->GetArray();
  }


  TVector3 mAngles[28];
  unsigned short fDummy;
  
  TString cPlateShapeString();

  // BEGIN: Support structure constants
  // define some error to avoid overlaps
  const Float_t sEps = 0.05;
  const Float_t sFrameZ = 5.700;

  // PMT socket dimensions
  const Float_t sPmtSide = 5.950;
  const Float_t sPmtZ = 3.750;

  // quartz radiator socket dimensions
  const Float_t sQuartzRadiatorSide = 5.350;
  const Float_t sQuartzRadiatorZ = 1.950;
  // for the rounded socket corners
  const Float_t sCornerRadius = 0.300;

  // quartz & PMT socket transformations
  const Float_t sQuartzHeight = -sFrameZ / 2 + sQuartzRadiatorZ / 2;
  const Float_t sPmtHeight = sFrameZ / 2 - sPmtZ / 2;
  const Float_t sPmtCornerTubePos = -0.15;
  const Float_t sPmtCornerPos = 2.825;
  const Float_t sEdgeCornerPos[2] = {-6.515, -0.515};
  const Float_t sQuartzFrameOffsetX = -1.525;
  // END: Support structure constants

  const Float_t mPosModuleAx[NCellsA] = {-12.25, -6.15, -0.05, 6.15, 12.25,
                                         -12.25, -6.15, -0.05, 6.15, 12.25,
                                         -13.58, -7.48, 7.48, 13.58,
                                         -12.25, -6.15, 0.05, 6.15, 12.25,
                                         -12.25, -6.15, 0.05, 6.15, 12.25};

  const Float_t mPosModuleAy[NCellsA] = {12.2, 12.2, 13.53, 12.2, 12.2,
                                         6.1, 6.1, 7.43, 6.1, 6.1,
                                         0.0, 0.0, 0.0, 0.0,
                                         -6.1, -6.1, -7.43, -6.1, -6.1,
                                         -12.2, -12.2, -13.53, -12.2, -12.2};

  Double_t mPosModuleCx[NCellsC];
  Double_t mPosModuleCy[NCellsC];
  Double_t mPosModuleCz[NCellsC];
  Float_t mStartC[3] = {20., 20, 5.5};
  Float_t mStartA[3] = {20., 20., 5};
  Float_t mInStart[3] = {2.9491, 2.9491, 2.6};
  
private:
  
  // Define the aluminium frame for the detector
  TGeoVolume *constructFrameAGeometry(); // A-side
  TGeoVolume *constructFrameCGeometry(); // C-side

  Int_t *fIdtmed;

  ClassDef(AliFT0, 8);
};

#endif
