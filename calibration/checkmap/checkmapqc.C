bool checkNew = false;
bool hitmap = true;

bool masking(int ch){
  if(checkNew){
    return false;
  }

  int ech = o2::tof::Geo::getECHFromCH(ch);
  int crate = o2::tof::Geo::getCrateFromECH(ech);
  int indexes[5];
  o2::tof::Geo::getVolumeIndices(ch, indexes);
  int& module = indexes[1];

  // for masking one crate (e.g. crate = 53)
//  if(crate == 53){
//    return true;
//  }

  // for masking one module (e.g. crate=53, module=1
//  if(crate == 52 && indexes[1] == 1){// && (module == 0 && module == 1)){
//    return true;
//  }

  return false;
}

void checkmapqc(){
  TFile *f = new TFile("TOF/Calib/ChannelCalib/snapshot.root");
  o2::dataformats::CalibTimeSlewingParamTOF *o = (o2::dataformats::CalibTimeSlewingParamTOF *) f->Get("ccdb_object");
  int nch = 91*96*18;
  int ninsec = 91*96;
  int nact=0;
  int nact2=0;
  int nact3=0;
  TH2F *h = new TH2F("h","calibrated;sector;",48*18,0,18,91*2,0,91);
  TH2F *h2 = new TH2F("h2","FEE map;sector;",48*18,0,18,91*2,0,91);
  TH2F *h3 = new TH2F("h3","FEE map && calibrated;sector;",48*18,0,18,91*2,0,91);

  TFile *f2;
  if(! checkNew){
    f2 = new TFile("TOF/Calib/FEELIGHT/snapshot.root");
  } else {
    f2 = new TFile("TOF/Calib/FEELIGHT/snapshot2.root");
  }

  o2::tof::TOFFEElightInfo *oo2 = (o2::tof::TOFFEElightInfo *) f2->Get("ccdb_object");

  TH2F *hhit = nullptr;

  if(hitmap){
    TFile *fqc = TFile::Open("QC.root");
    if(fqc){
      o2::quality_control::core::MonitorObjectCollection *mon = (o2::quality_control::core::MonitorObjectCollection *) fqc->Get("int/TOF/Digits");
      o2::quality_control::core::MonitorObject *m1 = (o2::quality_control::core::MonitorObject *) mon->FindObject("HitMapNoiseFiltered");
      hhit = (TH2F *) m1->getObject();
    } else {
      hitmap = false;
    }
  }

  for(int i=0; i < nch; i++){
    float sector = i/ninsec;
    float istrip = (i%ninsec) / 96;
    int chLoc = i%96;
    int padz = chLoc/48;
    int padx = chLoc%48;
    sector += (padx+0.5)/48.;
    istrip += (padz+0.5)/2.;

    if(masking(i)){
      oo2->mChannelEnabled[i] = false;
    }

    bool isGoodFEE = oo2->getChannelEnabled(i);
    bool isGoodCal = ! o->isProblematic(i);

    if(isGoodFEE){
      h2->Fill(sector,istrip);
      nact2++;
    }
    if(isGoodCal){
      h->Fill(sector,istrip);
      nact++;
    }

    if(isGoodCal && isGoodFEE){
      h3->Fill(sector,istrip);
      nact3++;
    }

  }
  TCanvas *c = new TCanvas;
  c->Divide(3,1);
  c->cd(1);
  h2->Draw("colz");
  h2->SetStats(0);
  c->cd(2);
  h->Draw("colz");
  h->SetStats(0);
  c->cd(3);
  h3->Draw("colz");
  h3->SetStats(0);
  printf("nActiveFEE = %d\n",nact2);
  printf("nActiveCal = %d\n",nact);
  printf("nActiveFEE&Cal = %d\n",nact3);

  TCanvas *c2 = new TCanvas;
  c2->Divide(1 + hitmap,1);
  c2->cd(1);
  h3->Draw("colz");

  if(! checkNew){
    TFile *fout = new TFile("TOF/Calib/FEELIGHT/snapshot2.root","RECREATE");
    fout->WriteObjectAny(oo2, oo2->Class(), "ccdb_object");
    fout->Close();
  }

  if(! hhit){
    return;
  }

  c2->cd(2);
  int nbinsx = hhit->GetNbinsX();
  int nbinsy = hhit->GetNbinsY();
  int rebinx = 48*18 / nbinsx;
  int rebiny = 91*2 / nbinsy;
  h3->RebinX(rebinx);
  h3->RebinY(rebiny);
  h3->Divide(h3);
  h->RebinX(rebinx);
  h->RebinY(rebiny);
  h->Divide(h);
  h2->RebinX(rebinx);
  h2->RebinY(rebiny);
  h2->Divide(h2);

  hhit->Scale(nbinsx*nbinsy*1./hhit->Integral());
  for(int i=0; i < nbinsx; i++){
    for(int j=0; j < nbinsy; j++){
      if(hhit->GetBinContent(i,j) < 0.1){
        hhit->SetBinContent(i,j,0);
      }
    }
  }

  hhit->Divide(hhit);
  // combine with problematics
  hhit->Multiply(h);

  hhit->Draw("colz");

  printf("nActiveHitMap (extrapolated) = %d - ratio wrt expected = %.3f\n",int(hhit->Integral() * nact3 / h3->Integral()), hhit->Integral() / h3->Integral());

  hhit->Add(h3, -1);
  hhit->SetTitle("hit map&problematics - reference map");

}
