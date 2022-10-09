void histo_hestetics(TH1* h);
void canvas_hestetics_single(TCanvas* c);
void canvas_hestetics_layout(TCanvas* c, int n = 3, int n1 = 0);

void draw(){
    
    int jira = 3251;
    TString period = "LHC22o"
    TString run[] = {"526510"}; //here you can add more runs!
    TString name[] = {"mEffEta_ITSTPC-ITSTPCTRD", "mEffEta_TPC", "mEffPt_ITSTPC-ITSTPCTRD", "mEffPt_TPC","mTOFChi2ITSTPC-ITSTPCTRD", "mTOFChi2TPC", "mDeltaXEtaITSTPC-ITSTPCTRD","mDeltaXEtaTPC","mDeltaZEtaITSTPC-ITSTPCTRD","mDeltaZEtaTPC","HitMap","HadronMasses","BetavsP","DeltatPi"};      

    const int n = sizeof(name)/sizeof(TString); 
    const int nruns = sizeof(run)/sizeof(TString); 
    //
    TFile* f[n][nruns];    
    TH2F* h[n][nruns];

    for (int j =0; j<nruns;j++){
        for (int i = 0; i < n; i++){
            f[i][j] = TFile::Open(Form("Jira_O2_%i/Run%s/rootfiles/%s.root",jira,run[j].Data(),name[i].Data()));
            h[i][j] = (TH2F*)f[i][j]->Get("ccdb_object");
            if (j<=8){
                h[i][j]->SetLineColor(j+1);
                h[i][j]->SetMarkerColor(j+1);
                h[i][j]->SetMarkerStyle(kFullCircle);
            }
            if (j>8){
                h[i][j]->SetLineColor(j-7+1);
                h[i][j]->SetMarkerColor(j-7+1);
                h[i][j]->SetMarkerStyle(kOpenCircle);
            }
        }
    }

    TLatex *label = new TLatex();
	label->SetTextFont(42);
    label-> SetNDC();
    label-> SetTextSize(0.05);
    label-> SetTextAlign(22);
    label-> SetTextAngle(0);

    TH1D* pt = new TH1D("pt",";p_{T} (GeV/c);Matching efficiency;",10,0.,20.);
    TH1D* eta = new TH1D("eta",";#eta;Matching efficiency;",10,-1,+1);
    TH1D* chi = new TH1D("chi",";#chi^{2} (cm);Counts (normalized);",10,0,+10);
    TH1D* eta1 = new TH1D("eta1",";#eta;#Delta x;",10,-1,+1);
    TH1D* eta2 = new TH1D("eta2",";#eta;#Delta z;",10,-1,+1);
    

    pt->SetStats(0);
    eta->SetStats(0);
    eta1->SetStats(0);
    eta2->SetStats(0);
    chi->SetStats(0);
    pt->GetYaxis()->SetRangeUser(0.,1.1);
    eta->GetYaxis()->SetRangeUser(0.,1.1);
    eta1->GetYaxis()->SetRangeUser(-10.,10.);
    eta2->GetYaxis()->SetRangeUser(-10.,10.);
    chi->GetYaxis()->SetRangeUser(0.,0.1);

    //---------------------------------
    //---------SINGLE PLOTS------------
    //---------------------------------
    TCanvas* csingle[n];
    TCanvas* cpdf[n];
    
    TLegend* leg = new TLegend(0.15,0.55,0.45,0.85);
    leg->SetTextSize(0.04);
    leg->SetNColumns(2);
    leg->SetBorderSize(0); 
    for (int j =0; j<nruns;j++){
        leg->AddEntry(h[0][j],Form("%s",run[j].Data()),"EP");
    }

    TLatex *xlabel = new TLatex();
	//xlabel->SetTextFont(42);
    xlabel-> SetNDC();
    xlabel-> SetTextColor(kBlack);
    xlabel-> SetTextSize(0.04);
    xlabel-> SetTextAlign(22);
    xlabel-> SetTextAngle(0);

    TLatex *labelmc = new TLatex();
	//labelmc->SetTextFont(42);
    labelmc-> SetNDC();
    labelmc-> SetTextColor(kRed);
    labelmc-> SetTextSize(0.04);
    labelmc-> SetTextAlign(22);
    labelmc-> SetTextAngle(0);

    cpdf[0] = new TCanvas(Form("cpdf%i",0),Form("cpdf%i",0),1800,800);
    cpdf[0]->Divide(2,0);
    
    for (int i = 0; i < 2; i++){
        //
        csingle[i] = new TCanvas(Form("c_%s",name[i].Data()),"",1000,800);
        eta->Draw();
        for (int j =0; j<nruns;j++){
            h[i][j]->Draw("EP SAME");
        }
        if (i % 2){
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else {
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD");    
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        }
        csingle[i]->SaveAs(Form("Jira_O2_%i/%s.png",jira,name[i].Data()));
        //
        if (i % 2){ 
            cpdf[0]->cd(2);
            eta->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->Draw("EP SAME");
            }
             xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
             labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        } else{
            cpdf[0]->cd(1);
            eta->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD"); 
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        }       
    }
    cpdf[0]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf(",jira,period.Data(),jira),"pdf");
    
    
    
    for (int i = 2; i < 4; i++){
        csingle[i] = new TCanvas(Form("c_%s",name[i].Data()),"",1000,800);
        pt->Draw();
        for (int j =0; j<nruns;j++){
            h[i][j]->Draw("EP SAME");
        }
        leg->Draw("SAME");
        if (i % 2){
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else {
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD");    
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        }
        csingle[i]->SaveAs(Form("Jira_O2_%i/%s.png",jira,name[i].Data())); 
        if (i % 2){ 
            cpdf[0]->cd(2);
            pt->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        } else{
            cpdf[0]->cd(1);
            pt->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD"); 
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        }  
    }
    cpdf[0]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");
    
    
    for (int i = 4; i < 6; i++){
        csingle[i] = new TCanvas(Form("c_%s",name[i].Data()),"",1000,800);
        chi->Draw();
        for (int j =0; j<nruns;j++){
            h[i][j]->DrawNormalized("EP SAME");
        }
        leg->Draw("SAME");
        if (i % 2){
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else {
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD");    
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        }
        csingle[i]->SaveAs(Form("Jira_O2_%i/%s.png",jira,name[i].Data())); 
        if (i % 2){ 
            cpdf[0]->cd(2);
            chi->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->DrawNormalized("EP SAME");
            }
            leg->Draw("SAME");
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else{
            cpdf[0]->cd(1);
            chi->Draw();
            for (int j =0; j<nruns;j++){
                h[i][j]->DrawNormalized("EP SAME");
            }
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD"); 
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        }  
    }
    cpdf[0]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");
    



    TProfile* p[2][nruns];
    for (int i = 6; i < 8; i++){
        csingle[i] = new TCanvas(Form("c_%s",name[i].Data()),"",1000,800);
        eta1->Draw();
        for (int j=0; j<nruns;j++){
            p[i-6][j] = h[i][j]->ProfileX(Form("p1%i%i",i,j));
            p[i-6][j]->Draw("EP SAME");
            if (i<8){
                p[i-6][j]->SetLineColor(j+1);
                p[i-6][j]->SetMarkerColor(j+1);
                p[i-6][j]->SetMarkerStyle(kFullCircle);
            }
            if (j>8){
                p[i-6][j]->SetLineColor(j-7+1);
                p[i-6][j]->SetMarkerColor(j-7+1);
                p[i-6][j]->SetMarkerStyle(kOpenCircle);
            }
        }
        leg->Draw("SAME");
        if (i % 2){
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else {
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD");    
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        }
        csingle[i]->SaveAs(Form("Jira_O2_%i/%s.png",jira,name[i].Data()));
                if (i % 2){ 
            cpdf[0]->cd(2);
            eta1->Draw();
            for (int j =0; j<nruns;j++){
                p[i-6][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        } else{
            cpdf[0]->cd(1);
            eta1->Draw();
            for (int j =0; j<nruns;j++){
                p[i-6][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD"); 
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        }
    }
    cpdf[0]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");
    
        



    TProfile* pz[2][nruns];
    for (int i = 8; i < 10; i++){
        csingle[i] = new TCanvas(Form("c_%s",name[i].Data()),"",1000,800);
        eta2->Draw();
        for (int j=0; j<nruns;j++){
            pz[i-6][j] = h[i][j]->ProfileX(Form("pz%i%i",i,j));
            pz[i-6][j]->Draw("EP SAME");
            if (i<8){
                pz[i-6][j]->SetLineColor(j+1);
                pz[i-6][j]->SetMarkerColor(j+1);
                pz[i-6][j]->SetMarkerStyle(kFullCircle);
            }
            if (j>8){
                pz[i-6][j]->SetLineColor(j-7+1);
                pz[i-6][j]->SetMarkerColor(j-7+1);
                pz[i-6][j]->SetMarkerStyle(kOpenCircle);
            }
        }
        leg->Draw("SAME");
        if (i % 2){
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        } else {
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD");    
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
        }
        csingle[i]->SaveAs(Form("Jira_O2_%i/%s.png",jira,name[i].Data()));
        if (i % 2){ 
            cpdf[0]->cd(2);
            eta2->Draw();
            for (int j =0; j<nruns;j++){
                pz[i-6][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.75, 0.8,"TOF-TPC");
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        } else{
            cpdf[0]->cd(1);
            eta2->Draw();
            for (int j =0; j<nruns;j++){
                pz[i-6][j]->Draw("EP SAME");
            }
            xlabel-> DrawLatex(0.65, 0.8,"TOF-ITSTPC/ITSTPCTRD"); 
            labelmc-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
            leg->Draw("SAME");
        }
    }
    cpdf[0]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");

    cpdf[1] = new TCanvas(Form("cpdf%i",0),Form("cpdf%i",0),1800,800);
    cpdf[1]->Divide(int(nruns)/3,3);

    TLatex *xlabel2 = new TLatex();
	//xlabel2->SetTextFont(42);
    xlabel2-> SetNDC();
    xlabel2-> SetTextColor(kBlack);
    xlabel2-> SetTextSize(0.08);
    xlabel2-> SetTextAlign(22);
    xlabel2-> SetTextAngle(0);

    TLatex *labelmc2 = new TLatex();
	//labelmc2->SetTextFont(42);
    labelmc2-> SetNDC();
    labelmc2-> SetTextColor(kRed);
    labelmc2-> SetTextSize(0.08);
    labelmc2-> SetTextAlign(22);
    labelmc2-> SetTextAngle(0);

    TLatex *phos = new TLatex();
	//phos->SetTextFont(42);
    //phos-> SetNDC();
    phos-> SetTextColor(kBlue);
    phos-> SetTextSize(0.1);
    phos-> SetTextAlign(22);
    phos-> SetTextAngle(0);
    
    for (int j=0; j<nruns;j++){
        cpdf[1]->cd(j+1);
        cpdf[1]->cd(j+1)->SetLogz();
        h[10][j]->Draw("colz ");
        h[10][j]->SetStats(0);;
        xlabel2-> DrawLatex(0.75, 0.8,Form("Run %s",run[j].Data()));
        labelmc2-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));  
        phos-> DrawLatex(14.4, 45,"PHOS");
    }
    cpdf[1]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");
           
    
    
    for (int j=0; j<nruns;j++){
        cpdf[1]->cd(j+1);
        //cpdf[1]->cd(j+1)->SetLogy();
        h[11][j]->Draw("HIST");
        h[11][j]->SetStats(0);
        xlabel2-> DrawLatex(0.75, 0.8,Form("Run %s",run[j].Data()));
        labelmc2-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
    }
    cpdf[1]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");

    for (int j=0; j<nruns;j++){
        cpdf[1]->cd(j+1);
        cpdf[1]->cd(j+1)->SetLogz();
        h[12][j]->Draw("colz ");
        h[12][j]->SetStats(0);;
        xlabel2-> DrawLatex(0.75, 0.8,Form("Run %s",run[j].Data()));
        labelmc2-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));  
    }
    cpdf[1]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf",jira,period.Data() ,jira),"pdf");
 
    //cpdf[1]->Print(Form("Jira_O2_%i/%s_BetaVsPTOF_O2-%i.png",jira,period.Data() ,jira),"png");

    for (int j=0; j<nruns;j++){
        cpdf[1]->cd(j+1);
        //cpdf[1]->cd(j+1)->SetLogy();
        h[13][j]->Draw("HIST");
        h[13][j]->SetStats(0);
        xlabel2-> DrawLatex(0.75, 0.8,Form("Run %s",run[j].Data()));
        labelmc2-> DrawLatex(0.65, 0.7,Form("%s",period.Data()));
    }
    cpdf[1]->Print(Form("Jira_O2_%i/%s_TOF_O2-%i.pdf)",jira,period.Data() ,jira),"pdf");
 

}

void histo_hestetics(TH1* h){
    //h->SetStats(0);
    //h->GetYaxis()->SetTitleSize(0.05);
    //h->GetXaxis()->SetTitleSize(0.05);
    
}

void canvas_hestetics_layout(TCanvas* c, int n = 3, int n1 = 0) {
    c->SetLeftMargin(0.02);
    c->SetBottomMargin(0.02);
    c->SetRightMargin(0.02);
    c->SetTopMargin(0.02);
    c->SetTicky();
    c->SetTickx();
    c->Divide(n,n1);
    for (int i = 1; i <= n ; i++){
        c->cd(i)->SetBottomMargin(0.1);
        c->cd(i)->SetLeftMargin(0.1);
        c->cd(i)->SetRightMargin(0.13);
    }
}

void canvas_hestetics_single(TCanvas* c) {
    c->SetTicky();
    c->SetTickx(); 
}