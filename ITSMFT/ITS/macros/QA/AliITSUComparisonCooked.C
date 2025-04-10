/****************************************************************************
 *          A standalone comparison macro for the upgraded ITS.             *
 *                                                                          *
 *               Creates list of "trackable" tracks,                        *
 *             calculates efficiency, resolutions etc.                      *
 *  The ESD tracks must be in an appropriate state: kITSrefit               *
 *                                                                          *
 *          The efficiency and resolutions are calculated                   * 
 *   wrt "primary-like" pions within the acceptance tan(lambda)~[-1,1]      *
 *        and for tracks having all 7 clusters correctly assigned.          *
 *                                                                          *
 * Before running, load the ITSU libraries:                                 *
 *  gSystem->Load("libITSUpgradeBase");gSystem->Load("libITSUpgradeRec");   *
 *                                                                          *
 *           Origin: I.Belikov, IPHC, Iouri.Belikov@iphc.cnrs.fr            *
 ****************************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT__)
  #include <TMath.h>
  #include <TError.h>
  #include <Riostream.h>
  #include <TH1F.h>
  #include <TH2F.h>
  #include <TTree.h>
  #include <TParticle.h>
  #include <TCanvas.h>
  #include <TLine.h>
  #include <TText.h>
  #include <TBenchmark.h>
  #include <TStyle.h>
  #include <TFile.h>
  #include <TROOT.h>

  #include "AliStack.h"
  #include "AliHeader.h"
  #include "AliGenEventHeader.h"
  #include "AliTrackReference.h"
  #include "AliRunLoader.h"
  #include "AliRun.h"
  #include "AliESDEvent.h"
  #include "AliESDtrack.h"

  #include "AliITSUClusterPix.h"
  #include "AliITSULoader.h"
#endif

Int_t GoodTracksCooked(const Char_t *dir=".");

extern AliRun *gAlice;
extern TBenchmark *gBenchmark;
extern TROOT *gROOT;

static Int_t allgood=0;
static Int_t allselected=0;
static Int_t allfound=0;

void root(TH1 *h) {
  Int_t nb=h->GetNbinsX();
  for (Int_t i=0; i<nb; i++) {
    Float_t c=h->GetBinContent(i);
    c=TMath::Sqrt(c);
    h->SetBinContent(i,c);

    Float_t e=h->GetBinError(i);
    if (c!=0) e = 0.5*e/c;
    h->SetBinError(i,e);
  }
}
void divide(TH1 *h) {
  Int_t nb=h->GetNbinsX();
  for (Int_t i=0; i<nb; i++) {
    Float_t c=h->GetBinContent(i);
    c *= h->GetBinCenter(i);
    h->SetBinContent(i,c);

    Float_t e=h->GetBinError(i);
    e *= h->GetBinCenter(i);
    h->SetBinError(i,e);
  }
}

Int_t AliITSUComparisonCooked
(Float_t ptcutl=0.01, Float_t ptcuth=10., const Char_t *dir=".") {
   gBenchmark->Start("AliITSUComparisonCooked");

   ::Info("AliITSUComparisonCooked.C","Doing comparison...");
   

   TH1F *hp=(TH1F*)gROOT->FindObject("hp");
   if (!hp) hp=new TH1F("hp","PHI resolution",50,-20.,20.); 
   hp->SetFillColor(4);

   TH1F *hl=(TH1F*)gROOT->FindObject("hl");
   if (!hl) hl=new TH1F("hl","LAMBDA resolution",50,-20,20);
   hl->SetFillColor(4);

   //TH1F *hz=(TH1F*)gROOT->FindObject("hz");
   //if (!hz) hz=new TH1F("hz","Longitudinal impact parameter",30,-777.,777.); 


   Int_t nb=100;
   Float_t xbins[nb+1];
   Double_t a=TMath::Log(ptcuth/ptcutl)/nb;
   for (Int_t i=0; i<=nb; i++) xbins[i] = ptcutl*TMath::Exp(i*a);

   TH1F *hgood=(TH1F*)gROOT->FindObject("hgood");
   if (!hgood) hgood=new TH1F("hgood","Good tracks",nb,xbins);
    
   TH1F *hfound=(TH1F*)gROOT->FindObject("hfound");
   if (!hfound) hfound=new TH1F("hfound","Found tracks",nb,xbins);

   TH1F *hfake=(TH1F*)gROOT->FindObject("hfake");
   if (!hfake) hfake=new TH1F("hfake","Fake tracks",nb,xbins);

   TH1F *hg=(TH1F*)gROOT->FindObject("hg");
   if (!hg) hg=new TH1F("hg","Efficiency for good tracks",nb,xbins);
   hg->SetLineColor(4); hg->SetLineWidth(2);

   TH1F *hf=(TH1F*)gROOT->FindObject("hf");
   if (!hf) hf=new TH1F("hf","Efficiency for fake tracks",nb,xbins);
   hf->SetFillColor(1); hf->SetFillStyle(3013); hf->SetLineWidth(2);

   TH1F *hpt=(TH1F*)gROOT->FindObject("hpt");
   if (!hpt) hpt=new TH1F("hpt","Relative Pt resolution",nb,xbins); 
   hpt->Sumw2();
   TH1F *hd=(TH1F*)gROOT->FindObject("hd");
   if (!hd) 
      hd=new TH1F("hd","Transverse impact parameter",nb,xbins); 
   hd->Sumw2();


   Char_t fname[100];
   snprintf(fname,100,"%s/GoodTracksCooked.root",dir);

   TFile *refFile=TFile::Open(fname,"old");
   if (!refFile || !refFile->IsOpen()) {
   ::Info("AliITSUComparisonCooked.C","Marking good tracks (will take a while)...");
     if (GoodTracksCooked(dir)) {
        ::Error("AliITSUComparisonCooked.C","Can't generate the reference file !");
        return 1;
     }
   }
   refFile=TFile::Open(fname,"old");
   if (!refFile || !refFile->IsOpen()) {
     ::Error("AliITSUComparisonCooked.C","Can't open the reference file !");
     return 1;
   }   
  
   TTree *itsTree=(TTree*)refFile->Get("itsTree");
   if (!itsTree) {
     ::Error("AliITSUComparisonCooked.C","Can't get the reference tree !");
     return 2;
   }
   TBranch *branch=itsTree->GetBranch("ITS");
   if (!branch) {
     ::Error("AliITSUComparisonCooked.C","Can't get the ITS branch !");
     return 3;
   }
   TClonesArray dummy("AliTrackReference",1000), *refs=&dummy;
   branch->SetAddress(&refs);


   snprintf(fname,100,"%s/AliESDs.root",dir);
   TFile *ef=TFile::Open(fname);
   if ((!ef)||(!ef->IsOpen())) {
      snprintf(fname,100,"%s/AliESDits.root",dir);
      ef=TFile::Open(fname);
      if ((!ef)||(!ef->IsOpen())) {
         ::Error("AliITSUComparisonCooked.C","Can't open AliESDits.root !");
         return 4;
      }
   }
   AliESDEvent* event = new AliESDEvent();
   TTree* esdTree = (TTree*) ef->Get("esdTree");
   if (!esdTree) {
      ::Error("AliITSComparison.C", "no ESD tree found");
      return 6;
   }
   event->ReadFromTree(esdTree);


   //******* Loop over events *********
   Int_t e=0;
   while (esdTree->GetEvent(e)) {
     cout<<endl<<endl<<"********* Processing event number: "<<e<<"*******\n";
 
     Int_t nentr=event->GetNumberOfTracks();
     allfound+=nentr;

     if (itsTree->GetEvent(e++)==0) {
        cerr<<"No reconstructable tracks !\n";
        continue;
     }

     Int_t ngood=refs->GetEntriesFast(); 
     allgood+=ngood;

     const Int_t MAX=15000;
     Int_t notf[MAX], nnotf=0;
     Int_t fake[MAX], nfake=0;
     Int_t mult[MAX], numb[MAX], nmult=0;
     Int_t k;
     for (k=0; k<ngood; k++) {
	AliTrackReference *ref=(AliTrackReference*)refs->UncheckedAt(k); 
        Int_t lab=ref->Label(), tlab=-1;
        Float_t ptg=TMath::Sqrt(ref->Px()*ref->Px() + ref->Py()*ref->Py());

        Int_t pdg=(Int_t)ref->GetLength();  //this is particle's PDG !
        if (TMath::Abs(pdg)!=211) continue; //select pions only

        if (ptg<ptcutl) continue;
        if (ptg>ptcuth) continue;

        allselected++;

        hgood->Fill(ptg);

        AliESDtrack *esd=0;
        Int_t cnt=0;
        for (Int_t i=0; i<nentr; i++) {
           AliESDtrack *t=event->GetTrack(i);
	   ULong64_t status=t->GetStatus();

           if ((status&AliESDtrack::kITSrefit)==0) continue;
           if (t->GetITSclusters(0)<4) continue;

           Int_t lbl=t->GetLabel();
           if (lab==TMath::Abs(lbl)) {
	      if (cnt==0) {esd=t; tlab=lbl;}
              if (lbl> 0) {esd=t; tlab=lbl;}  
              cnt++;
           }
        }
        if (cnt==0) {
           notf[nnotf++]=lab;
           continue;
        } else if (cnt>1){
           mult[nmult]=lab;
           numb[nmult]=cnt; nmult++;        
        }

        if (lab==tlab) hfound->Fill(ptg);
        else {
          fake[nfake++]=lab;
          hfake->Fill(ptg); 
        }

        if (esd->GetLabel()<0) continue; //resolutions for good tracks only

        Double_t alpha=esd->GetAlpha(),xv,par[5]; 
        esd->GetExternalParameters(xv,par);
        Float_t phi=TMath::ASin(par[2]) + alpha;
        if (phi<-TMath::Pi()) phi+=2*TMath::Pi();
        if (phi>=TMath::Pi()) phi-=2*TMath::Pi();
        Float_t lam=TMath::ATan(par[3]); 
        Float_t pt_1=TMath::Abs(par[4]);

        Float_t phig=TMath::ATan2(ref->Py(),ref->Px());
        hp->Fill((phi - phig)*1000.);

        Float_t lamg=TMath::ATan2(ref->Pz(),ptg);
        hl->Fill((lam - lamg)*1000.);

        Float_t d,z; esd->GetImpactParameters(d,z);
        d*=10000; //microns
        Float_t w=d*d;
        hd->Fill(ptg, w);
        //z*=10000; //microns
        //hz->Fill(z);

        w=(pt_1 - 1/ptg)*100 * (pt_1-1/ptg)*100;
        hpt->Fill(ptg, w);

     }

     cout<<"\nList of Not found tracks :\n";
     for (k=0; k<nnotf; k++){
       cout<<notf[k]<<"\t";
       if ((k%9)==8) cout<<"\n";
     }
     cout<<"\n\nList of fake  tracks :\n";
     for (k=0; k<nfake; k++){
       cout<<fake[k]<<"\t";
       if ((k%9)==8) cout<<"\n";
     }
     cout<<"\n\nList of multiple found tracks :\n";
     for (k=0; k<nmult; k++) {
         cout<<"id.   "<<mult[k]
             <<"     found - "<<numb[k]<<"times\n";
     }
     cout<<endl;

     cout<<"Number of found tracks : "<<nentr<<endl;
     cout<<"Number of \"good\" tracks : "<<ngood<<endl;

     refs->Clear();
   } //***** End of the loop over events

   delete event;
   delete esdTree;
   ef->Close();
   
   delete itsTree;
   refFile->Close();

   Stat_t ng=hgood->GetEntries(), nf=hfound->GetEntries();
   if (ng!=0) cout<<"\n\nIntegral efficiency is about "<<nf/ng*100.<<" %\n";
   cout<<"Total number selected of \"good\" tracks ="<<allselected<<endl<<endl;
   cout<<"Total number of found tracks ="<<allfound<<endl;
   cout<<"Total number of \"good\" tracks ="<<allgood<<endl;
   cout<<endl;

   gStyle->SetOptStat(111110);
   gStyle->SetOptFit(1);

   TCanvas *c1=new TCanvas("c1","",0,0,700,850);

   Int_t minc=33; 

   TPad *p1=new TPad("p1","",0,0.3,.5,.6); p1->Draw();
   p1->cd(); p1->SetFillColor(42); p1->SetFrameFillColor(10); 
   hp->SetFillColor(4);  hp->SetXTitle("(mrad)"); 
   if (hp->GetEntries()<minc) hp->Draw(); else hp->Fit("gaus"); c1->cd();

   TPad *p2=new TPad("p2","",0.5,.3,1,.6); p2->Draw(); 
   p2->cd(); p2->SetFillColor(42); p2->SetFrameFillColor(10);
   hl->SetXTitle("(mrad)");
   if (hl->GetEntries()<minc) hl->Draw(); else hl->Fit("gaus"); c1->cd();

   TPad *p3=new TPad("p3","",0,0,0.5,0.3); 
   p3->SetLogx(); p3->SetGridx(); p3->SetGridy();
   p3->Draw();
   p3->cd(); p3->SetFillColor(42); p3->SetFrameFillColor(10); 
   hpt->SetXTitle("p_{T} (GeV/c)");
   hpt->SetYTitle("(%)");
   TH1F *hh=new TH1F(*hfound);
   //hh->Add(hfake);
   hpt->Divide(hh);
   root(hpt);
   divide(hpt);
   hpt->Draw(); c1->cd();

   TPad *p4=new TPad("p4","",0.5,0,1,0.3); 
   p4->SetLogx(); p4->SetGridx(); p4->SetGridy();
   p4->Draw();
   p4->cd(); p4->SetFillColor(42); p4->SetFrameFillColor(10);
   hd->SetXTitle("p_{T} (GeV/c)");
   hd->SetYTitle("(micron)");
   hd->Divide(hh);
   root(hd);
   hd->Draw(); c1->cd();

   //hz->Draw("same"); c1->cd();
   

   TPad *p5=new TPad("p5","",0,0.6,1,1); 
   p5->SetLogx(); p5->SetGridx(); p5->SetGridy(); 
   p5->Draw(); p5->cd(); 
   p5->SetFillColor(41); p5->SetFrameFillColor(10);
   hfound->Sumw2(); hgood->Sumw2(); hfake->Sumw2();
   hg->Divide(hfound,hgood,1,1.,"b");
   hf->Divide(hfake,hgood,1,1.,"b");
   hg->SetYTitle("Tracking efficiency (%)");
   hg->SetXTitle("p_{T} (GeV/c)");
   hg->Scale(100);
   hg->Draw();

   hf->SetFillColor(1);
   hf->SetFillStyle(3013);
   hf->SetLineColor(2);
   hf->SetLineWidth(2);
   hf->Scale(100);
   hf->Draw("histsame");
   TText *text = new TText(0.4, 20., "Fake tracks");
   text->SetTextSize(0.05);
   text->Draw();
   text = new TText(0.4, 80., "Good tracks");
   text->SetTextSize(0.05);
   text->Draw();

   TFile fc("AliITSUComparisonCooked.root","RECREATE");
   c1->Write();
   fc.Close();

   gBenchmark->Stop("AliITSUComparisonCooked");
   gBenchmark->Show("AliITSUComparisonCooked");

   return 0;
}



Int_t GoodTracksCooked(const Char_t *dir) {
   if (gAlice) { 
       delete AliRunLoader::Instance();
       delete gAlice;//if everything was OK here it is already NULL
       gAlice = 0x0;
   }

   Char_t fname[100];
   snprintf(fname,100,"%s/galice.root",dir);

   AliRunLoader *rl = AliRunLoader::Open(fname,"COMPARISON");
   if (!rl) {
      ::Error("GoodTracksCooked","Can't start session !");
      return 1;
   }

   rl->LoadgAlice();
   rl->LoadHeader();
   rl->LoadKinematics();

   AliITSULoader* itsl = (AliITSULoader*)rl->GetLoader("ITSLoader");
   if (itsl == 0x0) {
       ::Error("GoodTracksCooked","Can not find the ITSLoader");
       delete rl;
       return 4;
   }
   itsl->LoadRecPoints();
  

   Int_t nev=rl->GetNumberOfEvents();
   ::Info("GoodTracksCooked","Number of events : %d\n",nev);  

   snprintf(fname,100,"%s/GoodTracksCooked.root",dir);
   TFile *itsFile=TFile::Open(fname,"recreate");
   TClonesArray dummy2("AliTrackReference",1000), *itsRefs=&dummy2;
   TTree itsTree("itsTree","Tree with info about the reconstructable ITS tracks");
   itsTree.Branch("ITS",&itsRefs);

   //********  Loop over generated events 
   for (Int_t e=0; e<nev; e++) {
     Int_t k;

     rl->GetEvent(e);  itsFile->cd();

     Int_t np = rl->GetHeader()->GetNtrack();
     cout<<"Event "<<e<<" Number of particles: "<<np<<endl;

     AliGenEventHeader *h=rl->GetHeader()->GenEventHeader();
     TArrayF vtx(3);
     h->PrimaryVertex(vtx);

     Bool_t skip=kFALSE;
     if (TMath::Abs(vtx[2]) > 10.) {
        cout<<"Skipping an event with Zv="<<vtx[2]<<endl;
        skip=kTRUE;
     }
 
     //******** Fill the "good" masks
     Int_t *good=new Int_t[np]; for (k=0; k<np; k++) good[k]=0;

     TTree *cTree=itsl->TreeR();
     if (!cTree) {
        ::Error("GoodTracksCooked","Can't get the cluster tree !"); 
        delete rl;
        return 8;
     }

     const Int_t nLayers=7;
     TBranch *branch[nLayers];
     TClonesArray clusters[nLayers];
     for (Int_t layer=0; layer<nLayers; layer++) {
       TClonesArray *ptr = 
       new(clusters+layer) TClonesArray("AliITSUClusterPix",1000);
       Char_t bname[33];
       snprintf(bname,33,"ITSRecPoints%d\0",layer);
       branch[layer]=cTree->GetBranch(bname);
       if (!branch[layer]) {
          ::Error("GoodTracksCooked","Can't get the clusters branch !"); 
          delete rl;
          return 9;
       }
       branch[layer]->SetAddress(&ptr);
     }

     Int_t entr=(Int_t)cTree->GetEntries();
     for (k=0; k<entr; k++) {
         cTree->GetEvent(k);
         for (Int_t lay=0; lay<nLayers; lay++) {
             Int_t ncl=clusters[lay].GetEntriesFast(); if (ncl==0) continue;
             while (ncl--) {
                AliITSUClusterPix *pnt=
                (AliITSUClusterPix*)clusters[lay].UncheckedAt(ncl);
                Int_t l0=pnt->GetLabel(0);
	        if (l0>=np) {
// 		   cerr<<"Wrong label: "<<l0<<endl;
		   continue;
	        }
                Int_t l1=pnt->GetLabel(1);
	        if (l1>=np) {
// 		   cerr<<"Wrong label: "<<l1<<endl;
		   continue;
	        }
                Int_t l2=pnt->GetLabel(2);
	        if (l2>=np) {
// 		   cerr<<"Wrong label: "<<l2<<endl;
		   continue;
	        }
                Int_t mask=1<<lay;
                if (l0>=0) good[l0]|=mask; 
                if (l1>=0) good[l1]|=mask; 
                if (l2>=0) good[l2]|=mask;
	     }
             clusters[lay].Clear();
	 }
     }



     //****** select tracks which are "good" enough
     AliStack* stack = rl->Stack();
     Int_t nt=0;
     for (k=0; k<np; k++) {
        if (good[k] != 0x7F) continue;

        if (skip) continue; //No good primary vertex for this event

        TParticle *p = (TParticle*)stack->Particle(k);
        if (p == 0x0) {
           cerr<<"Can not get particle "<<k<<endl;
           continue;
        }

        if (p->Pt() <= 0.) continue;
        if (TMath::Abs(p->Pz()/p->Pt())>0.999) continue;

        Double_t dx=p->Vx()-vtx[0], dy=p->Vy()-vtx[1], dz=p->Vz()-vtx[2];
        if (TMath::Sqrt(dx*dx+dy*dy)>0.0001) continue; //Primary-like
        if (TMath::Abs(dz) > 0.0001) continue;

	AliTrackReference *ref=new((*itsRefs)[nt]) AliTrackReference();
        ref->SetLabel(k);
        Int_t pdg=p->GetPdgCode();
        ref->SetLength(pdg);  //This will the particle's PDG !
	ref->SetMomentum(p->Px(),p->Py(),p->Pz());
	ref->SetPosition(p->Vx(),p->Vy(),p->Vz());
        nt++;
     }

     itsTree.Fill();
     itsRefs->Clear();

     delete[] good;

   } //*** end of the loop over generated events

   itsTree.Write();
   itsFile->Close();

   delete rl;
   return 0;
}


