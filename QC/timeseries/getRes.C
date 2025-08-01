TF1 *func,*fp0;
float sigma=65;
TFile *f;

Double_t fun2(Double_t *x, Double_t *par) {
  double trk = par[0]*TMath::Power((TMath::Max(x[0]-0.417,0.1))*(1-par[1]*x[1]*x[1]),-par[2]);
  return sqrt(trk*trk + sigma*sigma);
}

void tuneBack(TH1D *hB, TH1D *hS){
  TH1D hr(*hB);
  hr.SetName("htemp");
  hr.Divide(hS);
  hr.Fit(fp0,"WW");
  hB->Scale(1./fp0->GetParameter(0));
}

float getResEta(float par[6],const char* species="Pi",int binEta=0, int binPt=10){
  TH2F *hS = (TH2F *) f->Get(Form("h%sEta_%d",species,binEta));
  TH2F *hB = (TH2F *) f->Get(Form("h%sEtaM_%d",species,binEta));
  
  TH1D *hSig = hS->ProjectionY("hSig",binPt,binPt);
  TH1D *hBack = hB->ProjectionY("hBack",binPt,binPt);
  tuneBack(hBack, hSig);
  
  hSig->Draw();

  hSig->Add(hBack,-1);
  
  hSig->Fit(func,"","",-200,150);
  hSig->Fit(func,"WW","",-200,150);

  hBack->Draw("SAME");
  hBack->SetLineColor(2);
  
  par[0] = func->GetParameter(0);
  par[1] = func->GetParameter(1);
  par[2] = func->GetParameter(2);
  par[3] = hS->GetXaxis()->GetBinCenter(binPt);
  par[4] = func->GetParError(1);
  par[5] = func->GetParError(2);
  
  return par[2];
}

void getRes(){
  /* from O2Physics/Common/Core/PID/PIDTOF.h
      static constexpr std::array<const char*, 9> mDefaultResoParams{"14.3*TMath::Power((TMath::Max(x-0.319,0.1))*(1-0.4235*y*y),-0.8467)",
                                                                 "14.3*TMath::Power((TMath::Max(x-0.319,0.1))*(1-0.4235*y*y),-0.8467)",
                                                                 "14.3*TMath::Power((TMath::Max(x-0.319,0.1))*(1-0.4235*y*y),-0.8467)",
                                                                 "42.66*TMath::Power((TMath::Max(x-0.417,0.1))*(1-0.4235*y*y),-0.7145)",
                                                                 "99.46*TMath::Power((TMath::Max(x-0.447,0.1))*(1-0.4235*y*y),-0.8094)",
                                                                 "216*TMath::Power((TMath::Max(x-0.647,0.1))*(1-0.4235*y*y),-0.76)",
                                                                 "315*TMath::Power((TMath::Max(x-0.811,0.1))*(1-0.4235*y*y),-0.783)",
                                                                 "157*TMath::Power((TMath::Max(x-0.556,0.1))*(1-0.4235*y*y),-0.783)",
                                                                 "216*TMath::Power((TMath::Max(x-0.647,0.1))*(1-0.4235*y*y),-0.76)"};
   */
  
  static constexpr std::array<const char*, 9> mDefaultResoParams{"14.3*TMath::Power((TMath::Max(x-0.319,0.1))*(1-1.0235*y*y),-0.8467)",
                                                                 "14.3*TMath::Power((TMath::Max(x-0.319,0.1))*(1-1.0235*y*y),-0.8467)",
                                                                 "23*TMath::Power((TMath::Max(x-0.319,0.2))*(1-1.3*y*y),-0.373601)",
                                                                 "42.3505*TMath::Power((TMath::Max(x-0.417,0.1))*(1-1.13022*y*y),-0.588796)",
                                                                 "108.89*TMath::Power((TMath::Max(x-0.447,0.1))*(1-0.573154*y*y),-0.580692)",
                                                                 "236*TMath::Power((TMath::Max(x-0.647,0.1))*(1-0.4235*y*y),-0.76)",
                                                                 "335*TMath::Power((TMath::Max(x-0.811,0.1))*(1-0.4235*y*y),-0.783)",
                                                                 "170*TMath::Power((TMath::Max(x-0.556,0.1))*(1-0.4235*y*y),-0.783)",
                                                                 "236*TMath::Power((TMath::Max(x-0.647,0.1))*(1-0.4235*y*y),-0.76)"};

  TF2 *ffit = new TF2("ffit",fun2,0,3,-0.8,0.8,3);

  TF2 *fpi = new TF2("fpi",mDefaultResoParams[2],0,3,-0.8,0.8);
  TF2 *fka = new TF2("fka",mDefaultResoParams[3],0,3,-0.8,0.8);
  TF2 *fpr = new TF2("fpr",mDefaultResoParams[4],0,3,-0.8,0.8);
  TF2 *fde = new TF2("fde",mDefaultResoParams[5],0,3,-0.8,0.8);

  f = new TFile("summary.root");

  fp0 = new TF1("fp0","pol0");

  //  func = new TF1("func","[0]*TMath::Exp(-(x-[1])*(x-[1])/(2*([2]*[2]+[3]*[3])))",-150,150);
  func = new TF1("func","gaus");
  func->SetParameter(0,1);
  func->SetParameter(1,0);
  func->SetParLimits(1,-50,50);
  func->SetParameter(2,100);
  func->SetParLimits(2,sigma,2000);
  //  func->FixParameter(3,sigma);
  func->SetNpx(2000);

  float par[6];

  int imin = 25;
  int imax = 48;

  double xPt[2000];
  double xPtE[2000];
  double xEta[2000];
  double xEtaE[2000];
  double xMean[4][2000];
  double xSigma[4][2000];
  double xMeanE[4][2000];
  double xSigmaE[4][2000];
  int np = 0;
  
  for(int i=imax; i>= imin; i--){
    for(int j=0; j< 16; j++){
      xEta[np] = -0.75 + 0.1*j;
      xEtaE[np] = 0;
      getResEta(par, "Pi", j, i);
      xPt[np] = -par[3];
      xPtE[np] = 0;
      float piRes = sqrt(fpi->Eval(xPt[np],xEta[np]) * fpi->Eval(xPt[np],xEta[np]) + sigma*sigma);
      xMean[0][np] = par[1];
      xSigma[0][np] = par[2] / piRes;
      xMeanE[0][np] = par[4];
      xSigmaE[0][np] = par[5] / piRes;
      
      getResEta(par, "Ka", j, i);
      float kaRes = sqrt(fka->Eval(xPt[np],xEta[np]) * fka->Eval(xPt[np],xEta[np]) + sigma*sigma);
      xMean[1][np] = par[1];
      xSigma[1][np] = par[2] / kaRes;
      xMeanE[1][np] = par[4];
      xSigmaE[1][np] = par[5] / kaRes;
      
      getResEta(par, "Pr", j, i);
      float prRes = sqrt(fpr->Eval(xPt[np],xEta[np]) * fpr->Eval(xPt[np],xEta[np]) + sigma*sigma);
      xMean[2][np] = par[1];
      xSigma[2][np] = par[2] / prRes;
      xMeanE[2][np] = par[4];
      xSigmaE[2][np] = par[5] / prRes;

      /*
      getResEta(par, "De", j, i);
      float deRes = sqrt(fde->Eval(xPt[np],xEta[np]) * fde->Eval(xPt[np],xEta[np]) + sigma*sigma);
      xMean[3][np] = par[1];
      xSigma[3][np] = par[2] / deRes;
      xMeanE[3][np] = par[4];
      xSigmaE[3][np] = par[5] / deRes;
      */
      np++;
    }
  }
  
  TCanvas *c = new TCanvas("cpions","cpions");
  c->Divide(2,1);
  c->cd(1);
  TGraph2DErrors *gPiM = new TGraph2DErrors(np,xPt,xEta,xMean[0],xPtE,xEtaE,xMeanE[0]);
  gPiM->SetName("gPiM");
  gPiM->SetMarkerStyle(20);
  gPiM->Draw("SURF2");
  gPiM->SetLineColor(1);
  gPiM->SetMarkerColor(1);
  c->cd(2);
  TGraph2DErrors *gPiS = new TGraph2DErrors(np,xPt,xEta,xSigma[0],xPtE,xEtaE,xSigmaE[0]);
  gPiS->SetName("gPiS");
  gPiS->SetMarkerStyle(20);
  gPiS->Draw("SURF2");
  gPiS->SetLineColor(1);
  gPiS->SetMarkerColor(1);

  gPiM->SetMinimum(-50);
  gPiM->SetMaximum(50);
  gPiS->SetMinimum(0);
  gPiS->SetMaximum(2);
  
  c = new TCanvas("ckaons","ckaons");
  c->Divide(2,1);
  c->cd(1);
  TGraph2DErrors *gKaM = new TGraph2DErrors(np-16*7,xPt,xEta,xMean[1],xPtE,xEtaE,xMeanE[1]);
  gKaM->SetName("gKaM");
  gKaM->SetMarkerStyle(20);
  gKaM->Draw("SURF2");
  gKaM->SetLineColor(2);
  gKaM->SetMarkerColor(2);
  c->cd(2);
  TGraph2DErrors *gKaS = new TGraph2DErrors(np- 16*7,xPt,xEta,xSigma[1],xPtE,xEtaE,xSigmaE[1]);
  gKaS->SetName("gKaS");
  gKaS->SetMarkerStyle(20);
  gKaS->Draw("SURF2");
  gKaS->SetLineColor(2);
  gKaS->SetMarkerColor(2);

  gKaM->SetMinimum(-50);
  gKaM->SetMaximum(50);
  gKaS->SetMinimum(0);
  gKaS->SetMaximum(2);

  c = new TCanvas("cprotons","cprotons");
  c->Divide(2,1);
  c->cd(1);
  TGraph2DErrors *gPrM = new TGraph2DErrors(np,xPt,xEta,xMean[2],xPtE,xEtaE,xMeanE[2]);
  gPrM->SetName("gPrM");
  gPrM->SetMarkerStyle(20);
  gPrM->Draw("SURF2");
  gPrM->SetLineColor(4);
  gPrM->SetMarkerColor(4);
  c->cd(2);
  TGraph2DErrors *gPrS = new TGraph2DErrors(np,xPt,xEta,xSigma[2],xPtE,xEtaE,xSigmaE[2]);
  gPrS->SetName("gPrS");
  gPrS->SetMarkerStyle(20);
  gPrS->Draw("SURF2");
  gPrS->SetLineColor(4);
  gPrS->SetMarkerColor(4);

  gPrM->SetMinimum(-50);
  gPrM->SetMaximum(50);
  gPrS->SetMinimum(0);
  gPrS->SetMaximum(2);

  return;
  c = new TCanvas("cdeuterons","cdeuterons");
  c->Divide(2,1);
  c->cd(1);
  TGraph2DErrors *gDeM = new TGraph2DErrors(np,xPt,xEta,xMean[3],xPtE,xEtaE,xMeanE[3]);
  gDeM->SetName("gDeM");
  gDeM->SetMarkerStyle(20);
  gDeM->Draw("SURF2");
  gDeM->SetLineColor(6);
  gDeM->SetMarkerColor(6);
  c->cd(2);
  TGraph2DErrors *gDeS = new TGraph2DErrors(np,xPt,xEta,xSigma[3],xPtE,xEtaE,xSigmaE[3]);
  gDeS->SetName("gDeS");
  gDeS->SetMarkerStyle(20);
  gDeS->Draw("SURF2");
  gDeS->SetLineColor(6);
  gDeS->SetMarkerColor(6);

  gDeM->SetMinimum(-50);
  gDeM->SetMaximum(50);
  gDeS->SetMinimum(0);
  gDeS->SetMaximum(2);
}
