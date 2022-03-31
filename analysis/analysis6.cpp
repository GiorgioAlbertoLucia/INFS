#include <TFile.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TMath.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;


void analysis6()
{   
    //freopen("../output/analysis5.txt", "w", stdout);
    fstream file("../data/picco_sommaCo60.txt", fstream::in);

    vector<string> peak;
    vector<float> gross_area, net_area, live_time, err_live_time;
    string entry1;
    float entry2, entry3, entry4, entry5;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5)
    {
        peak.push_back(entry1);
        gross_area.push_back(entry2);
        net_area.push_back(entry3);
        live_time.push_back(entry4);
        err_live_time.push_back(entry5);
    }
    
    vector<float> err_net_area, Ssum1, err_Ssum1, Ssum2, err_Ssum2;            //rate - rel_err = relative error
    float entry6, entry7, entry9, entry10;
    float dt = 0.001;  // s
    
    for (int i = 0; i < peak.size(); i++)
    {   
        entry6 = sqrt(2*gross_area.at(i) - net_area.at(i));
        err_net_area.push_back(entry6);
        
        entry9 = net_area.at(i) / (live_time.at(i) * 1000); //kBq
        
    }
    /*
    string str_err_net_area("err_net"), str_R("R[kBq]"), str_err_R("err_rate[kBq]"), str_rel_err_R("rel_e_rate"), 
            str_X("X[g/cm^2]"), str_err_X("err_X[g/cm^2]");
    if(line.find(str_err_net_area) == string::npos) append_column("../data/coeff_ass.txt", "err_net", err_net_area);
    if(line.find(str_R) == string::npos)            append_column("../data/coeff_ass.txt", "R[kBq]", R);
    if(line.find(str_err_R) == string::npos)        append_column("../data/coeff_ass.txt", "err_rate[kBq]", err_R);
    if(line.find(str_rel_err_R) == string::npos)    append_column("../data/coeff_ass.txt", "rel_e_rate", rel_err_R);
    if(line.find(str_X) == string::npos)            append_column("../data/coeff_ass.txt", "X[g/cm^2]", X);
    if(line.find(str_err_X) == string::npos)        append_column("../data/coeff_ass.txt", "err_X[g/cm^2]", err_X);
    */

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < n_spessori.size(); i++)   
        cout << n_spessori.at(i) << "\t\t" << x.at(i) << "\t\t" << err_x.at(i) << "\t\t" << gross_area.at(i) << "\t\t"
        << net_area.at(i) << "\t\t" << live_time.at(i) << "\t\t" << err_live_time.at(i) << "\t\t" << err_net_area.at(i) << "\t\t"
        << R.at(i) << "\t\t" << err_R.at(i) << "\t\t" << rel_err_R.at(i) << "\t\t" << X.at(i) << "\t\t" << err_X.at(i) << endl;
    cout << endl;

    file.close();

    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    TCanvas * canvas1 = new TCanvas("canvas1", "R", 500, 500, 500, 600);

    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*exp(-[1]*x)", 0, 15);
    tf1->SetParName(0, "R_{0}");
    tf1->SetParName(1, "#mu");
    tf1->SetParameters(78.6, 0.1015);

    TGraphErrors * graph1 = new TGraphErrors(x.size(), &x[0], &R[0], &err_x[0], &err_R[0]);
    graph1->GetXaxis()->SetTitle("x [cm]");
    graph1->GetYaxis()->SetTitle("R [s^{-1}]");
    
    graph1->Fit(tf1, "m");
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/rate_x.png");

    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    float ref_mi_rho = 0.1035;
    cout << endl <<"Test z su mi: " << endl;
    z_test(tf1->GetParameter("#mu")/rho, ref_mi_rho, tf1->GetParError(1)/rho);
    cout << endl;

    
}