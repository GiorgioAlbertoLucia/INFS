#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TGraphErrors.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

float conversionCHNtoE(const float chn, const float a, const float b, const float c = 0.)
{   
    float E = a + b * chn + c * chn * chn;
    return E;
}

void data_analysis2()
{
    TString name = "spectrumCo60";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path);

    TNtuple * tntuple;
    gDirectory->GetObject("spectrumCo60", tntuple);
    if (tntuple)    tntuple->Print();
    else cerr << "Could not find the TNtuple in: " << gDirectory->GetName() << endl;
    
    tntuple->Draw("centroide>>hist");
    TH1F * hist = (TH1F*)gDirectory->Get("hist");

    float mean_centr1 = hist->GetMean();
    float std_dev1 = hist->GetStdDev();

    cout << mean_centr1 << endl;
    cout << std_dev1 << endl;


    TString name1 = "picco";
    TString file_path1 = "../doc/" + name1 + ".root";
    TFile tfile1(file_path1);

    gDirectory->GetObject("picco", tntuple);
    if (tntuple)    tntuple->Print();
    else cerr << "Could not find the TNtuple in: " << gDirectory->GetName() << endl;

    TCanvas * canvas1 = new TCanvas("Vin", "Vout", 500, 5, 500, 600);
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*x+[1]", 0, 15);
    tf1->SetParameter(0, -0.02);
    tf1->SetParameter(1, 0.0017);

    Long64_t N1 = tntuple->Draw("centroide:valore_noto:dev_std", "", "goff");
    TGraphErrors * graph1 = new TGraphErrors(N1, tntuple->GetV1(), tntuple->GetV2(), tntuple->GetV3());
    graph1->GetXaxis()->SetTitle("centroide [CHN]");
    graph1->GetYaxis()->SetTitle("E [MeV]");
    
    
    graph1->Fit(tf1);
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/calibrazione.png");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;
    
    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf1", "[0]+[1]*x+[2]*x*x", 0, 15);
    tf2->SetParameter(0, -0.02);
    tf2->SetParameter(1, 0.0017);

    TGraphErrors * graph2 = new TGraphErrors(N1, tntuple->GetV1(), tntuple->GetV2(), tntuple->GetV3());
    graph2->GetXaxis()->SetTitle("centroide [CHN]");
    graph2->GetYaxis()->SetTitle("E [MeV]");
    
    graph2->Fit(tf2);
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/calibrazione_quadr.png");
    
    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << 
    " (Probability: " << tf2->GetProb() << ")." << endl;

}