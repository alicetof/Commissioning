void process(){
  float tpcthr = 100000000;
  float minBetaGamma = 0.4;
  
  TChain *t = new TChain("tree");
  FILE *f = fopen("listaT","r");
  char fname[100];
  while(fscanf(f,"%s",fname) == 1){
    t->AddFile(fname);
  }
  TFile *feloss = TFile::Open("expDeDx.root");
  TH1D *eloss = nullptr;
  if(feloss){
    eloss = (TH1D *) feloss->Get("hDeDxExp_px");
    eloss->SetName("eloss");
    int iminBetaGamma = eloss->FindBin(minBetaGamma);
    for(int i=1; i <= eloss->GetNbinsX(); i++){
      if(i < iminBetaGamma) {
	eloss->SetBinContent(i,eloss->GetBinContent(iminBetaGamma));
      }
    }
  }

  TFile *fEtamism = new TFile("etaMism.root");
  TProfile *hEtaMism = (TProfile *) fEtamism->Get("ccdb_object");
  TFile *fMism = new TFile("mism.root");
  TH2F *hMism = (TH2F *) fMism->Get("ccdb_object");

  const int netaBins = 21;
  TH1D *hMismPro[netaBins];
  for(int i=0; i < netaBins; i++){
    hMismPro[i] = hMism->ProjectionY(Form("mism_pro%d",i),i,i);
  }

  double tof; t->SetBranchAddress("tof",&tof);
  float texp[9]; t->SetBranchAddress("texp",texp);
  float pt; t->SetBranchAddress("pt",&pt);
  float dx; t->SetBranchAddress("dx",&dx);
  float dz; t->SetBranchAddress("dz",&dz);
  float vtime; t->SetBranchAddress("vertex_time",&vtime);
  int ncl; t->SetBranchAddress("ncl",&ncl);
  UShort_t mult;  t->SetBranchAddress("vertex_nContributors",&mult);
  UInt_t mask; t->SetBranchAddress("TOFmask",&mask);
  float tgl; t->SetBranchAddress("tgl",&tgl);
  float phi; t->SetBranchAddress("phi",&phi);
  float dedx; t->SetBranchAddress("dedx",&dedx);
  float qpt; t->SetBranchAddress("qpt",&qpt);

  double bctime = o2::tof::Geo::BC_TIME_INPS;
  double bctimeInv = o2::tof::Geo::BC_TIME_INPS_INV;

  long nev = t->GetEntries();

  const int nbinEta = 16;
  float etaBin = 0.1;
  float minEta = -etaBin*nbinEta/2;
  float maxEta = etaBin*nbinEta/2;
  
  TH2F *hPiEta[nbinEta];
  TH2F *hKaEta[nbinEta];
  TH2F *hPrEta[nbinEta];
  TH2F *hDeEta[nbinEta];
  TH2F *hPiEtaM[nbinEta];
  TH2F *hKaEtaM[nbinEta];
  TH2F *hPrEtaM[nbinEta];
  TH2F *hDeEtaM[nbinEta];
  for(int i=0; i < nbinEta; i++){
    hPiEta[i] = new TH2F(Form("hPiEta_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hKaEta[i] = new TH2F(Form("hKaEta_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hPrEta[i] = new TH2F(Form("hPrEta_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hDeEta[i] = new TH2F(Form("hDeEta_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hPiEtaM[i] = new TH2F(Form("hPiEtaM_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hKaEtaM[i] = new TH2F(Form("hKaEtaM_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hPrEtaM[i] = new TH2F(Form("hPrEtaM_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
    hDeEtaM[i] = new TH2F(Form("hDeEtaM_%d",i),Form("%.1f < #eta < %.1f;p (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",minEta,maxEta),100,-5,5,400,-5000,5000);
  }
  
  TProfile *hEff = new TProfile("hEff","|charge|=1;p_{T} (GeV/c);#varepsilon",1000,-5,5);
  TProfile *hEff2 = new TProfile("hEff2","|charge|=2;p_{T} (GeV/c);#varepsilon",1000,-5,5);
  TH1F *hQPt = new TH1F("hQPt",";Q/p_{T}",100,-10,10);
  TH2F *hPi = new TH2F("hPi",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hKa = new TH2F("hKa",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{K} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hPr = new TH2F("hPr",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{p} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hDe = new TH2F("hDe",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{D} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hTr = new TH2F("hTr",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{T} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-10000,10000);
  TH2F *hHe = new TH2F("hHe",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{^{3}He} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hAl = new TH2F("hAl",";p_{T} (GeV/c); t_{TOF} - t_{exp}^{^{4}He} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hPiTPC = new TH2F("hPiTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-5,5,300,-100,100);
  TH2F *hKaTPC = new TH2F("hKaTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-5,5,300,-100,100);
  TH2F *hPrTPC = new TH2F("hPrTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-5,5,300,-100,100);
  TH2F *hDeTPC = new TH2F("hDeTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-5,5,300,-100,100);
  TH2F *hTrTPC = new TH2F("hTrTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-5,5,300,-100,100);
  TH2F *hHeTPC = new TH2F("hHeTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-10,10,300,-100,100);
  TH2F *hAlTPC = new TH2F("hAlTPC",";p_{T} (GeV/c); dE/dx - dE/dx^{exp}",100,-10,10,300,-100,100);
  TH2F *hMass = new TH2F("hMass","|charge|=1;Mass (GeV/c^{2});p_{T}",2000,-4,4,50,0,5);
  TH2F *hMass2 = new TH2F("hMass2","|charge|=2;Mass (GeV/c^{2});p_{T}",3000,-6,6,50,0,10);
  TH2F *hPiM = new TH2F("hPiM",";p_{T} (GeV/c); t_{mism} - t_{exp}^{#pi} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hKaM = new TH2F("hKaM",";p_{T} (GeV/c); t_{mism} - t_{exp}^{K} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hPrM = new TH2F("hPrM",";p_{T} (GeV/c); t_{mism} - t_{exp}^{p} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hDeM = new TH2F("hDeM",";p_{T} (GeV/c); t_{mism} - t_{exp}^{D} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-2000,10000);
  TH2F *hTrM = new TH2F("hTrM",";p_{T} (GeV/c); t_{mism} - t_{exp}^{T} - t_{0}^{FT0-AC} (ps)",100,-5,5,300,-10000,10000);
  TH2F *hMassM = new TH2F("hMassM","|charge|=1;Mass (GeV/c^{2});p_{T}",2000,-4,4,50,0,5);
  TH2F *hDeDx = new TH2F("hDeDx","|charge|=1;#beta#gamma = p/(M_{TOF} c);dE/dx_{TPC}",400,-10,10,1600,0,800);
  TProfile *hDeDxExp = new TProfile("hDeDxExp","|charge|=1;#beta#gamma = p/(M_{TOF} c);dE/dx_{TPC}",400,0,20);
  TH2F *hDeDxTh = new TH2F("hDeDxTh","|charge|=1;#beta#gamma = p/(M_{TOF} c);dE/dx_{TPC} - dE/dx_{expected} ",400,-10,10,1000,-200,200);
  TH2F *hDeDxThM = new TH2F("hDeDxThM","|charge|=1, p < 2 GeV/c;M_{TOF} (GeV/c^{2});dE/dx_{TPC} - dE/dx_{expected}",1000,-4,4,500,-200,200);
  TH2F *hDeDxThMr = new TH2F("hDeDxThMr",";M_{TOF} (GeV/c^{2});dE/dx_{TPC} / dE/dx_{expected}",1000,-5,5,500,0,10);
  
  for(long i=0; i < nev; i++){
    t->GetEvent(i);
    hQPt->Fill(qpt);
    float charge = qpt/std::abs(qpt);
    float pt_z = 1./qpt;

    float tglHalf = (sqrt(1 + tgl*tgl) - 1)/tgl;
    float eta = - TMath::Log((1 + tglHalf)/(1 - tglHalf));
    float etaAbs = TMath::Abs(eta);
    if(etaAbs > 0.8){
      continue;
    }

    float p_z = std::abs(pt_z)*TMath::CosH(eta);
    if(dedx > 200 && dedx > 600./p_z/p_z){ // this is He
      charge *= 2; 
    }
    pt = charge*pt_z;
    
    if(!(i % (nev/20))){
      printf("%ld/%ld\n",i,nev);
    }

    if(ncl < 130){
      continue;
    }

    if(std::abs(charge) < 1.5){
      hEff->Fill(pt_z,texp[2] > 1);
    } else {
      hEff2->Fill(pt_z,texp[2] > 1);
    }
    
    if(texp[2] < 1){
      continue;
    }

    float dvtime = vtime*1E6 - tof;
    if(std::abs(dvtime) > 20E3){
//      continue;
    }

    bool multiHitX = mask&1;
    bool multiHitZ = mask&2;
    bool badDy = mask&4;
    bool multistrip = mask&8;
    bool notinpad = mask&16;
    bool chiGT3 = mask&32;
    bool chiGT5 = mask&64;
    bool hasT0sameBC = mask&128;
    bool hasT0_1BCbefore = mask&256;
    bool hasT0_2BCbefore = mask&512;

    if(badDy || notinpad){
      continue;
    }

    if(!hasT0sameBC){
      continue;
    }

    if(hasT0_1BCbefore || hasT0_2BCbefore){
      continue;
    }

    if(multiHitX || multiHitZ){
      continue;
    }

    if(multiHitZ){
      tof -= 0;
    }
    if(multiHitX){
      tof -= 0;
    }
    
    double t0 = int((tof-10000) * bctimeInv)*bctime;
    double beta = texp[0]/(tof-t0);

    int ibinY = hMism->GetXaxis()->FindBin(eta);
    float loverc = hEtaMism->GetBinContent(ibinY);
    float tmism = hMismPro[ibinY-1]->GetRandom() + loverc;
    while(tmism > texp[6] + 2000){
      tmism = hMismPro[ibinY-1]->GetRandom() + loverc;
    }
    double betaM = texp[0]/(tmism);
    if(std::abs(charge) < 1.5){
      float m_z = p_z/beta*sqrt(std::abs(1-beta*beta))*charge;
      float nsigmComp = (dedx - eloss->Interpolate(p_z/std::abs(m_z)))/10;
      if((nsigmComp > -3 && nsigmComp < 5) || 1){//std::abs(nsigmComp) < 5){
	hPi->Fill(pt_z, tof - t0 -  texp[2]);
	hKa->Fill(pt_z, tof - t0 -  texp[3]);
	hPr->Fill(pt_z, tof - t0 -  texp[4]);
	if(eta > minEta && eta < maxEta){
	  int ibin = int((eta - minEta)/etaBin);
	  int ibin2 = 15 - int((eta - minEta)/etaBin);

	  hPiEta[ibin]->Fill(p_z*charge, tof - t0 -  texp[2]);
	  hPiEta[ibin2]->Fill(p_z*charge, tof - t0 -  texp[2]);
	  hPiEtaM[ibin]->Fill(p_z*charge, tmism -  texp[2]);
	  hPiEtaM[ibin2]->Fill(p_z*charge, tmism -  texp[2]);

	  hKaEta[ibin]->Fill(p_z*charge, tof - t0 -  texp[3]);
	  hKaEta[ibin2]->Fill(p_z*charge, tof - t0 -  texp[3]);
	  hKaEtaM[ibin]->Fill(p_z*charge, tmism -  texp[3]);
	  hKaEtaM[ibin2]->Fill(p_z*charge, tmism -  texp[3]);

	  hPrEta[ibin]->Fill(p_z*charge, tof - t0 -  texp[4]);
	  hPrEta[ibin2]->Fill(p_z*charge, tof - t0 -  texp[4]);
	  hPrEtaM[ibin]->Fill(p_z*charge, tmism -  texp[4]);
	  hPrEtaM[ibin2]->Fill(p_z*charge, tmism -  texp[4]);

	  hDeEta[ibin]->Fill(p_z*charge, tof - t0 -  texp[5]);
	  hDeEta[ibin2]->Fill(p_z*charge, tof - t0 -  texp[5]);
	  hDeEtaM[ibin]->Fill(p_z*charge, tmism -  texp[5]);
	  hDeEtaM[ibin2]->Fill(p_z*charge, tmism -  texp[5]);
	}

	if(std::abs(dedx - eloss->Interpolate(p_z/1.875)) < 15){
	  hDe->Fill(pt_z, tof - t0 -  texp[5]);
	}
	if(std::abs(dedx - eloss->Interpolate(p_z/2.809)) < 15){
  	  hTr->Fill(pt_z, tof - t0 -  texp[6]);
	}
	if(std::abs(tof - t0 - texp[2]) < 200 && std::abs(dedx- eloss->Interpolate(TMath::Min(p_z/0.139,6.))) < eloss->Interpolate(TMath::Min(p_z/0.139,6.))*tpcthr){
	  hPiTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z/0.139));
	  hDeDx->Fill(p_z/0.139*charge,dedx);
	  hDeDxExp->Fill(p_z/0.139,dedx);
	}
	if(std::abs(tof - t0 - texp[3]) < 200 && std::abs(dedx- eloss->Interpolate(p_z/0.493)) < eloss->Interpolate(p_z/0.493)*tpcthr && p_z < 2E6){
	  hKaTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z/0.493));
	  hDeDx->Fill(p_z/0.493*charge,dedx);
	  hDeDxExp->Fill(p_z/0.493,dedx);
	}
	if(std::abs(tof - t0 - texp[4]) < 200 && std::abs(dedx- eloss->Interpolate(p_z/0.938)) < eloss->Interpolate(p_z/0.938)*tpcthr && p_z < 3.5E6){
	  hPrTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z/0.938));
	  hDeDx->Fill(p_z/0.938*charge,dedx);
	  hDeDxExp->Fill(p_z/0.938,dedx);
	}
	if(std::abs(tof - t0 - texp[5]) < 1000 && std::abs(dedx- eloss->Interpolate(p_z/1.875)) < eloss->Interpolate(p_z/1.875)*tpcthr && p_z < 1.5E6){
	  hDeTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z/1.875));
	  hDeDx->Fill(p_z/1.875*charge,dedx);
	  hDeDxExp->Fill(p_z/1.875,dedx);
	}
	if(std::abs(tof - t0 - texp[6]) < 1000 && std::abs(dedx- eloss->Interpolate(p_z/2.808)) < eloss->Interpolate(p_z/2.808)*tpcthr && p_z < 1.5E6){
	  hTrTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z/2.808));
	  hDeDx->Fill(p_z/2.808*charge,dedx);
	  hDeDxExp->Fill(p_z/2.808,dedx);
	}
	hMass->Fill(m_z, pt);
	//     if(std::abs(p_z/m_z) > 1 || dedx > 70)
	hDeDxTh->Fill(p_z/m_z,dedx - eloss->Interpolate(p_z/std::abs(m_z)));
	if(std::abs(p_z)<2){
	  hDeDxThM->Fill(m_z,dedx - eloss->Interpolate(p_z/std::abs(m_z)));
	}
	hDeDxThMr->Fill(m_z,dedx / eloss->Interpolate(p_z/std::abs(m_z)));
      }
      m_z = p_z/betaM*sqrt(std::abs(1-betaM*betaM))*charge;
      nsigmComp = (dedx - eloss->Interpolate(p_z/std::abs(m_z)))/5.86;
      if((nsigmComp > -3 && nsigmComp < 5) || 1){//std::abs(nsigmComp) < 5){
	hPiM->Fill(pt_z, tmism-  texp[2]);
	hKaM->Fill(pt_z, tmism -  texp[3]);
	hPrM->Fill(pt_z, tmism -  texp[4]);
	hDeM->Fill(pt_z, tmism -  texp[5]);
	hTrM->Fill(pt_z, tmism -  texp[6]);
	hMassM->Fill(m_z, pt);
      }
    } else {
      float m_z = p_z/beta*sqrt(std::abs(1-beta*beta))*charge;
      hHe->Fill(pt_z*2, tof - t0 -  texp[7]);
      hAl->Fill(pt_z*2, tof - t0 -  texp[8]);
      if(std::abs(tof - t0 - texp[7]) < 1000 || 1){
	hHeTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z*2/2.81)*5);
      }
      if(std::abs(tof - t0 - texp[8]) < 1000 || 1){
	hAlTPC->Fill(p_z*charge, dedx- eloss->Interpolate(p_z*2/3.73)*5);
      }

      hMass2->Fill(m_z, pt);
      hDeDxThMr->Fill(m_z,dedx / eloss->Interpolate(p_z/std::abs(m_z)*2));
    }


  }

  TFile *fout = new TFile("summary.root","RECREATE");
  hEff->Write();
  hEff2->Write();
  hQPt->Write();
  hPi->Write();
  hKa->Write();
  hPr->Write();
  hDe->Write();
  hTr->Write();
  hHe->Write();
  hAl->Write();
  hPiM->Write();
  hKaM->Write();
  hPrM->Write();
  hDeM->Write();
  hTrM->Write();
  hMass->Write();
  hMass2->Write();
  hMassM->Write();
  hDeDx->Write();
  hDeDxTh->Write();
  hDeDxThM->Write();
  hDeDxThMr->Write();
  hPiTPC->Write();
  hKaTPC->Write();
  hPrTPC->Write();
  hDeTPC->Write();
  hTrTPC->Write();
  hHeTPC->Write();
  hAlTPC->Write();
  for(int i=0; i < nbinEta; i++){
    hPiEta[i]->Write();
    hKaEta[i]->Write();
    hPrEta[i]->Write();
    hDeEta[i]->Write();
    hPiEtaM[i]->Write();
    hKaEtaM[i]->Write();
    hPrEtaM[i]->Write();
    hDeEtaM[i]->Write();
  }
  fout->Close();

  /*
  TFile *expdedx = new TFile("expDeDx.root","RECREATE");
  hDeDxExp->Write();
  expdedx->Close();
  */
}
