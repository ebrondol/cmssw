///////////
// Macro that provide to plot comparison between clusters and VH
//////////

#include <iomanip>

std::string folderName = "/afs/cern.ch/work/e/ebrondol/MatchingHitsInHLLHC/CMSSW_6_2_0_SLHC25_patch1/src/RecoLocalTracker/SiPixelStubBuilder/test/";
std::string FileName1 = "cluster_validation_graph.root";
//std::string FileName2 = "vh_validation.root";
std::string FileName2 = "vhs_global.root";


std::string baseFolderRootName = "analysis/Common/";
std::string FolderRootName1 = baseFolderRootName ;
//std::string FolderRootName2 = baseFolderRootName ;
std::string FolderRootName2 = "" ;

std::string extension = "pdf";
std::string outputFileName = "VHcomparison.";


void VHcomparison()
{
  TCanvas* dummy = new TCanvas("dummy","",0,0,700,600);
  dummy -> Print((outputFileName+extension+"[").c_str(),extension.c_str());

  //PlotComparisonMaker1D();
  //PlotRatios();
  PlotComparisonMaker2DandProfile();
  //PlotPulls();

  dummy -> Print((outputFileName+extension+"]").c_str(),extension.c_str());

  return;
}

void PlotComparisonMaker1D()
{
  PlotComparisonMaker1D("effic");
  PlotComparisonMaker1D("fakerate");
  PlotComparisonMaker1D("tracks", true);
  PlotComparisonMaker1D("hits", true);
  PlotComparisonMaker1D("fakes", true);
  PlotComparisonMaker1D("chi2_prob", true);

  //residuals plots
  PlotComparisonMaker1D("dxyres_vs_eta_Mean");
  PlotComparisonMaker1D("dxyres_vs_eta_Sigma");
  PlotComparisonMaker1D("dzres_vs_eta_Mean");
  PlotComparisonMaker1D("dzres_vs_eta_Sigma");
  PlotComparisonMaker1D("phires_vs_eta_Mean");
  PlotComparisonMaker1D("phires_vs_eta_Sigma");
  PlotComparisonMaker1D("ptres_vs_eta_Mean");
  PlotComparisonMaker1D("ptres_vs_eta_Sigma");

  return;
}

void PlotComparisonMaker2DandProfile()
{
  //PlotComparisonClustersPixelStrip("RVsZ", true);
  //PlotComparisonClustersPixelStrip("XVsY", false);
  PlotComparisonClustersPixelStripVHArray("XVsY", true);

  //PlotComparisonMakerProfile("nhits_vs_eta_pfx", false, false);
  //PlotComparisonMakerProfile("nPXBhits_vs_eta_pfx", false, false);

  //Residuals plots
  //PlotComparisonMaker2D("dxyres_vs_eta", true);
  //PlotComparisonMaker2D("dzres_vs_eta", true);
  //PlotComparisonMaker2D("phires_vs_eta", true);
  //PlotComparisonMaker2D("ptres_vs_eta", true);
  return;
}

void PlotPulls()
{
  FitPull("pullDxy");
  FitPull("pullDz");
  FitPull("pullPhi");
  FitPull("pullPt");
  FitPull("pullQoverp");

  return;
}

void PlotRatios(){
  PlotRatios("dxyres_vs_eta_Sigma");
  PlotRatios("dzres_vs_eta_Sigma");
  PlotRatios("phires_vs_eta_Sigma");
  PlotRatios("ptres_vs_eta_Sigma");
}

void PlotComparisonMaker1D(const char* HistoName, bool HistoDraw = false, bool debug = false )
{
  if(debug)	std::cout << "PlotComparisonMaker1D: plot of " << HistoName << " variable..." << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)	std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)	std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFile1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFile2: " << FolderRootName2 << std::endl;

  TH1D* histo1 = new TH1D();
  TH1D* histo2 = new TH1D();
  histo1 = (TH1D*)( f1->Get((FolderRootName1+HistoName).c_str()) );
  histo2 = (TH1D*)( f2->Get((FolderRootName2+HistoName).c_str()) );

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();
  //c1 -> SetLogx();

  histo1 -> SetMarkerColor(2);
  histo1 -> SetLineColor(2);
  histo1 -> SetLineWidth(2);
  histo2 -> SetMarkerColor(4);
  histo2 -> SetLineColor(4);
  histo1 -> GetYaxis() -> SetTitle("#sigma_{#phi residuals}");
  if(HistoDraw) histo1 -> GetYaxis() -> SetTitle("Number of Events");
  histo1 -> GetXaxis() -> SetTitle("#eta");
  if(HistoDraw) histo1 -> GetXaxis() -> SetTitle("#chi^{2} prob");
  histo1 -> GetYaxis() -> SetTitleOffset(1.5);
  histo1 -> GetXaxis() -> SetTitleOffset(1.5);
  histo1 -> GetYaxis() -> SetTitleSize(0.03);
  histo1 -> GetXaxis() -> SetTitleSize(0.03);
  histo1 -> GetYaxis() -> SetLabelSize(0.03);
  histo1 -> GetXaxis() -> SetLabelSize(0.03);
  //histo1 -> GetXaxis() -> SetRangeUser(-5,5);

  TLegend* legend = new TLegend(0.16, 0.77, 0.30, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(histo1,(MultivalLabelTracks1).c_str(),"PL");
  legend -> AddEntry(histo2,(MultivalLabelTracks2).c_str(),"PL");

  if(HistoDraw){
    histo1 -> Draw("Histo");
    histo2 -> Draw("Histosame");
  } else {
    histo1 -> Draw("e1");
    histo2 -> Draw("e1same");
  }
  legend -> Draw("same");

  if(debug)     std::cout << "histo1*: " << histo1 << ", histo2*: " << histo2 << std::endl;
//  c1 -> Print(("Plots/"+string(HistoName)+"_"+Associator+".pdf").c_str(), "pdf");
//  c1 -> Print(("Plots/"+string(HistoName)+"_"+Associator+".png").c_str(), "png");
//  c1 -> SaveAs((string(HistoName)+"Comparison.C").c_str());
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}

void PlotComparisonMaker2D(const char* HistoName,  bool proj = false, bool debug = false ){
  if(debug)     std::cout << "Plotting " << HistoName << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFolder1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFolder2: " << FolderRootName2 << std::endl;

  if(!proj){
    TH2D* histo1 = new TH2D();
    TH2D* histo2 = new TH2D();
    histo1 = (TH2D*)( f1->Get((FolderRootName1+HistoName).c_str()) );
    histo2 = (TH2D*)( f2->Get((FolderRootName2+HistoName).c_str()) );
  }
  else{
    TH2F* histo2D1 = (TH2F*)( f1->Get((FolderRootName1+HistoName).c_str()) );
    TH2F* histo2D2 = (TH2F*)( f2->Get((FolderRootName2+HistoName).c_str()) );
    TH1D* histo1 = histo2D1->ProjectionY("histo1");
    TH1D* histo2 = histo2D2->ProjectionY("histo2");
  }

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();
  if(proj)  c1 -> SetLogy();
  //c1 -> SetLogx();
  histo1 -> SetMarkerColor(2);
  histo1 -> SetLineColor(2);
  histo1 -> SetLineWidth(2);
  histo2 -> SetMarkerColor(4);
  histo2 -> SetMarkerStyle(22);
  histo2 -> SetLineColor(4);
  histo1 -> GetYaxis() -> SetTitle(HistoName);
  if(proj)  histo1 -> GetYaxis() -> SetTitle("Number of Events");
  histo1 -> GetYaxis() -> SetTitleOffset(2);
  histo1 -> GetXaxis() -> SetTitle("");
  if(proj)  histo1 -> GetXaxis() -> SetTitle(HistoName);
  histo1 -> GetXaxis() -> SetTitleSize(0.03);
  histo1 -> GetYaxis() -> SetTitleSize(0.03);
  histo1 -> GetXaxis() -> SetLabelSize(0.03);
  histo1 -> GetYaxis() -> SetLabelSize(0.03);
  //if(histo2D)  histo1 -> GetXaxis() -> SetRangeUser(-0.01,0.01);

  TLegend* legend = new TLegend(0.16, 0.77, 0.45, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(histo1,"Phase2Clusters","PL");
  legend -> AddEntry(histo2,"VectorHits","PL");

  if(proj){
    histo1 -> DrawNormalized("Histo");
    histo2 -> DrawNormalized("Histosame");
  } else {
    histo1 -> Draw("P");
    histo2 -> Draw("Psame");
  }
  legend -> Draw("same");

  if(debug){
    std::cout << "histo1*: " << histo1 << ", histo2*: " << histo2 << std::endl;
    if(!proj) 	 c1 -> Print((string(HistoName)+".pdf").c_str(), "pdf");
    else	       c1 -> Print((string(HistoName)+"_ProjY.pdf").c_str(), "pdf");
//  c1 -> Print((string(HistoName)+"_"+Associator+".png").c_str(), "png");
//  c1 -> SaveAs((string(HistoName)+"Comparison.C").c_str());
  }
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}
void PlotComparisonClustersPixelStripVHArray(const char* HistoName, bool debug = false ){

  if(debug)     std::cout << "Plotting " << HistoName << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFolder1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFolder2: " << FolderRootName2 << std::endl;

  TGraph* histo1pixel = new TGraph();
  TGraph* histo1strip = new TGraph();

  histo1pixel = (TGraph*)( f1->Get((FolderRootName1+HistoName+"_Pixel").c_str()) );
  histo1strip = (TGraph*)( f1->Get((FolderRootName1+HistoName+"_Strip").c_str()) );
  TCanvas* canvasArray = (TCanvas*) f2->Get("c1") ;

  if(debug)     std::cout << "graph1*: " << histo1pixel << ", graph2*: " << histo1strip << std::endl;

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();
  //c1 -> SetLogy();
  //c1 -> SetLogx();
  histo1pixel -> SetMarkerColor(kBlue+1);
  histo1pixel -> SetLineColor(kBlue+1);
  histo1strip -> SetMarkerColor(kRed+1);
  histo1strip -> SetLineColor(kRed+1);

  histo1strip -> GetYaxis() -> SetTitle(HistoName);
  histo1strip -> GetXaxis() -> SetTitle("");
  //histo1pixel -> GetXaxis() -> SetTitle("Y [cm]");
  //histo1pixel -> GetYaxis() -> SetTitle("X [cm]");

  histo1strip -> GetYaxis() -> SetTitleOffset(2);
  histo1strip -> GetXaxis() -> SetTitleOffset(2);

  histo1strip -> GetXaxis() -> SetTitleSize(0.03);
  histo1strip -> GetYaxis() -> SetTitleSize(0.03);
  histo1strip -> GetXaxis() -> SetLabelSize(0.03);
  histo1strip -> GetYaxis() -> SetLabelSize(0.03);
  //histo1pixel -> GetXaxis() -> SetRangeUser(-300.,300.);

  histo1strip -> Draw("AP");
  histo1pixel -> Draw("Psame");

  TObject *obj;
  TIter next(canvasArray->GetListOfPrimitives());
  while ((obj=next())) {
    cout << "Reading: " << obj->GetName() << endl;
      if (obj->InheritsFrom("TGraph")) {
        TGraph* gr = obj;
        gr -> SetMarkerColor(7);
        //gr -> SetMarkerStyle(22);
        gr -> SetMarkerSize(0.5);
        gr -> SetLineColor(7);
        gr -> Draw("Psame");

        //cout << "graph: " << obj->GetName() << endl;
      }
      if (obj->InheritsFrom("TArrow")){
        //cout << "array: " << obj->GetName() << endl;
        TArrow* arr = obj;
        arr->SetLineColor(7);
        arr->Draw(">same");
      }
 }
 if(debug)     std::cout << "graph*: " << gr << std::endl;

  TLegend* legend = new TLegend(0.16, 0.77, 0.45, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(histo1pixel,"Phase2Clusters Pixels","PL");
  legend -> AddEntry(histo1strip,"Phase2Clusters Strips","PL");
  legend -> AddEntry(gr,"VectorHits","PL");

  legend -> Draw("same");

  if(debug){
    c1 -> Print((string(HistoName)+".pdf").c_str(), "pdf");

//  c1 -> Print((string(HistoName)+"_"+Associator+".png").c_str(), "png");
//  c1 -> SaveAs((string(HistoName)+"Comparison.C").c_str());
  }
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}

void PlotComparisonClustersPixelStrip(const char* HistoName, bool debug = false ){
  if(debug)     std::cout << "Plotting " << HistoName << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFolder1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFolder2: " << FolderRootName2 << std::endl;

  TH2D* histo1pixel = new TH2D();
  TH2D* histo1strip = new TH2D();
  TH2D* histo2pixel = new TH2D();
  TH2D* histo2strip = new TH2D();

  histo1pixel = (TH2D*)( f1->Get((FolderRootName1+HistoName+"_Pixel").c_str()) );
  histo2pixel = (TH2D*)( f2->Get((FolderRootName2+HistoName+"_Pixel").c_str()) );
  histo1strip = (TH2D*)( f1->Get((FolderRootName1+HistoName+"_Strip").c_str()) );
  histo2strip = (TH2D*)( f2->Get((FolderRootName2+HistoName+"_Strip").c_str()) );

  if(debug)     std::cout << "histo1*: " << histo1pixel << ", histo2*: " << histo2pixel << std::endl;

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();
  //c1 -> SetLogy();
  //c1 -> SetLogx();
  histo1pixel -> SetMarkerColor(kBlue+1);
  histo1pixel -> SetLineColor(kBlue+1);
  histo1strip -> SetMarkerColor(kRed+1);
  histo1strip -> SetLineColor(kRed+1);

  histo2pixel -> SetMarkerColor(7);
  histo2pixel -> SetMarkerStyle(22);
  histo2pixel -> SetLineColor(7);
  histo2strip -> SetMarkerColor(93);
  histo2strip -> SetMarkerStyle(22);
  histo2strip -> SetLineColor(93);

  histo1pixel -> GetYaxis() -> SetTitle(HistoName);
  histo1pixel -> GetXaxis() -> SetTitle("");
  //histo1pixel -> GetXaxis() -> SetTitle("Y [cm]");
  //histo1pixel -> GetYaxis() -> SetTitle("X [cm]");

  histo1pixel -> GetYaxis() -> SetTitleOffset(2);
  histo1pixel -> GetXaxis() -> SetTitleOffset(2);

  histo1pixel -> GetXaxis() -> SetTitleSize(0.03);
  histo1pixel -> GetYaxis() -> SetTitleSize(0.03);
  histo1pixel -> GetXaxis() -> SetLabelSize(0.03);
  histo1pixel -> GetYaxis() -> SetLabelSize(0.03);
  //if(histo2D)  histo1 -> GetXaxis() -> SetRangeUser(-0.01,0.01);

  TLegend* legend = new TLegend(0.16, 0.77, 0.45, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(histo1pixel,"Phase2Clusters Pixels","PL");
  legend -> AddEntry(histo1strip,"Phase2Clusters Strips","PL");
  legend -> AddEntry(histo2pixel,"VectorHits Pixels","PL");
  legend -> AddEntry(histo2strip,"VectorHits Strips","PL");

  histo1pixel -> Draw("P");
  histo1strip -> Draw("Psame");
  histo2pixel -> Draw("Psame");
  histo2strip -> Draw("Psame");

  legend -> Draw("same");

  if(debug){
    c1 -> Print((string(HistoName)+".pdf").c_str(), "pdf");

//  c1 -> Print((string(HistoName)+"_"+Associator+".png").c_str(), "png");
//  c1 -> SaveAs((string(HistoName)+"Comparison.C").c_str());
  }
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}

void PlotComparisonMakerProfile(const char* HistoName,  bool proj = false, bool debug = false ){
  if(debug)     std::cout << "Plotting " << HistoName << " variable..." << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFile1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFile2: " << FolderRootName2 << std::endl;

  if(!proj){
    TProfile* histo1 = (TProfile*)( f1->Get((FolderRootName1+HistoName).c_str()) );
    TProfile* histo2 = (TProfile*)( f2->Get((FolderRootName2+HistoName).c_str()) );
  }
  else{
    TProfile* histo2D1 = (TProfile*)( f1->Get((FolderRootName1+HistoName).c_str()) );
    TProfile* histo2D2 = (TProfile*)( f2->Get((FolderRootName2+HistoName).c_str()) );
    TH1D* histo1 = histo2D1->ProjectionY("histo1");
    TH1D* histo2 = histo2D2->ProjectionY("histo2");
  }

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();
  if(proj)  c1 -> SetLogy();
  //c1 -> SetLogx();
  histo1 -> SetMarkerColor(2);
  histo1 -> SetLineColor(2);
  histo1 -> SetLineWidth(2);
  histo2 -> SetMarkerColor(4);
  histo2 -> SetLineColor(4);
  histo1 -> GetYaxis() -> SetTitle(HistoName);
  if(proj)  histo1 -> GetYaxis() -> SetTitle("Number of Events");
  histo1 -> GetYaxis() -> SetTitleOffset(2);//0.01);
  histo1 -> GetXaxis() -> SetTitle("");
  if(proj)  histo1 -> GetXaxis() -> SetTitle(HistoName);//#chi^{2}");
  histo1 -> GetXaxis() -> SetTitleSize(0.03);
  histo1 -> GetYaxis() -> SetTitleSize(0.03);
  histo1 -> GetXaxis() -> SetLabelSize(0.03);
  histo1 -> GetYaxis() -> SetLabelSize(0.03);
  //if(histo2D)  histo1 -> GetXaxis() -> SetRangeUser(-0.01,0.01);

  TLegend* legend = new TLegend(0.16, 0.77, 0.30, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(histo1,(MultivalLabelTracks1+" - "+Associator).c_str(),"PL");
  legend -> AddEntry(histo2,(MultivalLabelTracks2+" - "+Associator).c_str(),"PL");

  if(proj){
    histo1 -> DrawNormalized("Histo");
    histo2 -> DrawNormalized("Histosame");
  } else {
    histo1 -> Draw("e1");
    histo2 -> Draw("e1same");
  }
  legend -> Draw("same");

  if(debug){
    std::cout << "histo1*: " << histo1 << ", histo2*: " << histo2 << std::endl;
    if(!proj)     c1 -> Print((string(HistoName)+"_"+Associator+".pdf").c_str(), "pdf");
    else          c1 -> Print((string(HistoName)+"_ProjY_"+Associator+".pdf").c_str(), "pdf");
  }
//  c1 -> Print((string(HistoName)+"_"+Associator+".png").c_str(), "png");
//  c1 -> SaveAs((string(HistoName)+"Comparison.C").c_str());
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}

void FitPull(const char* HistoName, bool debug = false)
{
  setTDRStyle();
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.13);
  gStyle->SetPadRightMargin(0.17);
  gStyle->SetLabelSize(0.04,"XYZ");
  gStyle->SetTitleSize(0.05,"XYZ");
  gStyle->SetOptFit(111111);

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();

  if(debug)     std::cout << "Plotting " << HistoName << " variable..." << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFile1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFile2: " << FolderRootName2 << std::endl;

  TH1D *p = (TH1D*)( f1->Get((FolderRootName1+HistoName).c_str()) );
  p -> Sumw2();
  p -> Scale(1/p->Integral());
  p -> SetMarkerColor(kRed);
  p -> SetLineColor(kRed);
  p -> SetLineWidth(2);
  p -> SetLineWidth(2);
  p -> GetYaxis() -> SetTitle("Arbitrary Unit");
  p -> GetYaxis() -> SetTitleOffset(2);//0.01);
  p -> GetXaxis() -> SetTitle("pull Q/p");
  p -> GetXaxis() -> SetTitleSize(0.03);
  p -> GetYaxis() -> SetTitleSize(0.03);
  p -> GetXaxis() -> SetLabelSize(0.03);
  p -> GetYaxis() -> SetLabelSize(0.03);
  p -> GetXaxis() -> SetRangeUser(-3.0,3.0);
  p -> Draw();
  TH1D *p1 = (TH1D*)( f2->Get((FolderRootName2+HistoName).c_str()) );
  p1 -> Sumw2();
  p1 -> Scale(1/p1->Integral());
  TF1* f_gaus1 = new TF1("f_gaus1","gaus(0)",-1.5,1.5);
//  f_gaus1 -> SetParameter(0, 0.085);
  f_gaus1 -> SetParameter(1, 0.);
  f_gaus1 -> SetParameter(2, 1.);
  f_gaus1 -> SetParName(0,"Constant");
  f_gaus1 -> SetParName(1,"Mean");
  f_gaus1 -> SetParName(2,"#sigma");
  f_gaus1 -> SetLineColor(kRed+1);
  p->Fit("f_gaus1","R","");
  TF1* f_gaus2 = new TF1("f_gaus2","gaus(0)",-1.5,1.5);
//  f_gaus2 -> SetParameter(0, 0.085);
  f_gaus2 -> SetParameter(1, 0.);
  f_gaus2 -> SetParameter(2, 1.);
  f_gaus2 -> SetParName(0,"Constant");
  f_gaus2 -> SetParName(1,"Mean");
  f_gaus2 -> SetParName(2,"#sigma");
  f_gaus2 -> SetLineColor(kBlue+1);
  p1->Fit("f_gaus2","R","sames");

  p1->SetMarkerColor(kBlue);
  p1->SetLineColor(kBlue);
  p1->Draw("sames");
  c1->Update();

  TLegend* legend = new TLegend(0.16, 0.77, 0.30, 0.92);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.03);

  legend -> AddEntry(p,(MultivalLabelTracks1).c_str(),"PL");
  legend -> AddEntry(p1,(MultivalLabelTracks2).c_str(),"PL");
  legend -> Draw("same");
  c1->Update();

  TPaveStats *stat = (TPaveStats*)(p->FindObject("stats"));
  TPaveStats *stat1 = (TPaveStats*)(p1->FindObject("stats"));
  if(stat && stat1) {
    stat->SetTextColor(kRed);
    stat1->SetTextColor(kBlue);
    float height = stat1->GetY2NDC() - stat1->GetY1NDC();
    stat1->SetY1NDC(stat->GetY1NDC() - height);
    stat1->SetY2NDC(stat->GetY1NDC() );
    stat1->Draw();
  }
  c1 -> Print(("Plots/"+string(HistoName)+"_"+Associator+".pdf").c_str(), "pdf");
  c1 -> Print(("Plots/"+string(HistoName)+"_"+Associator+".png").c_str(), "png");
  return;
}


void PlotRatios(const char* HistoName, bool debug = false)
{
  if(debug)     std::cout << "Plotting ratio of " << HistoName << " variable..." << std::endl;
  std::string inFileTotalName1 = folderName + FileName1;
  std::string inFileTotalName2 = folderName + FileName2;

  if(debug)     std::cout << "InputFile1: " << inFileTotalName1 << std::endl;
  if(debug)     std::cout << "InputFile2: " << inFileTotalName2 << std::endl;

  TFile* f1 = TFile::Open(inFileTotalName1.c_str(), "READ");
  TFile* f2 = TFile::Open(inFileTotalName2.c_str(), "READ");

  if(debug)     std::cout << "InputRootFile1: " << FolderRootName1 << std::endl;
  if(debug)     std::cout << "InputRootFile2: " << FolderRootName2 << std::endl;

  TH1F* histo1 = new TH1F();
  TH1F* histo2 = new TH1F();
  histo1 = (TH1F*)( f1->Get((FolderRootName1+HistoName).c_str()) );
  histo2 = (TH1F*)( f2->Get((FolderRootName2+HistoName).c_str()) );

    //TH1F* ratio = ratioHisto(histo1, histo2);
  histo2->Divide(histo1);
  TH1F* ratio = histo2;

  TCanvas* c1 = new TCanvas();
  c1 -> cd();
  c1 -> SetGridx();
  c1 -> SetGridy();

  ratio -> SetMarkerColor(6);
  ratio -> SetLineWidth(1);
  ratio -> GetYaxis() -> SetTitle("#sigma_{#phi, DAF}/#sigma_{#phi, Refit}");
  ratio -> GetXaxis() -> SetTitle("#eta");
  //ratio -> GetYaxis() -> SetTitle("#sigma_{dxyres, DAF}/#sigma_{dxyres, Refit}");
  //ratio -> GetXaxis() -> SetTitle("#eta");
  ratio -> GetYaxis() -> SetTitleOffset(2);//0.01);
  ratio -> GetXaxis() -> SetTitleSize(0.03);
  ratio -> GetYaxis() -> SetTitleSize(0.03);
  ratio -> GetXaxis() -> SetLabelSize(0.03);
  ratio -> GetYaxis() -> SetLabelSize(0.03);
  double yMin = 0.;
  double yMax = 2.;
  ratio -> GetYaxis() -> SetRangeUser(yMin,yMax);

  ratio -> Draw("e");

  TArrow* line = new TArrow(-2.5,1,2.5,1);
  line -> SetLineWidth(1.5);
  line -> Draw("same");

  if(debug){
    std::cout << "histo1*: " << histo1 << ", histo2*: " << histo2 << std::endl;
  }
  c1 -> Print(("Plots/"+string(HistoName)+"Ratio_"+Associator+".pdf").c_str(), "pdf");
  c1 -> Print(("Plots/"+string(HistoName)+"Ratio_"+Associator+".png").c_str(), "png");
  c1 -> Print((outputFileName+extension).c_str(),extension.c_str());

}


TH1F* ratioHisto(TH1F* h_num, TH1F* h_den)
{
  TH1F* h_ratio = (TH1F*)( h_num->Clone() );

  for(int bin = 1; bin <= h_num->GetNbinsX(); ++bin)
  {
    double int_num = h_num -> Integral(1,bin);
    double int_den = h_den -> Integral(1,bin);
    h_ratio -> SetBinContent(bin,int_num/int_den);
    h_ratio -> SetBinError(bin,0.);
  }

  double yMin = 0.75;
  double yMax = 1.25;
  h_ratio -> GetYaxis() -> SetRangeUser(yMin,yMax);

  return h_ratio;
}
