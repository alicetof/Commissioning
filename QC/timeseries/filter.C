void filter(const char* fname="time_series_tracks_0.root",const char* fnameOut="filtered.root"){
  int typoV[1000];
  int intV[1000]; // 0
  float floatV[1000]; // 1
  double doubleV[1000]; // 2
  UShort_t ushortV[1000]; // 3
  UInt_t uintV[1000]; // 4

  float *pt;
  int *ncl;

  //  float downscaleextra = 1;

  TGrid::Connect("alien://");

  TFile *fts = TFile::Open(fname);
  TTree *t = (TTree *) fts->Get("treeTimeSeries");

  TFile *fout = new TFile(fnameOut,"RECREATE");
  TTree *tout = new TTree("tree","tree");

  char hasITS;
  t->SetBranchAddress("hasITSTPC",&hasITS);
  
  FILE *f = fopen("filter","r");
  char typeV[30],labelIn[100],labelOut[100];
  int ndim;
  int np=0;
  while(fscanf(f,"%s %d %s %s",typeV,&ndim,labelIn,labelOut) == 4){
    printf("new\n");
    TString a(typeV);
    TString b(labelIn);
    if(a.Contains("int")){
      printf("%s -> int\n",typeV);
      typoV[np] = 0;
      t->SetBranchAddress(labelIn,&(intV[np]));
      if(ndim==1){
	tout->Branch(labelOut,&(intV[np]));
      } else {
	tout->Branch(labelOut,&(intV[np]),Form("%s[%d]",labelOut,ndim));
      }

      if(b.Contains("ncl")){
	ncl = &(intV[np]);
      }

    } else if(a.Contains("float")){
      printf("%s -> float\n",typeV);
      typoV[np] = 1;
      t->SetBranchAddress(labelIn,&(floatV[np]));
      if(ndim==1){
	tout->Branch(labelOut,&(floatV[np]));
      } else {
	tout->Branch(labelOut,&(floatV[np]),Form("%s[%d]",labelOut,ndim));
      }

      if(b.Contains("pt") && !b.Contains("qpt")){
	pt = &(floatV[np]);
      }
      
    } else if(a.Contains("double")){
      printf("%s -> double\n",typeV);
      typoV[np] = 2;
      t->SetBranchAddress(labelIn,&(doubleV[np]));
      if(ndim==1){
	tout->Branch(labelOut,&(doubleV[np]));
      } else {
	tout->Branch(labelOut,&(doubleV[np]),Form("%s[%d]",labelOut,ndim));
      }
    } else if(a.Contains("UShort_t")){
      printf("%s -> ushort\n",typeV);
      typoV[np] = 2;
      t->SetBranchAddress(labelIn,&(ushortV[np]));
      if(ndim==1){
	tout->Branch(labelOut,&(ushortV[np]));
      } else {
	tout->Branch(labelOut,&(ushortV[np]),Form("%s[%d]",labelOut,ndim));
      }
    } else if(a.Contains("UInt_t")){
      printf("%s -> uint\n",typeV);
      typoV[np] = 2;
      t->SetBranchAddress(labelIn,&(uintV[np]));
      if(ndim==1){
	tout->Branch(labelOut,&(uintV[np]));
      } else {
	tout->Branch(labelOut,&(uintV[np]),Form("%s[%d]",labelOut,ndim));
      }
    }
    np+=ndim;
  }
  
  // factor 10 downscale for pT < 1
  for(int i=0; i < t->GetEntries();i++){
    t->GetEvent(i);

    /*
    if(gRandom->Rndm() < downscaleextra){
      continue;
    }
    */
    
    if(! hasITS){
      continue;
    }
    
    if(*pt > 40 || *ncl < 100){
      continue;
    }
    if(1 || *pt > 1 || gRandom->Rndm() < 0.1){
      tout->Fill();
    }
  }
  
  fout->cd();
  tout->Write();
  fout->Close();
  
}
