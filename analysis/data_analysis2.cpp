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

void data_analysis2()
{
    TString name = "spectrumCo60";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path);

    TNtuple * tntuple;
    gDirectory->GetObject("spectrumCo60", tntuple);
    if (tntuple)    tntuple->Print();
    else cerr << "Could not find the TNtuple in: " << gDirectory->GetName() << endl;
    
    tntuple->Draw("centroid>>hist");
    TH1F * hist = (TH1F*)gDirectory->Get("hist");

    float mean_centr = hist->GetMean();
    float std_dev = hist->GetStdDev();

    // usando semidisp. max sugli ultimi due dati
    float std_dev_new = (799.19 - 797.95) / 2;  

    tntuple->Draw("FWHM>>FWHM_hist");
    TH1F * FWHM_hist = (TH1F*)gDirectory->Get("FWHM_hist");

    cout << endl << "std dev FWHM = " << FWHM_hist->GetStdDev() << endl;

    name = "picco_calib";
    file_path = "../doc/" + name + ".root";
    tfile.Close();
    tfile.Open(file_path);

    gDirectory->GetObject(name, tntuple);
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

    float a = tf2->GetParameter(0);
    float b = tf2->GetParameter(1);
    float c = tf2->GetParameter(2);
    float err_a = tf2->GetParError(0);
    float err_b = tf2->GetParError(1);
    float err_c = tf2->GetParError(2);

    float CHN;
    vector<float> E;
    vector<float> err_E;
    tntuple->SetBranchAddress("centroide", &CHN);
    for(Int_t i = 0; i < tntuple->GetEntries(); i++)   
    {
        tntuple->GetEvent(i);
        float Ei = a + b * CHN + c * CHN * CHN;
        E.push_back(Ei);
        float err_Ei = sqrt(err_a*err_a + CHN*err_b*err_b + CHN*CHN*err_c*err_c + (b + c*CHN)*std_dev_new*std_dev_new);
        err_E.push_back(err_Ei);
    }

    cout << "Energies: " << endl;
    for(vector<float>::const_iterator i = E.begin(); i != E.end(); i++) cout << *i << "MeV" << endl;
    cout << "Error on Energies: " << endl;
    for(vector<float>::const_iterator i = err_E.begin(); i != err_E.end(); i++) cout << *i << "MeV" << endl;
}