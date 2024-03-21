#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TROOT.h>
#include <TStyle.h>
#include <Riostream.h>
#include <TStopwatch.h>
#include <TDatime.h>
#include <TClassTable.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TProfile.h>
#include <TFunction.h>
#include <TCanvas.h>
#include <TGrid.h>
#include <TFile.h>

#endif

void CheckPedestalHistos(Int_t nRun=0,  Int_t optPlot = 1)
{
  if(nRun==0){
    printf("\n\n YOU MUST PROVIDE A RUN NUMBER!!! \n\n");
    return;
  }
  
  TGrid::Connect("alien:",0,0,"t");
  
  char histoFName[150];
  snprintf(histoFName,150,"alien:///alice/data/2012/Reference/ZDC/%d_pedestalReference.root",nRun);

  int const kNChannels = 24;
  //
 TFile *file = TFile::Open(histoFName);
  file->cd();
  TH1F::AddDirectory(0);
  //
  TH1F *hPedhg[kNChannels], *hPedOutOfTimehg[kNChannels];
  TH2F *hPedCorrhg[kNChannels];
  TH1F *hPedlg[kNChannels], *hPedOutOfTimelg[kNChannels];
  TH2F *hPedCorrlg[kNChannels];
  //
  char namhist1hg[50], namhist2hg[50], namhist3hg[50];
  char namhist1lg[50], namhist2lg[50], namhist3lg[50];
  for(Int_t j=0; j<kNChannels; j++){
     if(j<=4){ // ZNC
       snprintf(namhist1hg,50,"PedZNChg_%d",j);
       snprintf(namhist2hg,50,"PedZNChgOutOfTime_%d",j);
       snprintf(namhist3hg,50,"PedCorrZNChg_%d",j);
       //
       snprintf(namhist1lg,50,"PedZNClg_%d",j);
       snprintf(namhist2lg,50,"PedZNClgOutOfTime_%d",j);
       snprintf(namhist3lg,50,"PedCorrZNClg_%d",j);
     }
     else if(j>=5 && j<=9){ // ZPC
       snprintf(namhist1hg,50,"PedZPChg_%d",j-5);
       snprintf(namhist2hg,50,"PedZPChgOutOfTime_%d",j-5);
       snprintf(namhist3hg,50,"PedCorrZPChg_%d",j-5);
       //
       snprintf(namhist1lg,50,"PedZPClg_%d",j-5);
       snprintf(namhist2lg,50,"PedZPClgOutOfTime_%d",j-5);
       snprintf(namhist3lg,50,"PedCorrZPClg_%d",j-5);       
     }
     else if(j==10 || j==11){ // ZEM
       snprintf(namhist1hg,50,"PedZEMhg_%d",j-9);
       snprintf(namhist2hg,50,"PedZEMhgOutOfTime_%d",j-9);
       snprintf(namhist3hg,50,"PedCorrZEMhg_%d",j-9);
       //
       snprintf(namhist1lg,50,"PedZEMlg_%d",j-9);
       snprintf(namhist2lg,50,"PedZEMlgOutOfTime_%d",j-9);
       snprintf(namhist3lg,50,"PedCorrZEMlg_%d",j-9);
     }
     else if(j>=12 && j<=16){ // ZNA
       snprintf(namhist1hg,50,"PedZNAhg_%d",j-12);
       snprintf(namhist2hg,50,"PedZNAhgOutOfTime_%d",j-12);
       snprintf(namhist3hg,50,"PedCorrZNAhg_%d",j-12);
       //
       snprintf(namhist1lg,50,"PedZNAlg_%d",j-12);
       snprintf(namhist2lg,50,"PedZNAlgOutOfTime_%d",j-12);
       snprintf(namhist3lg,50,"PedCorrZNAlg_%d",j-12);
     }
     else if(j>=17 && j<=21){ // ZPA
       snprintf(namhist1hg,50,"PedZPAhg_%d",j-17);
       snprintf(namhist2hg,50,"PedZPAhgOutOfTime_%d",j-17);
       snprintf(namhist3hg,50,"PedCorrZPAhg_%d",j-17);
       //
       snprintf(namhist1lg,50,"PedZPAlg_%d",j-17);
       snprintf(namhist2lg,50,"PedZPAlgOutOfTime_%d",j-17);
       snprintf(namhist3lg,50,"PedCorrZPAlg_%d",j-17);
     }
     else if(j==22 || j==23){ //Reference PMs
       snprintf(namhist1hg,50,"PedRefhg_%d",j-22);
       snprintf(namhist2hg,50,"PedRefhgOutOfTime_%d",j-22);
       snprintf(namhist3hg,50,"PedCorrRefhg_%d",j-22);
       //
       snprintf(namhist1lg,50,"PedReflg_%d",j-22);
       snprintf(namhist2lg,50,"PedReflgOutOfTime_%d",j-22);
       snprintf(namhist3lg,50,"PedCorrReflg_%d",j-22);
     }
     // --- High gain chain histos
     hPedhg[j] = (TH1F*) file->Get(namhist1hg);
     hPedOutOfTimehg[j] = (TH1F*) file->Get(namhist2hg);
     hPedCorrhg[j] =  (TH2F*) file->Get(namhist3hg);
     // --- Low gain chain histos
     hPedlg[j] = (TH1F*) file->Get(namhist1lg);
     hPedOutOfTimelg[j] =(TH1F*) file->Get(namhist2lg);
     hPedCorrlg[j] = (TH2F*) file->Get(namhist3lg);
  }

 if(optPlot==1){
  // Plot the retrieved histos
  //***********************************************************
  // #### ROOT initialization
  gROOT->Reset();
  gStyle->SetCanvasColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);
  gStyle->SetTitleTextColor(9);
  gStyle->SetStatTextColor(4);
  gStyle->SetLineColor(1);
  gStyle->SetPalette(1);
  //***********************************************************
  TCanvas *c6 = new TCanvas("c6","Side C correlations",0,200,1000,800);
  c6->Divide(5,4);
  for(Int_t t=0; t<10; t++){
    c6->cd(t+1);
    hPedCorrhg[t]->Draw();
    c6->cd(t+11);
    hPedCorrlg[t]->Draw();
  }
  c6->Print("CorrSideC.gif");
  //
  TCanvas *c7 = new TCanvas("c7","Side A correlations",300,200,1000,800);
  c7->Divide(5,4);
  for(Int_t t=0; t<10; t++){
    c7->cd(t+1);
    hPedCorrhg[t+12]->Draw();
    c7->cd(t+11);
    hPedCorrlg[t+12]->Draw();
  }
  c7->Print("CorrSideA.gif");
  //
  TCanvas *c8 = new TCanvas("c8","ZEM correlations",400,200,400,400);
  c8->Divide(2,2);
  for(Int_t t=0; t<2; t++){
    c8->cd(t+1);
    hPedCorrhg[t+10]->Draw();
    c8->cd(t+3);
    hPedCorrlg[t+10]->Draw();
  }
  c8->Print("CorrZEM.gif");
  //***********************************************************
  TCanvas *c1 = new TCanvas("c1","ZNC pedestals",0,0,1000,400);
  c1->Divide(5,2);
  for(Int_t y=0; y<5; y++){
    c1->cd(y+1);
    hPedhg[y]->Draw();
    c1->cd(y+6);
    hPedlg[y]->Draw();
  }
  c1->Print("ZNCPed.gif");
  //
  TCanvas *c2 = new TCanvas("c2","ZPC pedestals",300,0,1000,400);
  c2->Divide(5,2);
  for(Int_t y=0; y<5; y++){
    c2->cd(y+1);
    hPedhg[y+5]->Draw();
    c2->cd(y+6);
    hPedlg[y+5]->Draw();
  }
  c2->Print("ZPCPed.gif");
  //
  TCanvas *c3 = new TCanvas("c3","ZEM pedestals",400,0,400,400);
  c3->Divide(2,2);
  for(Int_t y=0; y<2; y++){
    c3->cd(y+1);
    hPedhg[y+10]->Draw();
    c3->cd(y+3);
    hPedlg[y+10]->Draw();
  }
  c3->Print("ZEMPed.gif");
  //
  TCanvas *c4 = new TCanvas("c4","ZNA pedestals",0,400,1000,400);
  c4->Divide(5,2);
  for(Int_t y=0; y<5; y++){
    c4->cd(y+1);
    hPedhg[y+12]->Draw();
    c4->cd(y+6);
    hPedlg[y+12]->Draw();
  }
  c4->Print("ZNAPed.gif");
  //
  TCanvas *c5 = new TCanvas("c5","ZPA pedestals",300,400,1000,400);
  c5->Divide(5,2);
  for(Int_t y=0; y<5; y++){
    c5->cd(y+1);
    hPedhg[y+17]->Draw();
    c5->cd(y+6);
    hPedlg[y+17]->Draw();
  }
  c5->Print("ZPAPed.gif");
 }
}
