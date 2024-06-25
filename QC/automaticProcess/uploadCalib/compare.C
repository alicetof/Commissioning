void compare(int run1, int run2){
  TFile *f1 = new TFile(Form("%d_ts.root",run1));
  o2::dataformats::CalibTimeSlewingParamTOF *tsN = (o2::dataformats::CalibTimeSlewingParamTOF *) f1->Get("ccdb_object");
  TFile *f2 = new TFile(Form("%d_ts.root",run2));
  o2::dataformats::CalibTimeSlewingParamTOF *tsO = (o2::dataformats::CalibTimeSlewingParamTOF *) f2->Get("ccdb_object");
  
  int nch = o2::tof::Geo::NCHANNELS;
  int nreal = o2::tof::Geo::NCHANNELS - (96 * 15 * 3);

  TH1F *hDT = new TH1F("hDT",";#Deltat (ps)",10000,-100000,100000);
  int nactive = 0;
  for(int i=0; i < nch; i++){
    

    if(tsN->isProblematic(i) || tsO->isProblematic(i)){
      continue;
    }

    float deltat = tsN->evalTimeSlewing(i,12) - tsO->evalTimeSlewing(i,12);

    hDT->Fill(deltat);
  }

  hDT->Draw();
  printf("Mean = %f\n",hDT->GetMean());
}
