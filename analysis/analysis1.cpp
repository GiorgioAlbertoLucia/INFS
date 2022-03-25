#include <TSystem.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TH1.h>
#include <TStyle.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> 
#include <chrono>

using namespace std;

void analysis1()
{   
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);

    ifstream file("../data/picco_calib.txt");

    vector<float> chn_centroid, FWHM, ref_centroid, err_centroid;
    float entry1, entry2, entry3, entry4;
    string line;
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4)
    {
        chn_centroid.push_back(entry1);
        FWHM.push_back(entry2);
        ref_centroid.push_back(entry3);
        err_centroid.push_back(entry4);
    }

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < chn_centroid.size(); i++)   
        cout << chn_centroid.at(i) << "\t\t" << FWHM.at(i) << "\t\t" << err_centroid.at(i) << "\t\t\t" << ref_centroid.at(i) << endl;
    cout << endl;

    TCanvas * canvas1 = new TCanvas("centroide [CHN]", "E [MeV]", 500, 5, 500, 600);
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]+[1]*x+[2]*x*x", 0, 15);
    tf1->SetParameter(0, -0.02);
    tf1->SetParameter(1, 0.0017);

    TGraphErrors * graph1 = new TGraphErrors(chn_centroid.size(), &chn_centroid[0], &ref_centroid[0], &err_centroid[0]);
    graph1->GetXaxis()->SetTitle("centroide [CHN]");
    graph1->GetYaxis()->SetTitle("E [MeV]");
    
    graph1->Fit(tf1);
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/calibrazione_quadr.png");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;
    
}