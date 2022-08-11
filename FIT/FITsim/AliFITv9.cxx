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

/////////////////////////////////////////////////////////////////////
//                                                                 //
// FIT detector full geometry  version 9 similar to O2
//
// Ported from O2 by Hadi Hassan, hadi.hassan@cern.ch
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TGeoVolume.h"
#include <TGeoMedium.h>

#include <TGeoGlobalMagField.h>
#include <TGraph.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TString.h>
#include <TVirtualMC.h>

#include "AliLog.h"
#include "AliMagF.h"
#include "AliRun.h"
#include "AliMC.h"
#include "AliTrackReference.h"

#include "AliFITv9.h"

ClassImp(AliFITv9);

using std::cout;
using std::endl;

//--------------------------------------------------------------------
AliFITv9::AliFITv9() : AliFIT(), fIdSens1(0), fPMTeff(0x0), fSenseless(-1)
{
  fFT0Det = new AliFT0();
  fFV0Det = new AliFV0(AliFV0::eFull);
}

//--------------------------------------------------------------------
AliFITv9::AliFITv9(const char *name, const char *title) : AliFIT(name, title), fIdSens1(0), fPMTeff(0x0), fSenseless(-1)
{
  fFT0Det = new AliFT0();
  fFV0Det = new AliFV0(AliFV0::eFull);
  fIshunt = 2;
}

//_____________________________________________________________________________
AliFITv9::~AliFITv9()
{
  if (fFT0Det)
    delete fFT0Det;
  if (fFV0Det)
    delete fFV0Det;
}

//-------------------------------------------------------------------------
void AliFITv9::CreateGeometry()
{
  fFT0Det->ConstructGeometry();
  fFV0Det->ConstructGeometry();
}

//--------------------------------------------------------------------
void AliFITv9::AddAlignableVolumes() const
{
  // Create entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path. Needs to be synchronized with
  // eventual changes in the geometry.
  fFT0Det->addAlignableVolumes();
  fFV0Det->addAlignableVolumes();
}

//------------------------------------------------------------------------
void AliFITv9::CreateMaterials()
{

  Int_t isxfld = ((AliMagF *)TGeoGlobalMagField::Instance()->GetField())->Integ();
  Float_t sxmgmx = ((AliMagF *)TGeoGlobalMagField::Instance()->GetField())->Max();

  Float_t aAir[4] = {12.0107, 14.0067, 15.9994, 39.948};
  Float_t zAir[4] = {6., 7., 8., 18.};
  Float_t wAir[4] = {0.000124, 0.755267, 0.231781, 0.012827};
  Float_t dAir = 1.20479E-3;
  Float_t dAir1 = 1.20479E-11;
  // Radiator  glass SiO2
  Float_t aglass[2] = {28.0855, 15.9994};
  Float_t zglass[2] = {14., 8.};
  Float_t wglass[2] = {1., 2.};
  Float_t dglass = 2.2;
  // MCP glass SiO2
  Float_t dglass_mcp = 1.3;
  /* Ceramic   97.2% Al2O3 , 2.8% SiO2 : average material for
   -  stack of 2 MCPs thickness 2mm with density 1.6 g/cm3
   -  back wall of MCP thickness 2 mm with density 2.4 g/cm3
   -  MCP electrods thickness 1 mm with density 4.2 g/cm3
   -  Backplane PCBs thickness 4.5 mm with density 1.85 g/cm3
   -  electromagnetic shielding 1 mm  with density 2.8 g/cm3
   -  Al back cover 5mm  2.7 g/cm3
  */
  Float_t aCeramic[2] = {26.981539, 15.9994};
  Float_t zCeramic[2] = {13., 8.};
  Float_t wCeramic[2] = {2., 3.};
  Float_t denscer = 2.37;

  // MCP walls Ceramic+Nickel (50//50)
  const Int_t nCeramicNice = 3;
  Float_t aCeramicNicel[3] = {26.981539, 15.9994, 58.6934};
  Float_t zCeramicNicel[3] = {13., 8., 28};
  Float_t wCeramicNicel[3] = {0.2, 0.3, 0.5};
  Float_t denscerCeramicNickel = 5.6;

  // Mixed Cables material simulated as plastic with density taken from description of Low Loss Microwave Coax24 AWG 0
  //   plastic + cooper (6%)
  const Int_t nPlast = 4;
  Float_t aPlast[nPlast] = {1.00784, 12.0107, 15.999, 63.54};
  Float_t zPlast[nPlast] = {1, 6, 8, 29};
  Float_t wPlast[nPlast] = {0.08, 0.53, 0.22, 0.17}; ////!!!!!
  const Float_t denCable = 3.66;

  //*** Definition Of avaible FIT materials ***
  AliMixture(1, "Vacuum$", aAir, zAir, dAir1, 4, wAir);
  AliMixture(2, "Air$", aAir, zAir, dAir, 4, wAir);
  AliMixture(3, "Ceramic$", aCeramic, zCeramic, denscer, -2, wCeramic);
  AliMixture(4, "MCP glass   $", aglass, zglass, dglass_mcp, -2, wglass);
  AliMaterial(11, "Aliminium$", 26.98, 13.0, 2.7, 8.9, 999);
  AliMixture(23, "CablePlasticCooper$", aPlast, zPlast, denCable, 4, wPlast);
  AliMixture(24, "Radiator Optical glass$", aglass, zglass, dglass, -2, wglass);
  AliMixture(25, "MCPwalls $", aCeramicNicel, zCeramicNicel, denscerCeramicNickel, 3, wCeramicNicel);

  AliMedium(1, "Air$", 2, 0, isxfld, sxmgmx, 10., .1, 1., .003, .003);
  AliMedium(3, "Vacuum$", 1, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(4, "Ceramic$", 3, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(6, "Glass$", 4, 0, isxfld, sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(15, "Aluminium$", 11, 0, isxfld, sxmgmx, 10., .01, 1., .003, .003);
  AliMedium(17, "OptAluminium$", 11, 0, isxfld, sxmgmx, 10., .01, 1., .003, .003);
  AliMedium(16, "OpticalGlass$", 24, 1, isxfld, sxmgmx, 10., .01, .1, .003, .01);
  AliMedium(19, "OpticalGlassCathode$", 24, 1, isxfld, sxmgmx, 10., .01, .1, .003, .003);
  AliMedium(23, "Cables$", 23, 1, isxfld, sxmgmx, 10., .1, 1., .003, .003);
  AliMedium(25, "MCPWalls", 25, 1, isxfld, sxmgmx, 10., .1, 1., .003, .003);

  // V0+

  // EJ-204 scintillator, based on polyvinyltoluene
  const Int_t nScint = 2;
  Float_t aScint[nScint] = {1.00784, 12.0107};
  Float_t zScint[nScint] = {1, 6};
  Float_t wScint[nScint] = {0.07085, 0.92915}; // based on EJ-204 datasheet: n_atoms/cm3
  const Float_t dScint = 1.023;

  // PMMA plastic mixture: (C5O2H8)n, same for plastic fiber support and for the fiber core
  //   Fiber cladding is different, but it comprises only 3% of the fiber volume, so it is not included
  const Int_t nPlastV0 = 3;
  Float_t aPlastV0[nPlastV0] = {1.00784, 12.0107, 15.999};
  Float_t zPlastV0[nPlastV0] = {1, 6, 8};
  Float_t wPlastV0[nPlastV0] = {0.08054, 0.59985, 0.31961};
  const Float_t dPlast = 1.18;

  // Densities of fiber-equivalent material, for five radially-distributed density regions
  const Int_t nFiberRings = 5;
  Float_t dFiberRings[nFiberRings] = {0.035631, 0.059611, 0.074765, 0.079451, 0.054490};

  // Densities of fiber-equivalent material, for five density regions in front of the PMTs
  const Int_t nFiberPMTs = 5;
  Float_t dFiberPMTs[nFiberPMTs] = {0.109313, 0.217216, 0.364493, 1.373307, 1.406480};

  // Titanium grade 5 (https://en.wikipedia.org/wiki/Titanium_alloy) without iron and oxygen
  const Int_t nTitanium = 3;
  Float_t aTitanium[nTitanium] = {47.87, 26.98, 50.94};
  Float_t zTitanium[nTitanium] = {22, 13, 23};
  Float_t wTitanium[nTitanium] = {0.9, 0.06, 0.04};
  const Float_t dTitanium = 4.42;

  // Mixture of elements found in the PMTs
  const Int_t nPMT = 14;
  Float_t aPMT[nPMT] = {63.546, 65.38, 28.085, 15.999, 12.011, 1.008, 14.007, 55.845, 51.996, 10.81, 121.76, 132.91, 9.0122, 26.982};
  Float_t zPMT[nPMT] = {29, 30, 14, 8, 6, 1, 7, 26, 24, 5, 51, 55, 4, 13};
  Float_t wPMT[nPMT] = {0.07, 0.02, 0.14, 0.21, 0.11, 0.02, 0.02, 0.04, 0.01, 0.01, 0.000001, 0.00001, 0.01, 0.34};
  const Float_t dPMT = fFV0Det->getPmtDensity();

  Int_t matId = 31;                 // tmp material id number
  const Int_t unsens = 0, sens = 1; // sensitive or unsensitive medium

  Float_t tmaxfd = -10.0; // max deflection angle due to magnetic field in one step
  Float_t stemax = 0.1;   // max step allowed [cm]
  Float_t deemax = 1.0;   // maximum fractional energy loss in one step 0<deemax<=1
  Float_t epsil = 0.03;   // tracking precision [cm]
  Float_t stmin = -0.001; // minimum step due to continuous processes [cm] (negative value: choose it automatically)

  Int_t fieldType = ((AliMagF *)TGeoGlobalMagField::Instance()->GetField())->Integ(); // Field type
  Double_t maxField = ((AliMagF *)TGeoGlobalMagField::Instance()->GetField())->Max(); // Field max.

  AliMixture(8, "Scintillator$", aScint, zScint, dScint, nScint, wScint);
  AliMedium(8, "Scintillator$", 8, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);

  AliMixture(9, "Plastic$", aPlastV0, zPlastV0, dPlast, nPlastV0, wPlastV0);
  AliMedium(9, "Plastic$", 9, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);

  for (int i = 0; i < nFiberRings; i++)
  {
    AliMixture(++matId, Form("FiberRing%i$", i + 1), aPlastV0, zPlastV0, dFiberRings[i], nPlastV0, wPlastV0);
    AliMedium(matId, Form("FiberRing%i$", i + 1), matId, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);
  }

  for (int i = 0; i < nFiberPMTs; i++)
  {
    AliMixture(++matId, Form("FiberPMT%i$", i + 1), aPlastV0, zPlastV0, dFiberPMTs[i], nPlastV0, wPlastV0);
    AliMedium(matId, Form("FiberPMT%i$", i + 1), matId, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);
  }

  AliMixture(++matId, "Titanium$", aTitanium, zTitanium, dTitanium, nTitanium, wTitanium);
  AliMedium(matId, "Titanium$", matId, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);

  AliMixture(++matId, "PMT$", aPMT, zPMT, dPMT, nPMT, wPMT);
  AliMedium(matId, "PMT$", matId, unsens, fieldType, maxField, tmaxfd, stemax, deemax, epsil, stmin);

  std::cout << "\n AliFITv9:: ===> Debug:: Created All Medium Successfully ------------" << std::endl;

  fFT0Det->setMediumArray(fIdtmed);

  AliDebugClass(1, ": ++++++++++++++Medium set++++++++++");
}

//-------------------------------------------------------------------
void AliFITv9::Init()
{
  AliFIT::Init();

  fIdSens1 = TVirtualMC::GetMC()->VolId("0REG"); // <--- 0REG is sensitive volume Id?

  std::string volSensitiveName;
  for (int iCell = 0; iCell < fFV0Det->getSensitiveVolumeNames().size(); iCell++)
  {
    volSensitiveName = fFV0Det->getSensitiveVolumeNames().at(iCell);
    if (volSensitiveName.empty())
    {
      AliFatal(Form("FV0: Can't find sensitive volume %s", volSensitiveName.c_str()));
    }
    else
    {
      fIdV0Plus[iCell] = TVirtualMC::GetMC()->VolId(volSensitiveName.c_str());
      AliInfo(Form("FV0: Sensitive volume added: %s with ID: %d", volSensitiveName.c_str(), fIdV0Plus[iCell]));
    }
  }

  AliDebug(1, Form("%s: *** FIT version 1 initialized ***\n", ClassName()));
}

//-------------------------------------------------------------------
void AliFITv9::StepManager()
{
  // Called for every step in the FIT AliFITv9
  Int_t id, copy, copy1;
  static Int_t vol[3];
  TLorentzVector pos;
  TLorentzVector mom;

  if (!TVirtualMC::GetMC()->IsTrackAlive())
    return; // particle has disappeared

  id = TVirtualMC::GetMC()->CurrentVolID(copy);

  if (id == fIdSens1)
  {

    if (TVirtualMC::GetMC()->IsTrackEntering())
    {

      TVirtualMC::GetMC()->CurrentVolOffID(1, copy1);
      vol[1] = copy1;
      vol[0] = copy;

      TVirtualMC::GetMC()->TrackPosition(pos);
      TVirtualMC::GetMC()->TrackMomentum(mom);

      Float_t Pt = TMath::Sqrt(mom.Px() * mom.Px() + mom.Py() * mom.Py());

      fHits[0] = pos[0];
      fHits[1] = pos[1];
      fHits[2] = pos[2];

      if (pos[2] < 0)
      {
        vol[2] = 0;
      }
      else
      {
        vol[2] = 1;
      }
      Float_t etot = TVirtualMC::GetMC()->Etot();
      fHits[3] = etot;

      Int_t iPart = TVirtualMC::GetMC()->TrackPid();
      Int_t partID = TVirtualMC::GetMC()->IdFromPDG(iPart);
      fHits[4] = partID;

      Float_t ttime = TVirtualMC::GetMC()->TrackTime();
      fHits[5] = ttime * 1e12;
      fHits[6] = TVirtualMC::GetMC()->TrackCharge();
      fHits[7] = mom.Px();
      fHits[8] = mom.Py();
      fHits[9] = mom.Pz();
      fHits[10] = fSenseless; // Energy loss is sensless for T0+
      fHits[11] = fSenseless; // Track length is sensless for T0+
      fHits[12] = fSenseless; // Photon production for V0+

      if (TVirtualMC::GetMC()->TrackPid() == 50000050)
      { // If particles is photon then ...
        if (RegisterPhotoE(fHits[3]))
        {
          fIshunt = 2;
          AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, fHits);
          // Create a track reference at the exit of photocatode
        }
      }

      // charge particle HITS
      if (TVirtualMC::GetMC()->TrackCharge())
      {
        fIshunt = 0;
        AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, fHits);
        // charge particle TrackReference
        AddTrackReference(gAlice->GetMCApp()->GetCurrentTrackNumber(), AliTrackReference::kFIT);
      }
    } // trck entering
  }   // sensitive

  // V0+
  if (!gMC->TrackCharge() || !gMC->IsTrackAlive())
    return; // Only interested in charged and alive tracks

  // Check if there is a hit in any of the V0+ sensitive volumes defined in Init
  Bool_t IsId = kFALSE;

  for (Int_t iCell = 0; iCell < AliFV0::sNumberOfCells; iCell++)
  {
    if (id == fIdV0Plus[iCell])
    {
      IsId = kTRUE;
      break;
    }
  }

  if (IsId == kTRUE)
  { /// if the volume is sensitive

    // Defining V0+ ring numbers using the sensitive volumes

    Int_t RingNumber = -1; // This would change!! as no. of  scint Cell reduced.

    ////Rihan: Map for new Scintilator cells =>
    RingNumber = fFV0Det->getCurrentRingId(TVirtualMC::GetMC());

    /// this was the old map:

    if (RingNumber < 1)
    {
      std::cout << "\n\n  MC cell id = " << id << " This volume is not a V0+ Ring" << std::endl;
    }
    else if (RingNumber)
    {

      // Track entering
      if (TVirtualMC::GetMC()->IsTrackEntering())
      {
        TVirtualMC::GetMC()->TrackPosition(pos);
        TVirtualMC::GetMC()->TrackMomentum(mom);

        Float_t Pt = TMath::Sqrt(mom.Px() * mom.Px() + mom.Py() * mom.Py());
        fHits[0] = pos[0];
        fHits[1] = pos[1];
        fHits[2] = pos[2];

        Float_t etot = TVirtualMC::GetMC()->Etot();
        fHits[3] = etot;

        Int_t iPart = TVirtualMC::GetMC()->TrackPid();
        Int_t partID = TVirtualMC::GetMC()->IdFromPDG(iPart);
        fHits[4] = partID;

        Float_t ttime = TVirtualMC::GetMC()->TrackTime();
        fHits[5] = ttime * 1e12;
        fHits[6] = TVirtualMC::GetMC()->TrackCharge();
        fHits[7] = mom.Px();
        fHits[8] = mom.Py();
        fHits[9] = mom.Pz();
        fHits[10] = 0.;
      }
      else
      {
        fHits[10] += TVirtualMC::GetMC()->Edep();
      }

      if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
      {

        Float_t Tlength;
        Float_t EnergyDep = TVirtualMC::GetMC()->Edep();
        Float_t Step = TVirtualMC::GetMC()->TrackStep(); // Energy loss in the current step
        Int_t nPhotonsInStep = 0;
        Int_t nPhotons = 0;
        nPhotonsInStep = Int_t(EnergyDep / (fV0PlusLightYield * 1e-9));
        nPhotons = nPhotonsInStep - Int_t((Float_t(nPhotonsInStep) * fV0PlusLightAttenuation * fV0PlusnMeters));
        nPhotons = nPhotons - Int_t(Float_t(nPhotons) * fV0PlusFibToPhot);
        Tlength += Step;
        fHits[11] = Tlength;
        fHits[12] = nPhotons;

        vol[0] = fFV0Det->getCurrentCellId(TVirtualMC::GetMC());
        vol[1] = RingNumber;
        vol[2] = 2;

        fIshunt = 0;

        AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, fHits);
        AddTrackReference(gAlice->GetMCApp()->GetCurrentTrackNumber(), AliTrackReference::kFIT);
        Tlength = 0.0;
      } // Track exiting, stopped or disappeared track
    }   // Ring number
  }
}

//------------------------------------------------------------------------
Bool_t AliFITv9::RegisterPhotoE(Double_t energy)
{
  Float_t eff = fPMTeff->Eval(energy);
  Double_t p = gRandom->Rndm();

  if (p > eff)
  {
    return kFALSE;
  }
  return kTRUE;
}

//-------------------------------------------------------------------
void AliFITv9::DefineOpticalProperties()
{
  // Path of the optical properties input file
  TString optPropPath = "$(ALICE_ROOT)/FIT/sim/quartzOptProperties.txt";
  optPropPath = gSystem->ExpandPathName(optPropPath.Data()); // Expand $(ALICE_ROOT) into real system path

  // Optical properties definition.
  Int_t *idtmed = fIdtmed->GetArray();

  // Prepare pointers for arrays read from the input file
  Float_t *aPckov = NULL;
  Double_t *dPckov = NULL;
  Float_t *aAbsSiO2 = NULL;
  Float_t *rindexSiO2 = NULL;
  Float_t *qeff = NULL;
  Int_t kNbins = 0;
  ReadOptProperties(optPropPath.Data(), &aPckov, &dPckov, &aAbsSiO2, &rindexSiO2, &qeff, kNbins);
  fPMTeff = new TGraph(kNbins, aPckov, qeff); // set QE

  // Prepare pointers for arrays with constant and hardcoded values (independent of wavelength)
  Float_t *efficAll = NULL;
  Float_t *rindexAir = NULL;
  Float_t *absorAir = NULL;
  Float_t *rindexCathodeNext = NULL;
  Float_t *absorbCathodeNext = NULL;
  Double_t *efficMet = NULL;
  Double_t *aReflMet = NULL;
  FillOtherOptProperties(&efficAll, &rindexAir, &absorAir, &rindexCathodeNext, &absorbCathodeNext, &efficMet, &aReflMet, kNbins);

  TVirtualMC::GetMC()->SetCerenkov(idtmed[AliFT0::kOpGlass], kNbins, aPckov, aAbsSiO2, efficAll, rindexSiO2);
  TVirtualMC::GetMC()->SetCerenkov(idtmed[AliFT0::kOpGlassCathode], kNbins, aPckov, aAbsSiO2, efficAll, rindexSiO2);

  DeleteOptPropertiesArr(&aPckov, &dPckov, &aAbsSiO2, &rindexSiO2, &efficAll, &rindexAir, &absorAir, &rindexCathodeNext,
                         &absorbCathodeNext, &efficMet, &aReflMet);
}

//--------------------------------------------------------------------
Int_t AliFITv9::ReadOptProperties(const std::string filePath, Float_t **e, Double_t **de, Float_t **abs, Float_t **n, Float_t **qe, Int_t &kNbins) const
{
  std::ifstream infile;
  infile.open(filePath.c_str());

  // Check if file is opened correctly
  if (infile.fail() == true)
  {
    AliFatal(Form("Error opening ascii file: %s", filePath.c_str()));
    return -1;
  }

  std::string comment; // dummy, used just to read 4 first lines and move the
                       // cursor to the 5th, otherwise unused
  if (!getline(infile, comment))
  { // first comment line
    AliFatal(Form("Error opening ascii file (it is probably a folder!): %s", filePath.c_str()));
    return -2;
  }
  getline(infile, comment); // 2nd comment line

  // Get number of elements required for the array
  infile >> kNbins;
  if (kNbins < 0 || kNbins > 1e4)
  {
    AliFatal(
        Form("Input arraySize out of range 0..1e4: %i. Check input file: %s", kNbins, filePath.c_str()));
    return -4;
  }

  // Allocate memory required for arrays
  *e = new Float_t[kNbins];
  *de = new Double_t[kNbins];
  *abs = new Float_t[kNbins];
  *n = new Float_t[kNbins];
  *qe = new Float_t[kNbins];
  getline(infile, comment); // finish 3rd line after the kNbins are read
  getline(infile, comment); // 4th comment line - ignore

  // read the main body of the file (table of values: energy, absorption length,
  // refractive index and quantum efficiency)
  Int_t iLine = 0;
  std::string sLine;
  getline(infile, sLine);
  while (!infile.eof())
  {
    if (iLine >= kNbins)
    {
      AliFatal(Form("Line number: %i reaches range of declared arraySize: %i. Check input file: %s",
                    iLine, kNbins, filePath.c_str()));
      return -5;
    }
    std::stringstream ssLine(sLine);
    ssLine >> (*de)[iLine];
    (*de)[iLine] *= 1e-9;                             // Convert eV -> GeV immediately
    (*e)[iLine] = static_cast<Float_t>((*de)[iLine]); // same value, different precision
    ssLine >> (*abs)[iLine];
    ssLine >> (*n)[iLine];
    ssLine >> (*qe)[iLine];
    if (!(ssLine.good() || ssLine.eof()))
    { // check if there were problems with
      // numbers conversion
      AliFatal(Form("Error while reading line %i: %s", iLine, ssLine.str().c_str()));
      return -6;
    }
    getline(infile, sLine);
    iLine++;
  }
  if (iLine != kNbins)
  {
    AliFatal(Form("Total number of lines %i is different than declared %i. Check input file: %s",
                  iLine, kNbins, filePath.c_str()));
    return -7;
  }

  AliInfo(Form("Optical properties taken from the file: %s. Number of lines read: %i",
               filePath.c_str(), iLine));
  return 0;
}

//--------------------------------------------------------------------
void AliFITv9::FillOtherOptProperties(Float_t **efficAll, Float_t **rindexAir, Float_t **absorAir,
                                      Float_t **rindexCathodeNext, Float_t **absorbCathodeNext,
                                      Double_t **efficMet, Double_t **aReflMet, const Int_t kNbins) const
{
  // Allocate memory for these arrays according to the required size
  *efficAll = new Float_t[kNbins];
  *rindexAir = new Float_t[kNbins];
  *absorAir = new Float_t[kNbins];
  *rindexCathodeNext = new Float_t[kNbins];
  *absorbCathodeNext = new Float_t[kNbins];
  *efficMet = new Double_t[kNbins];
  *aReflMet = new Double_t[kNbins];

  // Set constant values to the arrays
  for (Int_t i = 0; i < kNbins; i++)
  {
    (*efficAll)[i] = 1.;
    (*rindexAir)[i] = 1.;
    (*absorAir)[i] = 0.3;
    (*rindexCathodeNext)[i] = 0;
    (*absorbCathodeNext)[i] = 0;
    (*efficMet)[i] = 0.;
    (*aReflMet)[i] = 1.;
  }
}

//--------------------------------------------------------------------
void AliFITv9::DeleteOptPropertiesArr(Float_t **e, Double_t **de, Float_t **abs, Float_t **n, Float_t **efficAll,
                                      Float_t **rindexAir, Float_t **absorAir, Float_t **rindexCathodeNext,
                                      Float_t **absorbCathodeNext, Double_t **efficMet, Double_t **aReflMet) const
{
  delete[](*e);
  delete[](*de);
  delete[](*abs);
  delete[](*n);
  delete[](*efficAll);
  delete[](*rindexAir);
  delete[](*absorAir);
  delete[](*rindexCathodeNext);
  delete[](*absorbCathodeNext);
  delete[](*efficMet);
  delete[](*aReflMet);
}
