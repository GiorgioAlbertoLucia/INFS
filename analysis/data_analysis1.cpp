#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

void data_analysis1()
{   

    int nmisure = 10;

    TString name = "catena_elettronica";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path);

    TNtuple * tntuple;
    gDirectory->GetObject("catena_elettronica", tntuple);
    if (tntuple)    tntuple->Print();
    else cerr << "Could not find the TNtuple in: " << gDirectory->GetName() << endl;
    
    TCanvas * canvas1 = new TCanvas("Vin", "Vout", 500, 5, 500, 600);
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*x+[1]", 0, 15);

    Long64_t N1 = tntuple->Draw("Vin:Vout:err_Vin:err_Vout", "", "goff");
    TGraphErrors * graph1 = new TGraphErrors(N1, tntuple->GetV1(), tntuple->GetV2(), tntuple->GetV3(), tntuple->GetV4());
    graph1->GetXaxis()->SetTitle("Vin [V]");
    graph1->GetYaxis()->SetTitle("Vout [V]");
    
    graph1->Fit(tf1);
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/linearità1.jpg");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf2", "[0]*x+[1]", 0, 15);

    Long64_t N2 = tntuple->Draw("Vin:CHN:err_Vin:err_CHN", "", "goff");
    TGraphErrors * graph2 = new TGraphErrors(N2, tntuple->GetV1(), tntuple->GetV2(), tntuple->GetV3(), tntuple->GetV4());
    graph2->GetXaxis()->SetTitle("Vin [V]");
    graph2->GetYaxis()->SetTitle("CHN");
    
    graph2->Fit(tf2);
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/linearità2.jpg");
    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() 
    << " (Probability: " << tf2->GetProb() << ")." << endl;

}