void reduce(int sampling=10, int adddisc = 100){
  FILE *f = fopen("fulllist","r");

  o2::tof::Diagnostic mDia, *mPdia = &mDia;
  std::vector<o2::dataformats::CalibInfoTOF> mVect, *mPvect = &mVect;

  char filename[100];
  TChain *t = new TChain("calibTOF");
  while(fscanf(f,"%s",filename) == 1){
    t->AddFile(filename);
  }
  t->SetBranchAddress("TOFCalibInfo", &mPvect);
  t->SetBranchAddress("TOFDiaInfo", &mPdia);

  TFile *fout = new TFile("calibTobeKept.root","RECREATE");
  TTree *tout = new TTree("calibTOF","calibTOF");
  tout->Branch("TOFCalibInfo",&mPvect);
  tout->Branch("TOFDiaInfo",&mPdia);

  int keep = adddisc * sampling;

  for(int i=0; i < t->GetEntries(); i+=sampling){
    t->GetEvent(i);
    if((i % keep) != 0){
       mVect.clear();
    }

    tout->Fill();
  }

  fout->cd();
  tout->Write();
  fout->Close();
}
