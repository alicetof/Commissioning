bool checkNew = false;

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

void checkmap(){
  TFile *f = new TFile("TOF/Calib/ChannelCalib/snapshot.root");
  o2::dataformats::CalibTimeSlewingParamTOF *o = (o2::dataformats::CalibTimeSlewingParamTOF *) f->Get("ccdb_object");
  int nch = 91*96*18;
  int ninsec = 91*96;
  int nact=0;
  int nact2=0;
  int nact3=0;
  TH2F *h = new TH2F("h","calibrated;sector;",48*18,0,18,91*2,0,91);
  TH2F *h2 = new TH2F("h2","FEE map;sector;",48*18,0,18,91*2,0,91);
  TH2F *h3 = new TH2F("h2","FEE map && calibrated;sector;",48*18,0,18,91*2,0,91);

  TFile *f2;
  if(! checkNew){
    f2 = new TFile("TOF/Calib/FEELIGHT/snapshot.root");
  } else {
    f2 = new TFile("TOF/Calib/FEELIGHT/snapshot2.root");
  }

  o2::tof::TOFFEElightInfo *o2 = (o2::tof::TOFFEElightInfo *) f2->Get("ccdb_object");


  for(int i=0; i < nch; i++){
    float sector = i/ninsec;
    float istrip = (i%ninsec) / 96;
    int chLoc = i%96;
    int padz = chLoc/48;
    int padx = chLoc%48;
    sector += (padx+0.5)/48.;
    istrip += (padz+0.5)/2.;

    if(masking(i)){
      o2->mChannelEnabled[i] = false;
    }

    bool isGoodFEE = o2->getChannelEnabled(i);
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
  printf("nActiveFEE&Cal = %d\n",nact);

  new TCanvas;
  h3->Draw("colz");

  if(! checkNew){
    TFile *fout = new TFile("TOF/Calib/FEELIGHT/snapshot2.root","RECREATE");
    fout->WriteObjectAny(o2, o2->Class(), "ccdb_object");
    fout->Close();
  }
}
