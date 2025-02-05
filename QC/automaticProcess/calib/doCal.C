
#include "TFile.h"
#include "DataFormatsTOF/CalibTimeSlewingParamTOF.h"
#include "DataFormatsTOF/CalibInfoTOFshort.h"
#include "TOFBase/Utils.h"
#include "TF1.h"

std::vector<o2::dataformats::CalibInfoTOFshort> mVectC, *mPvectC = &mVectC;
TChain *mTreeFit;
int mNfits = 0;
const int NCHPERBUNCH = o2::tof::Geo::NCHANNELS / o2::tof::Geo::NSECTORS / 16;
static const int NMINTOFIT = 100;

float offsets[o2::tof::Geo::NCHANNELS];
float fraction[o2::tof::Geo::NCHANNELS];
float sigmaCh[o2::tof::Geo::NCHANNELS];
int run=0;

float frThreshold = 0.55; // threshold to define problematic channels
float sigmaThreshold = 500; // threshold to define problematic channels

o2::dataformats::CalibTimeSlewingParamTOF *ts;

int extractNewTimeSlewing(o2::dataformats::CalibTimeSlewingParamTOF* oldTS);
int findBC(o2::dataformats::CalibTimeSlewingParamTOF* oldTS);

TH2F *h2d[18],*h2check, *h2checkBC;
TH1F *h1d,*hfr,*hcheckBC;
TH1F *hfrDist,*hOffsetDist,*hsigmaDist;

void doCal(int runarg){
  TF1 *fMod = new TF1("fMod","[0]*(1 + gaus(1) + gaus(4) + gaus(7))"); // modulation of fraction vs channel
  fMod->SetNpx(5000);
  fMod->SetLineColor(8);
  
  fMod->SetParameter(0,1);
  fMod->SetParameter(1,-0.101372);
  fMod->SetParameter(2,0);
  fMod->SetParameter(3,102.644);
  fMod->SetParameter(4,-0.0610555);
  fMod->SetParameter(5,1507.81);
  fMod->SetParameter(6,98.553);
  fMod->SetParameter(7,-0.0908738);
  fMod->SetParameter(8,3501.12);
  fMod->SetParameter(9,60.0367);

  for(int isec=0; isec < 18; isec++){
    h2d[isec] = new TH2F(Form("h2d_%d",isec),";nchannel;#Deltat (ps)", o2::tof::Geo::NCHANNELS/18, 0, o2::tof::Geo::NCHANNELS/18, 12500, -100000, 150000);
  }
  h2check = new TH2F("h2check",";nchannel;#Deltat (ps)", o2::tof::Geo::NCHANNELS, 0, o2::tof::Geo::NCHANNELS, 500, -5000, 5000);
  h2checkBC = new TH2F("h2checkBC",";nchannel;#Deltat (ps)", o2::tof::Geo::NCHANNELS, 0, o2::tof::Geo::NCHANNELS, 200, -100000, 100000);
  hcheckBC = new TH1F("hcheckBC",";nchannel;#Deltat (ps)", o2::tof::Geo::NCHANNELS, 0, o2::tof::Geo::NCHANNELS);
  h1d = new TH1F("h1d",";nchannel;#Deltat (ps)", o2::tof::Geo::NCHANNELS, 0, o2::tof::Geo::NCHANNELS);
  hfr = new TH1F("hfr",";nchannel;Fraction Under Peak", o2::tof::Geo::NCHANNELS, 0, o2::tof::Geo::NCHANNELS);

  hfrDist = new TH1F("hfrDist",";Fraction Under Peak", 1000, 0.1, 1.1);
  hsigmaDist = new TH1F("hsigmaDist",";sigma (ps)", 100, 0, 500);
  hOffsetDist = new TH1F("hOffsetDist",";Offset (ps))", 10000, -100000, 100000);
  
  run = runarg;
  for(int i=0; i < o2::tof::Geo::NCHANNELS; i++){
    offsets[i] = 0;
  }

  TFile *fCal = new TFile("newtsNew.root");
  ts = (o2::dataformats::CalibTimeSlewingParamTOF *) fCal->Get("ccdb_object");

//  findBC(ts);
//  h2checkBC->Reset();
  extractNewTimeSlewing(ts);

  TF1 *fitFunc = new TF1("fTOFfit", "gaus");

  fitFunc->SetParameter(0, 100);
  fitFunc->SetParameter(1, 0);
  fitFunc->SetParameter(2, 200);
  fitFunc->SetParLimits(2, 150,450);

  int chPerSec = 91*96;
  for(int i=0; i < o2::tof::Geo::NCHANNELS; i++){
    if(!(i%10000)){
      printf("fitted: %d/%d\n",i,o2::tof::Geo::NCHANNELS);
    }
    int isec = i/chPerSec;
    int iloc = i % chPerSec;
    TH1D *htmp = h2d[isec]->ProjectionY("tmp",iloc+1,iloc+1);
    int binmax = htmp->GetMaximumBin();
    float xpeak = htmp->GetBinCenter(binmax);
    fitFunc->SetParameter(0, 100);
    fitFunc->SetParameter(1, xpeak);
    fitFunc->SetParLimits(1, xpeak-300, xpeak+300);
    fitFunc->SetParameter(2, 200);
    fitFunc->SetParLimits(2, 150,450);
    htmp->Fit(fitFunc,"WW,Q0,B","",xpeak-300,xpeak+300);

    double total = htmp->Integral(1, htmp->GetNbinsX());

    if(total < NMINTOFIT){ // isProblematic
      fraction[i] = -1;
      hfrDist->Fill(fraction[i]);
      sigmaCh[i] = 0;
      continue;
    }
    
    float sigma = fabs(fitFunc->GetParameter(2));
    if(sigma < 150 || sigma > 450){
       printf("sigma = %.0f\n",sigma);
    }
    sigmaCh[i] = sigma;
    hsigmaDist->Fill(sigma);
    
    if(fitFunc->GetParameter(1) < -99000 || fitFunc->GetParameter(1) > 99000){ // isProblematic
      fraction[i] = -1;
      hfrDist->Fill(fraction[i]);
      continue;
    }
    
    offsets[i] = fitFunc->GetParameter(1);
    int binLow = htmp->FindBin(offsets[i]-5*sigma);
    int binHigh = htmp->FindBin(offsets[i]+5*sigma);

    int ich = i % 8736;
    if(ich > 4368){
      ich = 8736 - ich;
    }
    fraction[i] = htmp->Integral(binLow,binHigh) / total;// / fMod->Eval(ich);

    hfrDist->Fill(fraction[i]);
    hfr->SetBinContent(i+1, fraction[i]);

    h1d->SetBinContent(i+1, offsets[i]);
    hOffsetDist->Fill(offsets[i]);
  }

  printf("Adjust TS\n");
  unsigned short oldtot[10000],newtot[1];
  short oldcorr[10000],newcorr[1];
  newtot[0] = 0;
  newcorr[0] = 0;

  TF1 *fitThr = new TF1("fTOFfitTh", "gaus");
  
  // define problematics
  fitThr->SetParameter(0,100);
  fitThr->SetParameter(2,0.5);
  fitThr->SetParameter(1,0.3);
  hfrDist->Fit(fitThr,"WW,Q0","",0,1);
  frThreshold = fitThr->GetParameter(1) - fitThr->GetParameter(2)*7; // it was 5
  fitThr->SetParameter(0,100);
  fitThr->SetParameter(2,200);
  fitThr->SetParameter(1,100);
  hsigmaDist->Fit(fitThr,"WW,Q0","",0,500);
  sigmaThreshold = fitThr->GetParameter(1) + fitThr->GetParameter(2)*8; // it was 7

  printf("frThreshold = %.2f\n",frThreshold);
  printf("sigmaThreshold = %.0f ps\n",sigmaThreshold);

  for(int i=0; i < o2::tof::Geo::NCHANNELS; i++){
    int sector = i / chPerSec;
    int chInSec = i % chPerSec;
    int istart = ts->getStartIndexForChannel(sector, chInSec);
    int istop = ts->getStopIndexForChannel(sector, chInSec);
    const std::vector<std::pair<unsigned short, short>>& vect = ts->getVector(sector);
    int count = 0;
    for (int j = istart; j < istop; j++) {
      oldtot[count] = vect[j].first;
      oldcorr[count] = vect[j].second;
      count++;
    }

    if(fraction[i] < frThreshold || sigmaCh[i] > sigmaThreshold || sigmaCh[i] < 40){
      fraction[i] = -1;
    }

    int nbinPrev = istop - istart;
    ts->updateOffsetInfo(i, offsets[i]);
    ts->setFractionUnderPeak(sector, chInSec, fraction[i]);
    ts->setSigmaPeak(sector, chInSec, sigmaCh[i]);
  }

  printf("Store TS\n");

  TFile *fsl = new TFile(Form("TOF/Calib/ChannelCalib/%d_ts.root",run),"RECREATE");
  fsl->WriteObjectAny(ts, ts->Class(), "ccdb_object");
  fsl->Close();
  

  TFile *fout = new TFile(Form("%d_offsets.root",run),"RECREATE");
  h2check->Write();
  h2checkBC->Write();
  hcheckBC->Write();
  for(int isec=0; isec < 18; isec++){
    h2d[isec]->Write();
  }
  hsigmaDist->Write();
  h1d->Write();
  hfr->Write();
  hfrDist->Write();
  hOffsetDist->Write();
  fout->Close();

}


int extractNewTimeSlewing(o2::dataformats::CalibTimeSlewingParamTOF* oldTS)
{
  if (!oldTS) { // objects were not defined -> to nothing
    return 1;
  }

  if (mTreeFit) { // remove previous tree
    delete mTreeFit;
  }

  mTreeFit = new TChain("treeCollectedCalibInfo", "treeCollectedCalibInfo");

  auto retval = system(Form("ls accumulated/%d*.root >listaCal%d",run,run)); // create list of calibInfo accumulated
  FILE* f = fopen(Form("listaCal%d",run), "r");

  if (!f) { // no inputs -> return
    printf("No inputs\n");
    return 2;
  }

  char namefile[50];
  while (fscanf(f, "%s", namefile) == 1) {
    mTreeFit->AddFile(namefile);
  }

  if (!mTreeFit->GetEntries()) { // return if no entry available
    printf("No entries\n");
    return 3;
  }

  mTreeFit->SetBranchAddress("TOFCollectedCalibInfo", &mPvectC);

  long fr10 = mTreeFit->GetEntries()/10 - 1;

  int chPerSec = 91*96;
  
  for (long i = 0; i < mTreeFit->GetEntries(); i++) {
    if(!(i % fr10)){
      printf("%ld/%lld\n",i,mTreeFit->GetEntries());
    }
    mTreeFit->GetEvent(i);
    int ch = i % o2::tof::Geo::NCHANNELS;
    int isec = ch / chPerSec;
    int chLoc = ch % chPerSec;
    float dtold = 1000000;
    for(const auto& obj : mVectC){
      float dt = obj.getDeltaTimePi() - oldTS->evalTimeSlewing(ch, obj.getTot());
      if(std::abs(dt-dtold) < 10){ // avoid repetion of some entry (sometimes happening)
        dtold = dt;
        continue;
      }
      if(obj.getFlags() != 2){ // 8 means ITS-TPC-TRD tracks (2 means ITS-TPC)
//         continue;
         float strip = chLoc/96.;
//         dt -= 5*abs(strip-45) + 23.82;
       }
      dtold = dt;
      h2d[isec]->Fill(chLoc, dt);
      h2check->Fill(ch, dt);
//      if(ch==1987){
//        printf("%f %f\n",dt,obj.getDeltaTimePi());
//      }
      h2checkBC->Fill(ch, dt);
    }
  }

  return 0;
}

int findBC(o2::dataformats::CalibTimeSlewingParamTOF* oldTS)
{
  if (!oldTS) { // objects were not defined -> to nothing
    return 1;
  }

  if (mTreeFit) { // remove previous tree
    delete mTreeFit;
  }

  mTreeFit = new TChain("treeCollectedCalibInfo", "treeCollectedCalibInfo");

  auto retval = system(Form("ls accumulated/%d*.root >listaCal%d",run,run)); // create list of calibInfo accumulated
  FILE* f = fopen(Form("listaCal%d",run), "r");

  if (!f) { // no inputs -> return
    printf("No inputs\n");
    return 2;
  }

  char namefile[50];
  while (fscanf(f, "%s", namefile) == 1) {
    mTreeFit->AddFile(namefile);
  }

  if (!mTreeFit->GetEntries()) { // return if no entry available
    printf("No entries\n");
    return 3;
  }

  mTreeFit->SetBranchAddress("TOFCollectedCalibInfo", &mPvectC);

  long fr10 = mTreeFit->GetEntries()/10 - 1;

  int chPerSec = 91*96;
  
  for (long i = 0; i < mTreeFit->GetEntries(); i++) {
    if(!(i % fr10)){
      printf("first round) %ld/%lld\n",i,mTreeFit->GetEntries());
    }
    mTreeFit->GetEvent(i);
    int ch = i % o2::tof::Geo::NCHANNELS;
    int isec = ch / chPerSec;
    int chLoc = ch % chPerSec;
    for(const auto& obj : mVectC){
      float dt = obj.getDeltaTimePi() - oldTS->evalTimeSlewing(ch, obj.getTot());
      if(obj.getFlags() != 2){ // 8 means ITS-TPC-TRD tracks (2 means ITS-TPC)
//         continue;
         float strip = chLoc/96.;
//         dt -= 5*abs(strip-45) + 23.82;
       }
      h2checkBC->Fill(ch, dt);
    }
  }

  return 0;
}

