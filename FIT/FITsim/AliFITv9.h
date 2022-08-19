#ifndef ALIFITV9_H
#define ALIFITV9_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

///////////////////////////////////////////////////
// Full geometry hits classes for detector: FIT //
///////////////////////////////////////////////////

#include "AliFIT.h"
#include "AliFT0.h"
#include "AliFV0.h"
#include "TGeoVolume.h"
#include "TGraph.h"
#include <TGeoMatrix.h>
#include <TVirtualMC.h>
#include <sstream>
#include <string>

class AliFITv9 : public AliFIT
{

public:
  AliFITv9();
  AliFITv9(const char *name, const char *title);
  AliFITv9(const AliFITv9 &o) : AliFIT(), fIdSens1(0), fPMTeff(0x0)
  {
    ((AliFITv9 &)o).Copy(*this);
  }

  AliFITv9 &operator=(const AliFITv9 &) { return *this; }
  virtual ~AliFITv9();
  virtual void CreateGeometry();

  // Optical properties reader: e-Energy, abs-AbsorptionLength[cm], n-refractive index
  void DefineOpticalProperties();
  Int_t ReadOptProperties(const std::string filePath, Float_t **e, Double_t **de, Float_t **abs, Float_t **n, Float_t **qe, Int_t &kNbins) const;
  void FillOtherOptProperties(Float_t **efficAll, Float_t **rindexAir, Float_t **absorAir,
                              Float_t **rindexCathodeNext, Float_t **absorbCathodeNext,
                              Double_t **efficMet, Double_t **aReflMet, const Int_t kNbins) const;
  void DeleteOptPropertiesArr(Float_t **e, Double_t **de, Float_t **abs, Float_t **n, Float_t **efficAll,
                              Float_t **rindexAir, Float_t **absorAir, Float_t **rindexCathodeNext,
                              Float_t **absorbCathodeNext, Double_t **efficMet, Double_t **aReflMet) const;

  virtual void AddAlignableVolumes() const;
  virtual void CreateMaterials();
  virtual void Init();
  virtual Int_t IsVersion() const { return 0; }
  Bool_t RegisterPhotoE(Double_t energy);
  virtual void StepManager();

protected:
  // T0+
  Int_t fIdSens1;            // Sensitive volume in FIT
  Int_t fSenseless;          // Senseless hit entry
  TGraph *fPMTeff;           // pmt registration effeicincy

  AliFT0 *fFT0Det;
  AliFV0 *fFV0Det;

  float fHits[13]; // Array of hits information

  // V0+
  Int_t fIdV0Plus[8 * 5]; // Sensitive volumes [nSectors][nRings], if modified then update the construct in .cxx

private:
  float fV0PlusnMeters;
  float fV0PlusLightYield;
  float fV0PlusLightAttenuation;
  float fV0PlusFibToPhot;

  ClassDef(AliFITv9, 2) // Class for FIT version 6
};

#endif
