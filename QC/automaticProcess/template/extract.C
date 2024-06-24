void extract(const char* path="MatchTrAll",  const char* objName="mEffPt_ITSTPC-ITSTPCTRD", const char* outDir="."){
  TFile *fd = new TFile("QC_fullrun.root");

  o2::quality_control::core::MonitorObjectCollection *mon = (o2::quality_control::core::MonitorObjectCollection *) fd->Get(Form("int/TOF/%s",path));
  o2::quality_control::core::MonitorObject *obj = (o2::quality_control::core::MonitorObject *) mon->FindObject(objName);
  TObject *hd = (TObject *) obj->getObject();
  TFile *fout = new TFile(Form("%s/%s.root",outDir,objName),"RECREATE");
  hd->Write("ccdb_object");
  fout->Close();
}
