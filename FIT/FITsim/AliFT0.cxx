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

#include "AliFT0.h"
#include <TSystem.h>
#include <TGeoVolume.h>
#include <TGeoCompositeShape.h>
#include <TGeoBBox.h>
#include <TGeoSphere.h>
#include <TGraph.h>
#include <TGeoMatrix.h>
#include <TVirtualMC.h>
#include "AliLog.h"
//#include <sstream>
#include <iostream>
#include <fstream>
//#include <string>

ClassImp(AliFT0);

AliFT0::AliFT0() : fDummy(0)
{

  Double_t crad = ZdetC; // define concave c-side radius here

  Double_t dP = mInStart[0]; // side length of mcp divided by 2

  // uniform angle between detector faces==
  Double_t btta = 2 * TMath::ATan(dP / crad);

  // get noncompensated translation data
  Double_t grdin[6] = {-3, -2, -1, 1, 2, 3};
  Double_t gridpoints[6];
  for (Int_t i = 0; i < 6; i++)
  {
    gridpoints[i] = crad * TMath::Sin((1 - 1 / (2 * TMath::Abs(grdin[i]))) * grdin[i] * btta);
  }
  Double_t xi[NCellsC] = {-15.038271418735729, 15.038271418735729,
                          -15.003757581112167, 15.003757581112167, -9.02690018974363,
                          9.02690018974363, -9.026897413747076, 9.026897413747076,
                          -9.026896531935773, 9.026896531935773, -3.0004568618531313,
                          3.0004568618531313, -3.0270795197907225, 3.0270795197907225,
                          3.0003978432927543, -3.0003978432927543, 3.0270569670429572,
                          -3.0270569670429572, 9.026750365564254, -9.026750365564254,
                          9.026837450695885, -9.026837450695885, 9.026849243816981,
                          -9.026849243816981, 15.038129472387304, -15.038129472387304,
                          15.003621961057961, -15.003621961057961};
  Double_t yi[NCellsC] = {3.1599494336464455, -3.1599494336464455,
                          9.165191680982874, -9.165191680982874, 3.1383331772537426,
                          -3.1383331772537426, 9.165226363918643, -9.165226363918643,
                          15.141616002932361, -15.141616002932361, 9.16517861649866,
                          -9.16517861649866, 15.188854859073416, -15.188854859073416,
                          9.165053319552113, -9.165053319552113, 15.188703787345304,
                          -15.188703787345304, 3.138263189805292, -3.138263189805292,
                          9.165104089644917, -9.165104089644917, 15.141494417823818,
                          -15.141494417823818, 3.1599158563428644, -3.1599158563428644,
                          9.165116302773846, -9.165116302773846};

  Double_t zi[NCellsC];
  for (Int_t i = 0; i < NCellsC; i++)
  {
    zi[i] = TMath::Sqrt(TMath::Power(crad, 2) - TMath::Power(xi[i], 2) - TMath::Power(yi[i], 2));
  }

  // get rotation data
  Double_t ac[NCellsC], bc[NCellsC], gc[NCellsC];
  for (Int_t i = 0; i < NCellsC; i++)
  {
    ac[i] = TMath::ATan(yi[i] / xi[i]) - TMath::Pi() / 2 + 2 * TMath::Pi();
    if (xi[i] < 0)
    {
      bc[i] = TMath::ACos(zi[i] / crad);
    }
    else
    {
      bc[i] = -1 * TMath::ACos(zi[i] / crad);
    }
  }
  Double_t xc2[NCellsC], yc2[NCellsC], zc2[NCellsC];

  // compensation based on node position within individual detector geometries
  // determine compensated radius
  Double_t rcomp = crad + mStartC[2] / 2.0; //
  for (Int_t i = 0; i < NCellsC; i++)
  {
    // Get compensated translation data
    xc2[i] = rcomp * TMath::Cos(ac[i] + TMath::Pi() / 2) * TMath::Sin(-1 * bc[i]);
    yc2[i] = rcomp * TMath::Sin(ac[i] + TMath::Pi() / 2) * TMath::Sin(-1 * bc[i]);
    zc2[i] = rcomp * TMath::Cos(bc[i]);

    // Convert angles to degrees
    ac[i] *= 180 / TMath::Pi();
    bc[i] *= 180 / TMath::Pi();
    gc[i] = -1 * ac[i];
    mAngles[i].SetXYZ(ac[i], bc[i], gc[i]);
    mPosModuleCx[i] = xc2[i];
    mPosModuleCy[i] = yc2[i];
    mPosModuleCz[i] = zc2[i] - 80; // !!! fix later
  }
}

void AliFT0::ConstructGeometry()
{
  AliDebug(2, "Creating FT0 geometry\n");

  TGeoVolumeAssembly *stlinA = new TGeoVolumeAssembly("FT0A"); // A side mother
  TGeoVolumeAssembly *stlinC = new TGeoVolumeAssembly("FT0C"); // C side mother

  AliInfo(Form("FT0: Building geometry. FT0_A volume name is '%s'", stlinA->GetName()));
  AliInfo(Form("FT0: Building geometry. FT0_C volume name is '%s'", stlinC->GetName()));

  // FIT interior
  TVirtualMC::GetMC()->Gsvolu("0MOD", "BOX", getMediumID(kAir), mInStart, 3);
  TGeoVolume *ins = gGeoManager->GetVolume("0MOD");
  //
  TGeoTranslation *tr[NCellsA + NCellsC];
  TString nameTr;
  // A side Translations
  for (Int_t itr = 0; itr < NCellsA; itr++)
  {
    nameTr = Form("0TR%i", itr + 1);
    float z = -mStartA[2] + mInStart[2];
    tr[itr] = new TGeoTranslation(nameTr.Data(), mPosModuleAx[itr], mPosModuleAy[itr], z);
    tr[itr]->RegisterYourself();
    stlinA->AddNode(ins, itr, tr[itr]);
    AliDebug(2, Form(" A geom %d %f %f", itr, mPosModuleAx[itr], mPosModuleAy[itr]));
  }
  SetCablesA(stlinA);

  // Add FT0-A support Structure to the geometry
  stlinA->AddNode(constructFrameAGeometry(), 1, new TGeoTranslation(0, 0, -mStartA[2] + mInStart[2]));

  // C Side
  TGeoRotation *rot[NCellsC];
  TString nameRot;
  TGeoCombiTrans *com[NCellsC];
  TGeoCombiTrans *comCable[NCellsC];
  TString nameCom;

  // Additional elements for the C-side frame
  TGeoCombiTrans *plateCom[NCellsC];
  TGeoMedium *Al = gGeoManager->GetMedium("FIT_Aluminium$");
  TGeoCompositeShape *plateCompositeShape = new TGeoCompositeShape("plateCompositeShape", cPlateShapeString().Data());
  TGeoVolume *plateVol = new TGeoVolume("plateVol", plateCompositeShape, Al);

  for (Int_t itr = NCellsA; itr < NCellsA + NCellsC; itr++)
  {
    nameTr = Form("0TR%i", itr + 1);
    nameRot = Form("0Rot%i", itr + 1);
    int ic = itr - NCellsA;
    float ac1 = tiltMCP(ic).X();
    float bc1 = tiltMCP(ic).Y();
    float gc1 = tiltMCP(ic).Z();
    rot[ic] = new TGeoRotation(nameRot.Data(), ac1, bc1, gc1);
    AliDebug(2, Form(" rot geom %d %f %f %f", ic, ac1, bc1, gc1));
    rot[ic]->RegisterYourself();
    com[ic] = new TGeoCombiTrans(mPosModuleCx[ic], mPosModuleCy[ic], mPosModuleCz[ic], rot[ic]);
    TGeoHMatrix hm = *com[ic];
    TGeoHMatrix *ph = new TGeoHMatrix(hm);
    stlinC->AddNode(ins, itr, ph);
    // cables
    TGeoVolume *cables = SetCablesSize(itr);
    AliDebug(2, Form(" C %f %f", mPosModuleCx[ic], mPosModuleCy[ic]));
    //    cables->Print();
    // Additional shift (+0.1) introduced to cable planes so they don't overlap the C-side frame
    comCable[ic] = new TGeoCombiTrans(mPosModuleCx[ic], mPosModuleCy[ic], mPosModuleCz[ic] + mInStart[2] + 0.2 + 0.1, rot[ic]);
    TGeoHMatrix hmCable = *comCable[ic];
    TGeoHMatrix *phCable = new TGeoHMatrix(hmCable);
    stlinC->AddNode(cables, itr, comCable[ic]);

    // C-side frame elements - module plates
    plateCom[ic] = new TGeoCombiTrans(mPosModuleCx[ic], mPosModuleCy[ic], (mPosModuleCz[ic] - 3), rot[ic]);
    TGeoHMatrix hmPlate = *plateCom[ic];
    TGeoHMatrix *phPlate = new TGeoHMatrix(hmPlate);
    stlinC->AddNode(plateVol, itr, phPlate);
  }
  // Add C-side frame
  std::cout << "Contructing frame C geometry\n";
  stlinC->AddNode(constructFrameCGeometry(), NCellsA + NCellsC + 1);

  TGeoVolume *alice = gGeoManager->GetVolume("ALIC");
  // Add A-side detector
  alice->AddNode(stlinA, 1, new TGeoTranslation(0, 0., ZdetA + 0.63)); // offset to avoid overlap with FV0

  // Add C-side detector
  TGeoRotation *rotC = new TGeoRotation("rotC", 90., 0., 90., 90., 180., 0.);
  alice->AddNode(stlinC, 1, new TGeoCombiTrans(0., 0., -ZdetC, rotC));

  // MCP + 4 x wrapped radiator + 4xphotocathod + MCP + Al top in front of radiators
  SetOneMCP(ins);
  // SetCablesC(stlinC);
}

//_________________________________________
void AliFT0::SetOneMCP(TGeoVolume *ins)
{

  Double_t x, y, z;

  Float_t ptop[3] = {1.324, 1.324, 1.};      // Cherenkov radiator
  Float_t ptopref[3] = {1.3241, 1.3241, 1.}; // Cherenkov radiator wrapped with reflector
  Double_t prfv[3] = {0.0002, 1.323, 1.};    // Vertical refracting layer bettwen radiators and between radiator and not optical Air
  Double_t prfh[3] = {1.323, 0.0002, 1.};    // Horizontal refracting layer bettwen radiators and ...
  Float_t pmcp[3] = {2.949, 2.949, 0.66};    // MCP
  Float_t pmcpinner[3] = {2.749, 2.749, 0.1};
  Float_t pmcpbase[3] = {2.949, 2.949, 0.675};
  Float_t pmcpside[3] = {0.15, 2.949, 0.65};
  Float_t pmcptopglass[3] = {2.949, 2.949, 0.1}; // MCP top glass optical
  Float_t preg[3] = {1.324, 1.324, 0.005};       // Photcathode
  // Entry window (glass)
  TVirtualMC::GetMC()->Gsvolu("0TOP", "BOX", getMediumID(kOpGlass), ptop, 3); // Glass radiator
  TGeoVolume *top = gGeoManager->GetVolume("0TOP");
  TVirtualMC::GetMC()->Gsvolu("0TRE", "BOX", getMediumID(kAir), ptopref, 3); // Air: wrapped  radiator
  TGeoVolume *topref = gGeoManager->GetVolume("0TRE");
  TVirtualMC::GetMC()->Gsvolu("0RFV", "BOX", getMediumID(kOptAl), prfv, 3); // Optical Air vertical
  TGeoVolume *rfv = gGeoManager->GetVolume("0RFV");
  TVirtualMC::GetMC()->Gsvolu("0RFH", "BOX", getMediumID(kOptAl), prfh, 3); // Optical Air horizontal
  TGeoVolume *rfh = gGeoManager->GetVolume("0RFH");

  TVirtualMC::GetMC()->Gsvolu("0REG", "BOX", getMediumID(kOpGlassCathode), preg, 3);
  TGeoVolume *cat = gGeoManager->GetVolume("0REG");

  // wrapped radiator +  reflecting layers

  Int_t ntops = 0, nrfvs = 0, nrfhs = 0;
  x = y = z = 0;
  topref->AddNode(top, 1, new TGeoTranslation(0, 0, 0));
  float xinv = -ptop[0] - prfv[0];
  topref->AddNode(rfv, 1, new TGeoTranslation(xinv, 0, 0));
  xinv = ptop[0] + prfv[0];
  topref->AddNode(rfv, 2, new TGeoTranslation(xinv, 0, 0));
  float yinv = -ptop[1] - prfh[1];
  topref->AddNode(rfh, 1, new TGeoTranslation(0, yinv, 0));
  yinv = ptop[1] + prfh[1];
  topref->AddNode(rfh, 2, new TGeoTranslation(0, yinv, 0));

  // container for radiator, cathode
  for (Int_t ix = 0; ix < 2; ix++)
  {
    float xin = -mInStart[0] + 0.3 + (ix + 0.5) * 2 * ptopref[0];
    for (Int_t iy = 0; iy < 2; iy++)
    {
      float yin = -mInStart[1] + 0.3 + (iy + 0.5) * 2 * ptopref[1];
      ntops++;
      z = -mInStart[2] + ptopref[2];
      ins->AddNode(topref, ntops, new TGeoTranslation(xin, yin, z));
      AliDebug(2, Form(" n %d x %f y %f z radiator %f", ntops, xin, yin, z));
      z += ptopref[2] + 2. * pmcptopglass[2] + preg[2];
      ins->AddNode(cat, ntops, new TGeoTranslation(xin, yin, z));
      AliDebug(2, Form(" n %d x %f y %f z radiator %f", ntops, xin, yin, z));
    }
  }
  // MCP
  TVirtualMC::GetMC()->Gsvolu("0MTO", "BOX", getMediumID(kOpGlass), pmcptopglass, 3); // Op  Glass
  TGeoVolume *mcptop = gGeoManager->GetVolume("0MTO");
  z = -mInStart[2] + 2 * ptopref[2] + pmcptopglass[2];
  ins->AddNode(mcptop, 1, new TGeoTranslation(0, 0, z));

  TVirtualMC::GetMC()->Gsvolu("0MCP", "BOX", getMediumID(kAir), pmcp, 3); // glass
  TGeoVolume *mcp = gGeoManager->GetVolume("0MCP");
  z = -mInStart[2] + 2 * ptopref[2] + 2 * pmcptopglass[2] + 2 * preg[2] + pmcp[2];
  ins->AddNode(mcp, 1, new TGeoTranslation(0, 0, z));

  TVirtualMC::GetMC()->Gsvolu("0MSI", "BOX", getMediumID(kMCPwalls), pmcpside, 3); // glass
  TGeoVolume *mcpside = gGeoManager->GetVolume("0MSI");
  x = -pmcp[0] + pmcpside[0];
  y = -pmcp[1] + pmcpside[1];
  mcp->AddNode(mcpside, 1, new TGeoTranslation(x, y, 0));
  x = pmcp[0] - pmcpside[0];
  y = pmcp[1] - pmcpside[1];
  mcp->AddNode(mcpside, 2, new TGeoTranslation(x, y, 0));
  x = -pmcp[1] + pmcpside[1];
  y = -pmcp[0] + pmcpside[0];
  mcp->AddNode(mcpside, 3, new TGeoCombiTrans(x, y, 0, new TGeoRotation("R2", 90, 0, 0)));
  x = pmcp[1] - pmcpside[1];
  y = pmcp[0] - pmcpside[0];
  mcp->AddNode(mcpside, 4, new TGeoCombiTrans(x, y, 0, new TGeoRotation("R2", 90, 0, 0)));

  TVirtualMC::GetMC()->Gsvolu("0MBA", "BOX", getMediumID(kCeramic), pmcpbase, 3); // glass
  TGeoVolume *mcpbase = gGeoManager->GetVolume("0MBA");
  z = -mInStart[2] + 2 * ptopref[2] + pmcptopglass[2] + 2 * pmcp[2] + pmcpbase[2];
  ins->AddNode(mcpbase, 1, new TGeoTranslation(0, 0, z));
}

//----------------------------------
void AliFT0::SetCablesA(TGeoVolume *stl)
{

  float pcableplane[3] = {20, 20, 0.25}; //

  TVirtualMC::GetMC()->Gsvolu("0CAA", "BOX", getMediumID(kAir), pcableplane, 3); // container for cables
  TGeoVolume *cableplane = gGeoManager->GetVolume("0CAA");
  //  float zcableplane = -mStartA[2] + 2 * mInStart[2] + pcableplane[2];
  int na = 0;
  double xcell[24], ycell[24];
  for (int imcp = 0; imcp < 24; imcp++)
  {
    xcell[na] = mPosModuleAx[imcp];
    ycell[na] = mPosModuleAy[imcp];
    TGeoVolume *vol = SetCablesSize(imcp);
    cableplane->AddNode(vol, na, new TGeoTranslation(xcell[na], ycell[na], 0));
    na++;
  }

  // 12 cables extending beyond the frame
  Float_t pcablesextend[3] = {2, 15, 0.245};
  Float_t pcablesextendsmall[3] = {3, 2, 0.245};
  Float_t *ppcablesextend[] = {pcablesextend, pcablesextend, pcablesextendsmall, pcablesextendsmall};
  // left side
  double xcell_side[] = {-mStartA[0] + pcablesextend[0], mStartA[0] - pcablesextend[0], 0, 0};
  double ycell_side[] = {0, 0, -mStartA[1] + pcablesextendsmall[1], mStartA[1] - pcablesextendsmall[1]};

  for (int icab = 0; icab < 4; icab++)
  {
    const std::string volName = Form("CAB%2.i", 52 + icab);
    TVirtualMC::GetMC()->Gsvolu(volName.c_str(), " BOX", getMediumID(kCable), ppcablesextend[icab], 3); // cables
    TGeoVolume *vol = gGeoManager->GetVolume(volName.c_str());
    cableplane->AddNode(vol, 1, new TGeoTranslation(xcell_side[icab], ycell_side[icab], 0));
  }
  float zcableplane = mStartA[2] - pcableplane[2] - 3;
  stl->AddNode(cableplane, 1, new TGeoTranslation(0, 0, zcableplane));
}
//------------------------------------------

TGeoVolume *AliFT0::SetCablesSize(int mod)
{
  int na = 0;

  int mcpcables[52] = {2, 1, 2, 1, 2,
                       2, 1, 1, 1, 2,
                       2, 1, 1, 2,
                       2, 1, 1, 1, 2,
                       2, 1, 2, 1, 2,
                       2, 2, 3, 3, 1,
                       1, 2, 2, 2, 2,
                       1, 1, 1, 1, 1,
                       1, 1, 1, 1, 1,
                       2, 2, 2, 2, 2,
                       2, 3, 3};

  // cable D=0.257cm, Weight: 13 lbs/1000ft = 0.197g/cm; 1 piece 0.65cm
  // 1st 8 pieces - tube  8*0.65cm = 5.2cm; V = 0.0531cm2 -> box {0.27*0.27*1}cm; W = 0.66g
  // 2nd 24 pieces 24*0.65cm; V = 0.76 -> {0.44, 0.447 1}; W = 3.07g
  // 3d  48  pieces  48*0.65cm;  V = 1.53cm^3; ->box {0.66, 0.66, 1.}; W= 6.14g
  double xcell[NCellsC], ycell[NCellsC], zcell[NCellsC];
  float xsize[3] = {1.8, 1.8, 2.6}; //
  float ysize[3] = {0.6, 1.7, 2.};
  float zsize[3] = {0.1, 0.1, 0.1};

  int ic = mcpcables[mod];
  float calblesize[3];
  calblesize[0] = xsize[ic - 1];
  calblesize[1] = ysize[ic - 1];
  calblesize[2] = zsize[ic - 1];
  const std::string volName = Form("CAB%2.i", mod);
  TVirtualMC::GetMC()->Gsvolu(volName.c_str(), "BOX", getMediumID(kCable), calblesize, 3); // cables
  TGeoVolume *vol = gGeoManager->GetVolume(volName.c_str());
  AliDebug(2, Form("C cables %d %s %d", mod, volName.c_str(), ic));
  return vol;
}

void AliFT0::addAlignableVolumes() const
{
  //
  // Creates entries for alignable volumes associating the symbolic volume
  // name with the corresponding volume path.
  //
  //  First version (mainly ported from AliRoot)
  //

  AliInfo("Add FT0 alignable volumes");

  if (!gGeoManager)
  {
    AliFatal("TGeoManager doesn't exist !");
    return;
  }

  TString volPath = Form("/ALIC_1");
  // set A side
  TString volPathA = volPath + Form("/FT0A_1");
  TString symNameA = "FT0A";
  AliInfo(Form("%s <-> %s", symNameA.Data(), volPathA.Data()));
  if (!gGeoManager->SetAlignableEntry(symNameA.Data(), volPathA.Data()))
  {
    AliFatal(Form("Unable to set alignable entry ! %s : %s", symNameA.Data(), volPathA.Data()));
  }
  // set C side
  TString volPathC = volPath + Form("/FT0C_1");
  TString symNameC = "FT0C";
  AliInfo(Form("%s <-> %s", symNameC.Data(), volPathC.Data()));
  if (!gGeoManager->SetAlignableEntry(symNameC.Data(), volPathC.Data()))
  {
    AliFatal(Form("Alignable entry %s not created. Volume path %s not valid", symNameA.Data(), volPathA.Data()));
  }
  TString volPathMod, symNameMod;
  for (Int_t imod = 0; imod < NCellsA + NCellsC; imod++)
  {
    TString volPath = (imod < NCellsA) ? volPathA : volPathC;
    volPathMod = volPath + Form("/0MOD_%d", imod);
    symNameMod = Form("0MOD_%d", imod);
    if (!gGeoManager->SetAlignableEntry(symNameMod.Data(), volPathMod.Data()))
    {
      AliFatal(Form("Alignable entry %s not created. Volume path %s not valid", symNameMod.Data(), volPathMod.Data()));
    }
  }
}

// Construction of FT0-A support structure
// The frame is constructed by first building a block of Aluminum as a mother volume from which details can
// be subtracted. First, 6 boxe shapes are subtracted from around the edges and 2 from the center of the frame
// to create the fin shapes at the edges of the detector and the cross shape at the center of the detector.
// These boxe shapes are then subtracted again but from positions reflected in both x and y which is
// reflects the symmetry of the frame. Then a loop is used to subtract out the PMT sockets. In the loop,
//  after a socket is subtracted,
// either an inner or an outer plate group is placed inside of it. Inner and outer plate groups are
// both composed of a cover plate and a cable plate with fiber heads subtracted from the cable plate.
// The orientation of these holes differs between the inner and outer plates making them distinct.
// Contributors: Joe Crowley (2019-20), Jason Pruitt (2020-21), Sam Christensen (2021-22),
// and Jennifer Klay (2019-22) from Cal Poly SLO.
TGeoVolume *AliFT0::constructFrameAGeometry()
{
  // define the media
  TGeoMedium *Vacuum = gGeoManager->GetMedium("FIT_Vacuum$");
  TGeoMedium *Al = gGeoManager->GetMedium("FIT_Aluminium$");

  // make a volume assembly for the frame
  TGeoVolumeAssembly *FT0_Frame = new TGeoVolumeAssembly("FT0_Frame");

  // Define the block of aluminum that forms the frame
  Double_t blockdX = 37.1;  // slightly larger in x
  Double_t blockdY = 36.85; // than y
  Double_t blockdZ = 6.95;  // thickness of frame and back plates
  TGeoBBox *block = new TGeoBBox("block", blockdX / 2, blockdY / 2, blockdZ / 2);

  // To form the outer frame shape with fins that bolt it to the FV0, remove
  // aluminum in six chunks (boxes) from two sides, then reflect these to remove
  // from the other sides.  As viewed from the back side of the detector, count
  // clockwise from bottom left for numbering.
  Double_t box1dX = 1.57;                          // narrower
  Double_t box1dY = 6.55;                          // than it is tall
  Double_t box1PosX = -(blockdX / 2 - box1dX / 2); // placement on the frame block
  Double_t box1PosY = 0;                           // starts at the middle
  TGeoBBox *box1 = new TGeoBBox("box1", box1dX / 2, box1dY / 2, blockdZ / 2);
  TGeoTranslation *box1Tr1 = new TGeoTranslation("box1Tr1", box1PosX, box1PosY, 0);
  box1Tr1->RegisterYourself();
  TGeoTranslation *box1Tr2 = new TGeoTranslation("box1Tr2", -box1PosX, -box1PosY, 0);
  box1Tr2->RegisterYourself();

  Double_t box2dX = 2.9;
  Double_t box2dY = 15.1;
  Double_t box2PosX = -(blockdX / 2 - box2dX / 2);
  Double_t box2PosY = blockdY / 2 - box2dY / 2;
  TGeoBBox *box2 = new TGeoBBox("box2", box2dX / 2, box2dY / 2, blockdZ / 2);
  TGeoTranslation *box2Tr1 = new TGeoTranslation("box2Tr1", box2PosX, box2PosY, 0);
  box2Tr1->RegisterYourself();
  TGeoTranslation *box2Tr2 = new TGeoTranslation("box2Tr2", -box2PosX, -box2PosY, 0);
  box2Tr2->RegisterYourself();

  // Box 3 is shallower than the others to preserve the aluminum fin where the
  // FT0 is bolted to the FV0
  Double_t box3dX = 12.7;
  Double_t box3dY = 3;
  Double_t box3dZ = 5.45;
  Double_t box3PosX = -(blockdX / 2 - box2dX - box3dZ / 2);
  Double_t box3PosY = blockdY / 2 - box3dY / 2;
  Double_t box3PosZ = blockdZ / 2 - box3dZ / 2; // subtract from the back, leaving fin on the front
  TGeoBBox *box3 = new TGeoBBox("box3", box3dX / 2, box3dY / 2, box3dZ / 2);
  TGeoTranslation *box3Tr1 = new TGeoTranslation("box3Tr1", box3PosX, box3PosY, box3PosZ);
  box3Tr1->RegisterYourself();
  TGeoTranslation *box3Tr2 = new TGeoTranslation("box3Tr2", -box3PosX, -box3PosY, box3PosZ);
  box3Tr2->RegisterYourself();

  Double_t box4dX = 6.6;
  Double_t box4dY = 1.67;
  Double_t box4PosX = 0;
  Double_t box4PosY = blockdY / 2 - box4dY / 2;
  TGeoBBox *box4 = new TGeoBBox("box4", box4dX / 2, box4dY / 2, blockdZ / 2);
  TGeoTranslation *box4Tr1 = new TGeoTranslation("box4Tr1", box4PosX, box4PosY, 0);
  box4Tr1->RegisterYourself();
  TGeoTranslation *box4Tr2 = new TGeoTranslation("box4Tr2", -box4PosX, -box4PosY, 0);
  box4Tr2->RegisterYourself();

  Double_t box5dX = 15;
  Double_t box5dY = 3;
  Double_t box5PosX = blockdX / 2 - box5dX / 2;
  Double_t box5PosY = blockdY / 2 - box5dY / 2;
  TGeoBBox *box5 = new TGeoBBox("box5", box5dX / 2, box5dY / 2, blockdZ / 2);
  TGeoTranslation *box5Tr1 = new TGeoTranslation("box5Tr1", box5PosX, box5PosY, 0);
  box5Tr1->RegisterYourself();
  TGeoTranslation *box5Tr2 = new TGeoTranslation("box5Tr2", -box5PosX, -box5PosY, 0);
  box5Tr2->RegisterYourself();

  // Similar to box 3, box 6 is shallower in z to leave aluminum for the fin that
  // bolts FT0 to FV0
  Double_t box6dX = 2.9;
  Double_t box6dY = 12.2;
  Double_t box6dZ = 5.45;
  Double_t box6PosX = blockdX / 2 - box6dX / 2;
  Double_t box6PosY = blockdY / 2 - box5dY - box6dY / 2;
  Double_t box6PosZ = blockdZ / 2 - box6dZ / 2; // subtract from the back, leaving fin at the front
  TGeoBBox *box6 = new TGeoBBox("box6", box6dX / 2, box6dY / 2, box6dZ / 2);
  TGeoTranslation *box6Tr1 = new TGeoTranslation("box6Tr1", box6PosX, box6PosY, box6PosZ);
  box6Tr1->RegisterYourself();
  TGeoTranslation *box6Tr2 = new TGeoTranslation("box6Tr2", -box6PosX, -box6PosY, box6PosZ);
  box6Tr2->RegisterYourself();

  // The central hole that accommodates the beam pipe is not the same on all four sides
  // so we define two rectangular boxes - one vertical and one horizontal - and copy/rotate them
  // to remove the aluminum in a "+" shape at the center
  //  cbox is a central rectangle
  Double_t cbox1dX = 7.175; // horizontal center box
  Double_t cbox1dY = 5.5;
  Double_t cbox1Xoffset = 14.425;
  Double_t cbox1PosX = -(blockdX / 2 - cbox1Xoffset - cbox1dX / 2);
  Double_t cbox1PosY = 0;
  TGeoBBox *cbox1 = new TGeoBBox("cbox1", cbox1dX / 2, cbox1dY / 2, blockdZ / 2);
  TGeoTranslation *cbox1Tr1 = new TGeoTranslation("cbox1Tr1", cbox1PosX, cbox1PosY, 0);
  cbox1Tr1->RegisterYourself();
  TGeoTranslation *cbox1Tr2 = new TGeoTranslation("cbox1Tr2", -cbox1PosX, -cbox1PosY, 0);
  cbox1Tr2->RegisterYourself();

  Double_t cbox2dX = 5.75; // vertical center box

  Double_t cbox2dY = 6.575;
  Double_t cbox2Yoffset = 14.425;
  Double_t cbox2PosX = 0;
  Double_t cbox2PosY = blockdY / 2 - cbox2Yoffset - cbox2dY / 2;
  TGeoBBox *cbox2 = new TGeoBBox("cbox2", cbox2dX / 2, cbox2dY / 2, blockdZ / 2);
  TGeoTranslation *cbox2Tr1 = new TGeoTranslation("cbox2Tr1", cbox2PosX, cbox2PosY, 0);
  cbox2Tr1->RegisterYourself();
  TGeoTranslation *cbox2Tr2 = new TGeoTranslation("cbox2Tr2", -cbox2PosX, -cbox2PosY, 0);
  cbox2Tr2->RegisterYourself();

  // The two L-shaped pieces that form the frame have a small 1mm gap between them,
  // where they come together.  As viewed from the back, the gaps are on the upper
  // left and lower right, so that for the center column of modules, the upper two
  // are shifted slightly to the right (as viewed from the back) and the lower two
  // are shifted slightly to the left (as viewed from the back)
  Double_t gapBoxdX = 0.1;
  Double_t gapBoxdY = blockdY / 2;
  Double_t gapPosX = -(sPmtSide / 2 + sEps + gapBoxdX / 2);
  Double_t gapPosY = blockdY / 4;
  TGeoBBox *gapBox = new TGeoBBox("gapBox", gapBoxdX / 2, gapBoxdY / 2, blockdZ / 2);
  TGeoTranslation *gapBoxTr1 = new TGeoTranslation("gapBoxTr1", gapPosX, gapPosY, 0);
  gapBoxTr1->RegisterYourself();
  TGeoTranslation *gapBoxTr2 = new TGeoTranslation("gapBoxTr2", -gapPosX, -gapPosY, 0);
  gapBoxTr2->RegisterYourself();

  // Create a string to define the complete frame object shape
  // Start from the aluminum block then subtract the boxes
  std::string frameACompositeString = "block ";
  frameACompositeString += "- box1:box1Tr1 - box1:box1Tr2 ";
  frameACompositeString += "- box2:box2Tr1 - box2:box2Tr2 ";
  frameACompositeString += "- box3:box3Tr1 - box3:box3Tr2 ";
  frameACompositeString += "- box4:box4Tr1 - box4:box4Tr2 ";
  frameACompositeString += "- box5:box5Tr1 - box5:box5Tr2 ";
  frameACompositeString += "- box6:box6Tr1 - box6:box6Tr2 ";
  frameACompositeString += "- cbox1:cbox1Tr1 - cbox1:cbox1Tr2 ";
  frameACompositeString += "- cbox2:cbox2Tr1 - cbox2:cbox2Tr2 ";
  frameACompositeString += "- gapBox:gapBoxTr1 - gapBox:gapBoxTr2";

  // The next section defines the objects that form the sockets in the
  // frame for the sensitive elements and the individual cover plates
  // at the front of the detector which include the optical fiber
  // heads that permit the LED pulser light to reach the quartz radiator
  // surfaces of each module

  // There are two fiber head configurations, called "inner" and "outer"
  // with different locations and angles of the fiber heads.
  Double_t coverPlatedZ = 0.2; // top cover thickness
  Double_t fiberPlatedZ = 0.5; // fiberhead plate is underneath

  // Each fiber is guided to a small rectangular opening in the plate
  Double_t opticalFiberHeaddY = 0.52;  // narrow side
  Double_t opticalFiberHeaddX = 1.142; // long side

  // The "top" two fiber heads are positioned at slightly different
  // locations than the bottom two, which are also rotated

  //"Outer" fiberhead placements
  Double_t fh1TopPosX = -1.6;
  Double_t fh1TopPosY = 1.325;
  Double_t fh1BotPosX = 1.555;
  Double_t fh1BotPosY = 1.249;
  Double_t fh1BotAngle = 16; // degrees

  //"Inner" fiberhead placements
  // All of these are placed at an angle
  Double_t fh2TopPosX = -1.563;
  Double_t fh2TopPosY = 1.4625;
  Double_t fh2TopAngle = 60;

  Double_t fh2BotPosX = 1.084;
  Double_t fh2BotPosY = 1.186;
  Double_t fh2BotAngle = -30;

  // Define cover plate, fiber plate, and optical Fiber Head shapes
  TGeoBBox *coverPlate = new TGeoBBox("coverPlate", sPmtSide / 2 + sEps, sPmtSide / 2 + sEps, coverPlatedZ / 2);
  TGeoBBox *fiberPlate = new TGeoBBox("fiberPlate", sPmtSide / 2 + sEps, sPmtSide / 2 + sEps, fiberPlatedZ / 2);
  TGeoBBox *opticalFiberHead = new TGeoBBox("opticalFiberHead", opticalFiberHeaddX / 2, opticalFiberHeaddY / 2, fiberPlatedZ / 2);

  // Define transformations of optical fiber heads for outer plate
  TGeoTranslation *coverPlateTr = new TGeoTranslation("coverPlateTr", 0, 0, fiberPlatedZ / 2 + coverPlatedZ / 2);
  coverPlateTr->RegisterYourself();
  TGeoTranslation *fh1TopTr1 = new TGeoTranslation("fh1TopTr1", fh1TopPosX, fh1TopPosY, 0);
  fh1TopTr1->RegisterYourself();
  TGeoTranslation *fh1TopTr2 = new TGeoTranslation("fh1TopTr2", fh1TopPosX, -fh1TopPosY, 0);
  fh1TopTr2->RegisterYourself();
  TGeoCombiTrans *fh1BotTr1 = new TGeoCombiTrans("fh1BotTr1", fh1BotPosX, fh1BotPosY, 0, new TGeoRotation("fh1BotRot1", fh1BotAngle, 0, 0));
  fh1BotTr1->RegisterYourself();
  TGeoCombiTrans *fh1BotTr2 = new TGeoCombiTrans("fh1BotTr2", fh1BotPosX, -fh1BotPosY, 0, new TGeoRotation("fh1BotRot2", -fh1BotAngle, 0, 0));
  fh1BotTr2->RegisterYourself();
  TGeoCombiTrans *fh2TopTr1 = new TGeoCombiTrans("fh2TopTr1", fh2TopPosX, fh2TopPosY, 0, new TGeoRotation("fh2TopRot1", fh2TopAngle + 90, 0, 0));
  fh2TopTr1->RegisterYourself();
  TGeoCombiTrans *fh2TopTr2 = new TGeoCombiTrans("fh2TopTr2", fh2TopPosX, -fh2TopPosY, 0, new TGeoRotation("fh2TopRot2", -fh2TopAngle - 90, 0, 0));
  fh2TopTr2->RegisterYourself();
  TGeoCombiTrans *fh2BotTr1 = new TGeoCombiTrans("fh2BotTr1", fh2BotPosX, fh2BotPosY, 0, new TGeoRotation("fh2BotRot1", -fh2BotAngle, 0, 0));
  fh2BotTr1->RegisterYourself();
  TGeoCombiTrans *fh2BotTr2 = new TGeoCombiTrans("fh2BotTr2", fh2BotPosX, -fh2BotPosY, 0, new TGeoRotation("fh2BotRot2", fh2BotAngle, 0, 0));
  fh2BotTr2->RegisterYourself();

  // Create a string that defines the plate group for the outer plates
  std::string outerPlateGroupString = "fiberPlate ";
  outerPlateGroupString += "- opticalFiberHead:fh1TopTr1 ";
  outerPlateGroupString += "- opticalFiberHead:fh1TopTr2 ";
  outerPlateGroupString += "- opticalFiberHead:fh1BotTr1 ";
  outerPlateGroupString += "- opticalFiberHead:fh1BotTr2 ";
  outerPlateGroupString += "+ coverPlate:coverPlateTr";

  // Create the composite shape for the outer plates
  TGeoCompositeShape *outerPlateGroup = new TGeoCompositeShape("outerPlateGroup", outerPlateGroupString.c_str());

  // Create a string that defines the plate group for the inner plates
  std::string innerPlateGroupString = "fiberPlate ";
  innerPlateGroupString += "- opticalFiberHead:fh2TopTr1 ";
  innerPlateGroupString += "- opticalFiberHead:fh2TopTr2 ";
  innerPlateGroupString += "- opticalFiberHead:fh2BotTr1 ";
  innerPlateGroupString += "- opticalFiberHead:fh2BotTr2 ";
  innerPlateGroupString += "+ coverPlate:coverPlateTr";

  // Create the composite shape for the inner plates
  TGeoCompositeShape *innerPlateGroup = new TGeoCompositeShape("innerPlateGroup", innerPlateGroupString.c_str());

  // The sockets that are cut out of the aluminum block for the senitive elements
  // to fit into are offset slightly in z to leave a thin plate of aluminum at the
  // back - the back plate covers
  Double_t backPlanedZ = 0.25;
  Double_t socketdZ = blockdZ - backPlanedZ;

  // Define the socket volume as a box of vacuum
  TGeoVolume *socket = gGeoManager->MakeBox("Socket", Vacuum, sPmtSide / 2 + sEps, sPmtSide / 2 + sEps, socketdZ / 2);

  // Define the orientation angles of the plate groups that will cover
  // the sockets holding the sensitive elements
  Double_t rotAngle[NCellsA] = {0, 0, -90, -90, -90, 0, 0, -90, -90, -90, 0, 0, 180, 180, 90, 90, 90, 180, 180, 90, 90, 90, 180, 180};
  // Define the socket and plate group translations
  TGeoTranslation *trSocket[NCellsA];
  TString nameTrSocket;
  TGeoCombiTrans *trPlateGroup[NCellsA];
  TString nameTrPlateGroup;
  TString namePGRot;

  // Loop over the number of modules, subtracting the sockets and adding back in the
  // plate groups at the position of each module
  for (Int_t itr = 0; itr < NCellsA; itr++)
  {

    nameTrSocket = Form("trSocket%i", itr + 1);
    float z = -backPlanedZ / 4.0;
    trSocket[itr] = new TGeoTranslation(nameTrSocket.Data(), mPosModuleAx[itr], mPosModuleAy[itr], z);
    trSocket[itr]->RegisterYourself();
    frameACompositeString += "- Socket:";         // subtract it from the aluminum block
    frameACompositeString += nameTrSocket.Data(); // at its corresponding location

    nameTrPlateGroup = Form("trPlateGroup%i", itr + 1);
    namePGRot = Form("pgRot%i", itr + 1);
    float z2 = -blockdZ / 2 + (coverPlatedZ + fiberPlatedZ) / 2;
    trPlateGroup[itr] = new TGeoCombiTrans(nameTrPlateGroup.Data(), mPosModuleAx[itr], mPosModuleAy[itr], z2, new TGeoRotation(namePGRot.Data(), rotAngle[itr], 0, 0));
    trPlateGroup[itr]->RegisterYourself();

    if (itr == 0 || itr == 2 || itr == 3 || itr == 4 || itr == 5 || itr == 10 || itr == 13 || itr == 18 || itr == 19 || itr == 20 || itr == 21 || itr == 23)
    {
      frameACompositeString += " + outerPlateGroup:"; // add the outer plate group back on to these modules
      frameACompositeString += nameTrPlateGroup.Data();
      frameACompositeString += " ";
    }
    else
    {
      frameACompositeString += " + innerPlateGroup:"; // or add the inner plate group back on to all other modules
      frameACompositeString += nameTrPlateGroup.Data();
      frameACompositeString += " ";
    }
  }

  // Finally, define the A side frame object from the complete composite shape defined above
  TGeoVolume *frameA = new TGeoVolume("frameA", new TGeoCompositeShape("frameA", frameACompositeString.c_str()), Al);

  // Add the frame object to the mother volume
  FT0_Frame->AddNode(frameA, 1);

  return FT0_Frame;
}

// C-side Support Structure
// This code was written by Jason Pruitt and Sam Christensen of Cal Poly in 2021
// They followed a similar method as for the A-side frame but had to account for the
// spherical geometry of the C-side.
TGeoVolume *AliFT0::constructFrameCGeometry()
{
  // define the media
  TGeoMedium *Vacuum = gGeoManager->GetMedium("FIT_Vacuum$");
  TGeoMedium *Al = gGeoManager->GetMedium("FIT_Aluminium$");
  const Double_t sFrameZC = 5.5632;
  const Double_t frameHeightC = 2.5; // pinstart[2]  or l_s

  // quartz & PMT C-side transformations
  const Double_t sensShift = 0.5;
  const Double_t sQuartzRadiatorZC = 1.94360;                              // Dimension variable (l_{q}
  const Double_t sQuartzHeightC = (-sFrameZC / 2 + sQuartzRadiatorZC / 2); // placement variable )
  const Double_t sPmtZC = 3.600;                                           // Dimension variable (l_{p}
  const Double_t sPmtHeightC = (sFrameZC / 2 - sPmtZC / 2);                // placement variable

  Double_t crad = 82.;
  Float_t sweep = 3.5 * 2;
  Float_t rMin = 81.9791;
  Float_t rMax = rMin + sFrameZC;
  Float_t tMin = 0;
  Float_t tMax = 35;
  Float_t pMin = 0;
  Float_t pMax = 180;
  Float_t pinstart[3] = {2.9491, 2.9491, 2.5};
  Float_t pstartC[3] = {20., 20, 5};

  Float_t multCorn = 1.275; // multiplication factor for corners
  Double_t xCorn = multCorn * (-14.75272569);
  Double_t yCorn = multCorn * (14.9043284);
  Double_t zCorn = 79.27306024;

  Double_t xCorn2 = -xCorn;
  Double_t yCorn2 = yCorn;
  Double_t zCorn2 = zCorn;

  Double_t acCorn = TMath::ATan(yCorn / xCorn) - TMath::Pi() / 2 + 2 * TMath::Pi();
  Double_t bcCorn = /*(-1)**/ TMath::ACos(zCorn / crad);
  Double_t gcCorn = -1 * acCorn;

  // holepunch corners not implemented for quartzRadiatorSeat, rounded corners are
  // in place for PMT
  Double_t flopsErr = 0.00001;
  Double_t exag = 5;

  // highest overlap values
  Double_t errPMTZ = 10 * sEps;
  Double_t errPMTXY = 0.02;
  Double_t errQrdZ = 0.143 + 0.22;
  Double_t errQrdXY = 0.35;

  Float_t backPlateZ = 0.5;

  // sphere1 is the spherical shell that will be subtracted
  // to approximate the c-side support frame and the subsequent
  // spheres clip the shape with curvature preserved
  TGeoSphere *sphere1 = new TGeoSphere("sphere1", rMin, rMax, tMin, tMax, pMin, pMax);
  TGeoSphere *sphere2 = new TGeoSphere("sphere2", rMin - sweep, rMax + sweep, tMin, tMax, pMin, pMax);
  TGeoSphere *sphere3 = new TGeoSphere("sphere3", rMin, rMin + backPlateZ, tMin, tMax, pMin, pMax);
  TGeoSphere *sphere4 = new TGeoSphere("sphere4", rMin - sweep, rMax + backPlateZ + sweep, tMin, tMax, pMin, pMax);

  TGeoBBox *insSeat = new TGeoBBox("insSeat", pinstart[0] * 2, pinstart[1] * 2, pinstart[2] * 2);

  TGeoBBox *quartzRadiatorSeat = new TGeoBBox("quartzRadiatorSeat",
                                              sQuartzRadiatorSide / 2 + sEps + errQrdXY,
                                              sQuartzRadiatorSide / 2 + sEps + errQrdXY,
                                              sQuartzRadiatorZC / 2 + sEps + errQrdZ);

  TGeoBBox *pmtBoxSeat = new TGeoBBox("pmtBoxSeat",
                                      sPmtSide / 2 + sEps + errPMTXY,
                                      sPmtSide / 2 + sEps + errPMTXY,
                                      sPmtZ / 2 + sEps + errPMTZ);
  TGeoBBox *pmtCornerRect = new TGeoBBox("pmtCornerRect",
                                         sCornerRadius / 2 - flopsErr,
                                         sCornerRadius / 2 - flopsErr,
                                         sPmtZ / 2);

  TGeoBBox *framecornerBox = new TGeoBBox("framecornerBox", 5, 5, 10);

  // C-side transformations
  TGeoRotation *rot1 = new TGeoRotation("rot1", 90, 0, 0);
  rot1->RegisterYourself();
  TGeoCombiTrans *rotTr1 = new TGeoCombiTrans("rotTr1", -20, -1, -5, rot1); // cuts off left side of shell
  rotTr1->RegisterYourself();

  TGeoRotation *rot2 = new TGeoRotation("rot2", -90, 0, 0);
  rot2->RegisterYourself();
  TGeoCombiTrans *rotTr2 = new TGeoCombiTrans("rotTr2", 20, -1, -5, rot2);
  rotTr2->RegisterYourself();

  TGeoRotation *rot3 = new TGeoRotation("rot3", 0, 0, 0);
  rot3->RegisterYourself();
  TGeoCombiTrans *rotTr3 = new TGeoCombiTrans("rotTr3", 0, 20, -5, rot3);
  rotTr3->RegisterYourself();

  TGeoTranslation *centerTrans = new TGeoTranslation("centerTrans", 0, 0, 85);
  centerTrans->RegisterYourself();

  TGeoRotation *reflectC1 = new TGeoRotation("reflectC1", 0, 0, 0);
  reflectC1->ReflectX(true);
  reflectC1->ReflectY(true);
  reflectC1->RegisterYourself();

  TGeoRotation *rotCorners = new TGeoRotation("rotCorners", acCorn, bcCorn, gcCorn);
  rotCorners->RegisterYourself();

  TGeoCombiTrans *comCorners = new TGeoCombiTrans("comCorners", xCorn, yCorn, zCorn, rotCorners);
  comCorners->RegisterYourself();

  TGeoCombiTrans *comCorners2 = new TGeoCombiTrans("comCorners2", xCorn2, yCorn2, zCorn2, rotCorners);
  comCorners2->RegisterYourself();

  // Create a string that defines the composite shape
  std::string shellString = "";
  shellString += "sphere1";                      // start with spherical shell - this will be reflected
  shellString += "- sphere2:rotTr1";             // copy and combitrans a subtraction
  shellString += "- sphere2:rotTr2";             //
  shellString += "- sphere2:rotTr3";             //
  shellString += "- insSeat:centerTrans";        // subtract center
  shellString += "- framecornerBox:comCorners";  // subtract corners
  shellString += "- framecornerBox:comCorners2"; //

  // Create string that defines the back plate composite shape
  std::string backPlateString = "";
  backPlateString += "sphere3";
  backPlateString += "- sphere4:rotTr1";
  backPlateString += "- sphere4:rotTr2";
  backPlateString += "- sphere4:rotTr3";
  backPlateString += "- insSeat:centerTrans";
  backPlateString += "- framecornerBox:comCorners";
  backPlateString += "- framecornerBox:comCorners2";

  // These could be set up to use the values in the geometry file after some
  // investigation of subtle differences...
  Double_t xi[NCellsC] = {-15.038271418735729, 15.038271418735729,
                          -15.003757581112167, 15.003757581112167, -9.02690018974363,
                          9.02690018974363, -9.026897413747076, 9.026897413747076,
                          -9.026896531935773, 9.026896531935773, -3.0004568618531313,
                          3.0004568618531313, -3.0270795197907225, 3.0270795197907225,
                          3.0003978432927543, -3.0003978432927543, 3.0270569670429572,
                          -3.0270569670429572, 9.026750365564254, -9.026750365564254,
                          9.026837450695885, -9.026837450695885, 9.026849243816981,
                          -9.026849243816981, 15.038129472387304, -15.038129472387304,
                          15.003621961057961, -15.003621961057961};
  Double_t yi[NCellsC] = {3.1599494336464455, -3.1599494336464455,
                          9.165191680982874, -9.165191680982874, 3.1383331772537426,
                          -3.1383331772537426, 9.165226363918643, -9.165226363918643,
                          15.141616002932361, -15.141616002932361, 9.16517861649866,
                          -9.16517861649866, 15.188854859073416, -15.188854859073416,
                          9.165053319552113, -9.165053319552113, 15.188703787345304,
                          -15.188703787345304, 3.138263189805292, -3.138263189805292,
                          9.165104089644917, -9.165104089644917, 15.141494417823818,
                          -15.141494417823818, 3.1599158563428644, -3.1599158563428644,
                          9.165116302773846, -9.165116302773846};
  Double_t zi[NCellsC];
  for (Int_t ic = 0; ic < NCellsC; ic++)
  {
    zi[ic] = TMath::Sqrt(TMath::Power(crad, 2) - TMath::Power(xi[ic], 2) - TMath::Power(yi[ic], 2));
  }

  // get rotation data
  Double_t ac[NCellsC], bc[NCellsC], gc[NCellsC];
  for (Int_t i = 0; i < NCellsC; i++)
  {
    ac[i] = TMath::ATan(yi[i] / xi[i]) - TMath::Pi() / 2 + 2 * TMath::Pi();
    if (xi[i] < 0)
    {
      bc[i] = TMath::ACos(zi[i] / crad);
    }
    else
    {
      bc[i] = -1 * TMath::ACos(zi[i] / crad);
    }
  }

  Double_t xc2[NCellsC], yc2[NCellsC], zc2[NCellsC];

  // compensation based on node position within individual detector geometries
  // determine compensated radius
  Double_t rcomp = crad + pstartC[2] / 2.0;
  for (Int_t i = 0; i < NCellsC; i++)
  {
    // Get compensated translation data
    xc2[i] = rcomp * TMath::Cos(ac[i] + TMath::Pi() / 2) * TMath::Sin(-1 * bc[i]);
    yc2[i] = rcomp * TMath::Sin(ac[i] + TMath::Pi() / 2) * TMath::Sin(-1 * bc[i]);
    zc2[i] = rcomp * TMath::Cos(bc[i]);

    // Convert angles to degrees
    ac[i] *= 180 / TMath::Pi();
    bc[i] *= 180 / TMath::Pi();
    gc[i] = -1 * ac[i];
  }

  Double_t rmag = sqrt(xc2[0] * xc2[0] + yc2[0] * yc2[0] + zc2[0] * zc2[0]);

  Double_t scalePMT = (rmag + (frameHeightC / 2.0) - (sPmtHeightC / 2)) / rmag;
  Double_t scaleQrad = (rmag + (frameHeightC / 2.0) - sPmtHeightC - (sQuartzRadiatorZC / 2.0)) / rmag;

  Double_t xPMT[NCellsC];
  Double_t yPMT[NCellsC];
  Double_t zPMT[NCellsC];
  Double_t aPMT[NCellsC];
  Double_t bPMT[NCellsC];
  Double_t gPMT[NCellsC];

  Double_t xQrad[NCellsC];
  Double_t yQrad[NCellsC];
  Double_t zQrad[NCellsC];
  Double_t aQrad[NCellsC];
  Double_t bQrad[NCellsC];
  Double_t gQrad[NCellsC];

  Double_t rotC[NCellsC];
  Double_t comC[NCellsC];

  for (Int_t i = 0; i < NCellsC; i++)
  {
    // PMT Transformations
    xPMT[i] = scalePMT * xc2[i];
    yPMT[i] = scalePMT * yc2[i];
    zPMT[i] = scalePMT * zc2[i];

    // Quartz radiator transformations
    xQrad[i] = scaleQrad * xc2[i];
    yQrad[i] = scaleQrad * yc2[i];
    zQrad[i] = scaleQrad * zc2[i];
  }

  TString nameRot;
  TString nameComPMT;
  TString nameComQuartz;
  TString nameComPlates;
  TString nameComC;

  for (Int_t itr = NCellsA; itr < NCellsA + NCellsC; itr++)
  {
    nameRot = Form("0Rot%i", itr + 1);
    int ic = itr - NCellsA;
    nameComPMT = Form("0ComPMT%i", ic + 1);
    nameComQuartz = Form("0ComQuartz%i", ic + 1);

    // getting even indices to skip reflections -> reflections happen later in
    // frame construction
    if (ic % 2 == 0)
    {
      TGeoRotation *rotC = new TGeoRotation(nameRot.Data(), ac[ic], bc[ic], gc[ic]);
      rotC->RegisterYourself();

      TGeoCombiTrans *comC = new TGeoCombiTrans(nameComC.Data(), xc2[ic], yc2[ic], zc2[ic], rotC);
      comC->RegisterYourself();

      TGeoRotation *rotPMT = new TGeoRotation(nameRot.Data(), ac[ic], bc[ic], gc[ic]);
      rotPMT->RegisterYourself();

      TGeoCombiTrans *comPMT = new TGeoCombiTrans(nameComPMT.Data(), xPMT[ic], yPMT[ic], zPMT[ic], rotPMT);
      comPMT->RegisterYourself();

      TGeoRotation *rotQuartz = new TGeoRotation(nameRot.Data(), ac[ic], bc[ic], gc[ic]);
      rotQuartz->RegisterYourself();

      TGeoCombiTrans *comQuartz = new TGeoCombiTrans(nameComQuartz.Data(),
                                                     xQrad[ic], yQrad[ic],
                                                     zQrad[ic] - (sQuartzRadiatorZC / 2 + 3 * sEps),
                                                     rotQuartz);
      comQuartz->RegisterYourself();

      TGeoRotation *rotPlates = new TGeoRotation(nameRot.Data(), ac[ic], bc[ic], gc[ic]);
      rotPlates->RegisterYourself();
      TGeoCombiTrans *comPlates = new TGeoCombiTrans(nameComPlates.Data(),
                                                     xQrad[ic], yQrad[ic],
                                                     zQrad[ic], rotPlates);
      comPlates->RegisterYourself();

      // Subtract the PMTs from the frame
      std::string pmtCombiString = "";
      pmtCombiString += "- ";
      pmtCombiString += "pmtBoxSeat:";
      pmtCombiString += nameComPMT.Data();
      shellString += pmtCombiString;

      // Subtract the QuartzRadiators from the frame
      std::string quartzCombiString = "";
      quartzCombiString += "- ";
      quartzCombiString += "quartzRadiatorSeat:";
      quartzCombiString += nameComQuartz.Data();
      shellString += quartzCombiString;
    }
  }

  // Construct composite shape from boolean
  TGeoCompositeShape *shellCompShape = new TGeoCompositeShape("shellCompShape", shellString.c_str());

  TGeoVolume *shellVol = new TGeoVolume("shellVol", shellCompShape, Al);

  // frame mother assembly
  TGeoVolumeAssembly *FT0_C_Frame = new TGeoVolumeAssembly("FT0_C_Frame");

  // placement and reflections of frame approxes
  TGeoTranslation *shellTr1 = new TGeoTranslation("shellTr1", 0, 0, -80);
  shellTr1->RegisterYourself();

  TGeoCombiTrans *shellTr2 = new TGeoCombiTrans("shellTr2", 0, 0, -80, reflectC1);
  shellTr2->RegisterYourself();

  FT0_C_Frame->AddNode(shellVol, 1, shellTr1);
  FT0_C_Frame->AddNode(shellVol, 2, shellTr2);

  TGeoTranslation *backPlateTr1 = new TGeoTranslation("backPlateTr1", 0, 0, -74);
  backPlateTr1->RegisterYourself();

  TGeoCombiTrans *backPlateTr2 = new TGeoCombiTrans("backPlateTr2", 0, 0, -74, reflectC1);
  backPlateTr2->RegisterYourself();

  TGeoCompositeShape *backPlateShape = new TGeoCompositeShape("backPlateShape", backPlateString.c_str());
  TGeoVolume *backPlateVol = new TGeoVolume("backPlateVol", backPlateShape, Al);

  FT0_C_Frame->AddNode(backPlateVol, 3, backPlateTr1);
  FT0_C_Frame->AddNode(backPlateVol, 4, backPlateTr2);

  return FT0_C_Frame;
}

TString AliFT0::cPlateShapeString()
{
  Double_t prismHeight = 0.3895; // height of vertical edge of square prism part of base
  Double_t prismSide = 5.9;      // width and length of square prism part of base
  Double_t radCurve = 81.9469;   // radius of curvature of top part of base
  Double_t delHeight = radCurve * (1.0 - TMath::Sqrt(1.0 - 0.5 * TMath::Power(prismSide / radCurve, 2.0)));
  // height from top of square prism to center of curved top surface of base

  Double_t heightBase = prismHeight + delHeight; // from center of bottom to center of top
  Double_t sliceSide = 5.3;                      // side lengths of slice's flat top
  Double_t heightBaseBox = 2 * heightBase;
  Double_t totalHeight = 0.5;
  Double_t sliceHeight = 0.5 - heightBase;

  // cable dimensions and distances
  Double_t cableHoleWidth = 0.3503;
  Double_t cableHoleLength = 0.9003;
  Double_t cableHoleDepth = 1; // really big just to punch a hole

  // sholes denotes "straight holes" and rholes denote "rotated holes"
  // all distances measured from edges of slice
  // up and down sholes
  Double_t sHolesBottomEdge = 1.585;
  Double_t sHolesTopEdge = 0.515;
  Double_t sHolesAvgTopBottom = (sHolesBottomEdge + sHolesTopEdge) / 2.0;
  Double_t sHolesUpFromCenter = ((sliceSide / 2.0) - sHolesAvgTopBottom); // amount up in x the sholes need to move
  // left and right sholes
  Double_t sHolesFarEdge = 1.585;
  Double_t sHolesNearEdge = 1.065;
  Double_t sHolesAvgNearFar = (sHolesFarEdge + sHolesNearEdge) / 2.0;
  Double_t sHolesLateralFromCenter = ((sliceSide / 2.0) - sHolesAvgNearFar);

  // Create Boxes
  TGeoBBox *box = new TGeoBBox("BASE", prismSide / 2.0, heightBaseBox / 2.0, prismSide / 2.0);

  // Base raw box to be subtracted
  TGeoBBox *slice = new TGeoBBox("SLICE", sliceSide / 2.0, heightBaseBox / 2.0, sliceSide / 2.0);
  TGeoBBox *cableHole = new TGeoBBox("CABLE", cableHoleLength / 2.0, cableHoleDepth / 2.0, cableHoleWidth / 2.0);
  TGeoBBox *cableHole2 = new TGeoBBox("CABLE2", cableHoleWidth / 2.0, cableHoleLength / 2.0, cableHoleDepth / 2.0);

  TGeoSphere *baseShape = new TGeoSphere("BASE_SUBTRACTION", radCurve, radCurve + 5.0, 80, 100, 80, 100);

  TGeoTranslation *rTrans = new TGeoTranslation("rTrans", 0, radCurve, 0);
  rTrans->RegisterYourself();

  TGeoTranslation *rBackTrans = new TGeoTranslation("rBackTrans", 0, -1.0 * radCurve, 0);
  rBackTrans->RegisterYourself();

  TGeoTranslation *subSliceTrans = new TGeoTranslation("subSliceTrans", 0, (heightBaseBox / 2.0) + sliceHeight, 0);
  subSliceTrans->RegisterYourself();

  TGeoTranslation *sHolesTopLeftTrans = new TGeoTranslation("sHolesTopLeftTrans", sHolesUpFromCenter, 0, sHolesLateralFromCenter);
  sHolesTopLeftTrans->RegisterYourself();

  TGeoTranslation *sHolesTopRightTrans = new TGeoTranslation("sHolesTopRightTrans", sHolesUpFromCenter, 0, -1.0 * sHolesLateralFromCenter);
  sHolesTopRightTrans->RegisterYourself();

  TGeoTranslation *testTrans = new TGeoTranslation("testTrans", 0.1, 0.1, 0);
  testTrans->RegisterYourself();

  TGeoRotation *switchToZ = new TGeoRotation("switchToZ", 90, 90, 0);
  switchToZ->RegisterYourself();

  TGeoRotation *rotateHolesLeft = new TGeoRotation("rotateHolesLeft", 345, 0, 0);
  rotateHolesLeft->RegisterYourself();

  TGeoRotation *rotateHolesRight = new TGeoRotation("rotatetHolesRight", 15, 0, 0);
  rotateHolesRight->RegisterYourself();

  // Bottom holes rotation and translation with combitrans
  TGeoCombiTrans *rHolesBottomLeftTrans = new TGeoCombiTrans("rHolesBottomLeftTrans", -1.0 * sHolesLateralFromCenter, -1.0 * sHolesUpFromCenter, 0, rotateHolesLeft);
  rHolesBottomLeftTrans->RegisterYourself();

  TGeoCombiTrans *rHolesBottomRightTrans = new TGeoCombiTrans("rHolesBottomRightTrans", sHolesLateralFromCenter, -1.0 * sHolesUpFromCenter, 0, rotateHolesRight);
  rHolesBottomRightTrans->RegisterYourself();

  TString plateString = " ";
  plateString += "(((BASE:rTrans";
  plateString += "- BASE_SUBTRACTION)";
  plateString += "+ (SLICE:rTrans))";
  plateString += ":rBackTrans";
  plateString += "- BASE:subSliceTrans";
  plateString += "- (CABLE:sHolesTopLeftTrans)";
  plateString += "- (CABLE:sHolesTopRightTrans))";
  plateString += ":switchToZ";
  plateString += "- (CABLE2:rHolesBottomLeftTrans)";
  plateString += "- (CABLE2:rHolesBottomRightTrans)";

  return plateString;
}
