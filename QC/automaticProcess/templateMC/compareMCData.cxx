TH1F* DivideUncorr(TH1F* h1, TH1F* h2);

void compareMCData(
    TString year_data = "2024",
    TString pass_data = "apass1",
    TString period_mc = "LHC24f4d",
    int part = 1,
    TString folder_mc = "."
    )
{

    int colors[] = {kBlack, kRed, kBlue, kGreen, kOrange, kMagenta, kCyan, kBlack, kGray, kViolet, kTeal + 2, kYellow, kMagenta,
                 kBlack, kRed, kBlue, kGreen, kOrange, kMagenta, kCyan, kBlack, kGray, kViolet, kTeal + 2, kYellow, kMagenta,
                 kBlack, kRed, kBlue, kGreen, kOrange, kMagenta, kCyan, kBlack, kGray, kViolet, kTeal + 2, kYellow, kMagenta,
                 kBlack, kRed, kBlue, kGreen, kOrange, kMagenta, kCyan, kBlack, kGray, kViolet, kTeal + 2, kYellow, kMagenta};

    system(Form("awk \'{print $1}\' jiratext.txt > listofruns"));
    std::vector<TString> run;
    FILE *file = fopen("listofruns","r");
    char *line;
    size_t len=0;
    while(getline(&line, &len, file) != -1) {
        run.push_back(line);
        run.back().Remove(run.back().Length() - 1);
    }
    fclose(file);

    const int nruns = run.size();

    TFile* fdata[nruns];
    TFile* fmc[nruns];
    TEfficiency* eEff_data[nruns], *eEff_mc[nruns];
    TH1F* hEff_data[nruns], *hEff_mc[nruns];
    TString name = "mEffPt_ITSTPC-ITSTPCTRD";
    TH1F* hratiodatamc[nruns];
    TH1F* htrending_data = new TH1F("htrending_data","",nruns,0,nruns);
    TH1F* htrending_mc = new TH1F("htrending_mc","",nruns,0,nruns);
    const int neffbins = 100; //eEff_data[0]->GetTotalHistogram()->GetNbinsX();

    for (int n_run = 0; n_run < nruns; n_run++)
    {
        fdata[n_run] = TFile::Open(Form("../../%s/%s/Run%s/rootfiles/%s.root", year_data.Data(), pass_data.Data(), run.at(n_run).Data(), name.Data()));
        fmc[n_run] = TFile::Open(Form("%s/Run%s_%s/rootfiles/%s.root", folder_mc.Data(), run.at(n_run).Data(), period_mc.Data(), name.Data()));

        if (!fdata[n_run]) {
            continue;
        }
        eEff_data[n_run] = (TEfficiency *)fdata[n_run]->Get("ccdb_object");

        if (!fmc[n_run]) {
            continue;
        }
        eEff_mc[n_run] = (TEfficiency *)fmc[n_run]->Get("ccdb_object");

        hEff_data[n_run] = new TH1F(Form("hEff_data_%s", run.at(n_run).Data()), "", neffbins, 0., 20);
        hEff_mc[n_run] = new TH1F(Form("hEff_mc_%s", run.at(n_run).Data()), "", neffbins, 0., 20);

        for (int ibin = 1; ibin <= neffbins; ibin++){

            float bincontent_data = 0.;
            float bincontent_mc = 0.;
            float binerror_data = 0.;
            float binerror_mc = 0.;
            if (eEff_data[n_run]){
                bincontent_data =  eEff_data[n_run]->GetEfficiency(ibin);
                binerror_data =  eEff_data[n_run]->GetEfficiencyErrorUp(ibin);
            }
            if (eEff_mc[n_run]){
                bincontent_mc =  eEff_mc[n_run]->GetEfficiency(ibin);
                binerror_mc =  eEff_mc[n_run]->GetEfficiencyErrorUp(ibin);
            }

            hEff_data[n_run]->SetBinContent(ibin, bincontent_data);
            hEff_data[n_run]->SetBinError(ibin, binerror_data);
            hEff_mc[n_run]->SetBinContent(ibin, bincontent_mc);
            hEff_mc[n_run]->SetBinError(ibin, binerror_mc);
        }

        hEff_data[n_run]->SetMarkerStyle(kFullCircle);
        hEff_mc[n_run]->SetMarkerStyle(kOpenCircle);
        hEff_data[n_run]->SetMarkerSize(1.2);
        hEff_mc[n_run]->SetMarkerSize(1.2);
        hEff_data[n_run]->SetMarkerColor(colors[n_run]);
        hEff_mc[n_run]->SetMarkerColor(colors[n_run]);
        hEff_data[n_run]->SetLineColor(colors[n_run]);
        hEff_mc[n_run]->SetLineColor(colors[n_run]);

        htrending_mc->GetXaxis()->SetBinLabel(n_run + 1, Form("%s",run.at(n_run).Data()));
        htrending_mc->SetBinContent(n_run + 1, hEff_mc[n_run]->GetBinContent(hEff_mc[n_run]->GetXaxis()->FindBin(2.5)));
        htrending_data->GetXaxis()->SetBinLabel(n_run + 1, Form("%s",run.at(n_run).Data()));
        htrending_data->SetBinContent(n_run + 1, hEff_data[n_run]->GetBinContent(hEff_data[n_run]->GetXaxis()->FindBin(2.5)));

        hratiodatamc[n_run] = (TH1F *)hEff_data[n_run]->Clone(Form("hratiodatamc_%s", run[n_run].Data()));
        DivideUncorr(hratiodatamc[n_run], hEff_mc[n_run]);
    }

    TCanvas *c2[nruns];

    TLatex *label = new TLatex();
    label->SetTextFont(42);
    label->SetNDC();
    label->SetTextSize(0.045);
    label->SetTextAlign(22);
    label->SetTextAngle(0);

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    c1->SetTicks(1, 1);
    c1->SetLeftMargin(0.15);
    c1->SetBottomMargin(0.15);
    c1->SetRightMargin(0.05);
    c1->SetTopMargin(0.05);

    TLegend *leg = new TLegend(0.2, 0.18, 0.88, 0.43);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.04);
    leg->SetNColumns(3);

    TH1D *h = new TH1D("h", ";p_{T} (GeV/c);Data/MC;", 10, 0.2, 10.);
    h->SetStats(0);
    h->GetYaxis()->SetRangeUser(0., 2.);
    h->Draw();
    for (int i = 0; i < nruns; i++) {
        hratiodatamc[i]->Draw("EP SAME");
        leg->AddEntry(hratiodatamc[i], Form("%s", run[i].Data()), "lep");
    }
    leg->Draw("SAME");
    label->DrawLatex(0.3, 0.85, Form("Data: %s %s",year_data.Data(),pass_data.Data()));
    label->DrawLatex(0.3, 0.8, Form("MC: %s",period_mc.Data()));

    c1->SaveAs(Form("eos/Summary_Data%s%s_MC%s_part%i.pdf(", year_data.Data(), pass_data.Data(), period_mc.Data(),part));

    TCanvas *c3 = new TCanvas("c3", "c3", 1200, 600);
    c3->SetTicks(1, 1);
    c3->SetLeftMargin(0.15);
    c3->SetBottomMargin(0.15);
    c3->SetRightMargin(0.05);
    c3->SetTopMargin(0.05);

    htrending_data->SetStats(0);
    htrending_data->SetLineColor(kBlack);
    htrending_data->SetMarkerColor(kBlack);
    htrending_mc->SetLineColor(kRed);
    htrending_mc->SetMarkerColor(kRed);
    htrending_data->SetMarkerStyle(kFullCircle);
    htrending_mc->SetMarkerStyle(kFullCircle);
    htrending_data->GetYaxis()->SetTitle("TOF-tracks matching efficiency (p_{T} = 2.5 GeV/c)");
    htrending_data->GetYaxis()->SetRangeUser(0.,1.1);
    htrending_data->Draw();
    htrending_mc->Draw("SAME");

    TLegend *leg3 = new TLegend(0.85, 0.75, 0.4, 0.89);
    leg3->SetBorderSize(0);
    leg3->SetFillStyle(0);
    leg3->SetTextFont(42);
    leg3->SetTextSize(0.04);
    leg3->AddEntry(htrending_data,Form("Data: %s %s",year_data.Data(),pass_data.Data()),"LEP");
    leg3->AddEntry(htrending_mc,Form("MC: %s",period_mc.Data()),"LEP");
    leg3->Draw("SAME");

    c3->SaveAs(Form("eos/Summary_Data%s%s_MC%s_part%i.pdf", year_data.Data(), pass_data.Data(), period_mc.Data(),part));

    TCanvas *c4 = new TCanvas("c4", "c4", 1200, 600);
    c4->SetTicks(1, 1);
    c4->SetLeftMargin(0.15);
    c4->SetBottomMargin(0.15);
    c4->SetRightMargin(0.05);
    c4->SetTopMargin(0.05);

    TH1F* hratiotrending = (TH1F*)htrending_data->Clone("hratiotrending");
    hratiotrending->Divide(htrending_mc);
    hratiotrending->SetStats(0);
    hratiotrending->GetYaxis()->SetTitle("Ratio Data/MC matching efficiency (p_{T} = 2.5 GeV/c)");
    hratiotrending->GetYaxis()->SetRangeUser(0.8,1.2);
    hratiotrending->Draw();
    label->DrawLatex(0.3, 0.85, Form("Data: %s %s",year_data.Data(),pass_data.Data()));
    label->DrawLatex(0.3, 0.8, Form("MC: %s",period_mc.Data()));

    c4->SaveAs(Form("eos/Summary_Data%s%s_MC%s_part%i.pdf", year_data.Data(), pass_data.Data(), period_mc.Data(),part));



    TLatex* tex = new TLatex();
    tex->SetNDC();
    tex->SetTextFont(42);
    tex->SetTextSize(0.04);
    tex->SetTextAlign(11);

    TLegend *leg2 = new TLegend(0.75, 0.75, 0.9, 0.9);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetTextFont(42);
    leg2->SetTextSize(0.04);

    leg2->AddEntry(hEff_data[0], "Data", "lep");
    leg2->AddEntry(hEff_mc[0], "MC", "lep");

    TH1D *h2 = new TH1D("h2", ";p_{T} (GeV/c);Matching efficiency (ITS-TPC + ITS-TPC-TRD);", 10, 0.2, 10.);
    h2->SetStats(0);
    h2->GetYaxis()->SetRangeUser(0., 1.1);

    for (int i = 0; i < nruns; i++)
    {
        c2[i] = new TCanvas(Form("c2_%s", run[i].Data()), Form("c2_%s", run[i].Data()), 800, 600);
        c2[i]->SetTicks(1, 1);
        c2[i]->SetLeftMargin(0.15);
        c2[i]->SetBottomMargin(0.15);
        c2[i]->SetRightMargin(0.05);
        c2[i]->SetTopMargin(0.05);
        c2[i]->cd();

        h2->Draw();
        hEff_data[i]->Draw("EP SAME");
        hEff_mc[i]->Draw("EP SAME");
        tex->DrawLatex(0.2, 0.85, Form("%s", run[i].Data()));
        leg2->Draw("SAME");
        c2[i]->SaveAs(Form("eos/Summary_Data%s%s_MC%s_part%i.pdf", year_data.Data(), pass_data.Data(), period_mc.Data(),part));
    }

    TCanvas* empty = new TCanvas("empty", "empty", 800, 600);
    empty->SaveAs(Form("eos/Summary_Data%s%s_MC%s_part%i.pdf)", year_data.Data(), pass_data.Data(), period_mc.Data(),part));
}

TH1F* DivideUncorr(TH1F* h1, TH1F* h2){
    for(int ibin = 1; ibin <= h1->GetNbinsX(); ibin++){
        double bin1 = h1->GetBinContent(ibin);
        double bin2 = h2->GetBinContent(ibin);
        double bin1err = h1->GetBinError(ibin);
        double bin2err = h2->GetBinError(ibin);
        double binratio = 0;
        if(bin2!=0)
            binratio = bin1 / bin2;

        double binratioerr = binratio * TMath::Sqrt(TMath::Power(bin1err/bin1,2) + TMath::Power(bin2err/bin2,2));
        h1->SetBinContent(ibin, binratio);
        h1->SetBinError(ibin, binratioerr);
    }
    return h1;
}
