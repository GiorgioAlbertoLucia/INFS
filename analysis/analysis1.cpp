#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TROOT.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string> 

using namespace std;

void analysis1()
{   
    //////////////// VERIFICA DELLA LINEARITÀ DELLA CATENA ELETTRONICA

    //freopen("../output/analysis1.txt", "w", stdout);
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);

    ifstream file("../data/catena_elettronica.txt");

    vector<float> Vin, Vout, CHN, err_Vin, err_Vout, err_CHN;
    float entry1, entry2, entry3, entry4, entry5, entry6;
    string line;
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6)
    {
        Vin.push_back(entry1);
        Vout.push_back(entry2);
        CHN.push_back(entry3);
        err_Vin.push_back(entry4);
        err_Vout.push_back(entry5);
        err_CHN.push_back(entry6);
    }

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < Vin.size(); i++)   
        cout << Vin.at(i) << "\t" << Vout.at(i) << "\t" << CHN.at(i) << "\t" << err_Vin.at(i) << "\t" << 
        err_Vout.at(i) << "\t\t" << err_CHN.at(i) << endl;
    cout << endl;
    
    TCanvas * canvas1 = new TCanvas("Vin", "Vout", 500, 5, 500, 600);
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*x+[1]", -15, 15);

    TGraphErrors * graph1 = new TGraphErrors(Vin.size(), &Vin[0], &Vout[0], &err_Vin[0], &err_Vout[0]);
    graph1->GetXaxis()->SetTitle("Vin [V]");
    graph1->GetYaxis()->SetTitle("Vout [V]");
    
    graph1->Fit(tf1, "ER");
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/linearità1.jpg");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf2", "[0]*x+[1]", -15, 15);

    TGraphErrors * graph2 = new TGraphErrors(Vin.size(), &Vin[0], &CHN[0], &err_Vin[0], &err_CHN[0]);
    graph2->GetXaxis()->SetTitle("Vin [V]");
    graph2->GetYaxis()->SetTitle("CHN");
    
    graph2->Fit(tf2, "ER");
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/linearità2.jpg");
    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() 
    << " (Probability: " << tf2->GetProb() << ")." << endl;

}