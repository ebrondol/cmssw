
void drawingPlotFromFile(const std::string& datafile)
{
  TH1F *g = new TH1F("g","histo",100,0.5,100.5);
  std::string perc;
  std::string line;
  int i;
  std::ifstream inFile;
  inFile.open (datafile.c_str(), std::ios::in);

  while(!inFile.eof())
  {
    getline(inFile, line);
    istringstream iss(line);
    string sub;
    iss >> sub;
    if (sub == "Begin"){
      std::cin.ignore(256,' ');
    } else {
      inFile >> perc;
      if(atof(perc.c_str())) {
        double value = (double)atof(perc.c_str());
        std::cout<<"Alla riga " <<i<<" ho letto " << value <<std::endl;
        g->Fill(value);
      }
    }
    i++;

  }
  
  inFile.close();

  TCanvas *c = new TCanvas("c_graph","c_graph");
  c->cd();
  c->SetGridy();
  c->SetGridx();
  c->SetTickx();
  c->SetTicky();
  c->SetLogy();

  g->SetTitle("TGraph Title");
  g->GetXaxis()->SetTitle("Same Hits");
  g->GetYaxis()->SetTitle("N");
  g->SetMarkerStyle(11);
  g->SetMarkerColor(6);
//  g->SetMinimum(0);
//  g->SetMaximum(40);
  g->Draw("HISTO");

  c->Print( "SingleMu_Hits.pdf","pdf" );

}
