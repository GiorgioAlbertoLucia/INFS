#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>
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

/**
 * @brief Gaussian test. The function prints the value of the variable Z and the result of the test
 * @param exp_value 
 * @param ref_value 
 * @param err 
 */
void z_test(const float exp_value, const float ref_value, const float err)
{
    float z = (exp_value - ref_value) / err;
    if (abs(z) < 1.96)  cout << exp_value << " +/- " << err << "\t" << ref_value << "\t"  << z << "\t" << "True" << endl;
    else                cout << exp_value << " +/- " << err << "\t" << ref_value << "\t"  << z << "\t" << "False" << endl;
}


void data_analysis5()
{
    TString name = "coeff_ass";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path, "update");

    TTree * tree;
    gDirectory->GetObject(name, tree);
    if (tree)    tree->Print();
    else cerr << "Could not find the tree in: " << gDirectory->GetName() << endl;

    float rho = 11.34;

    float gross_area;
    float net_area;
    float time;
    float x;
    float err_x;
    
    float err_net_area;
    float R;
    float err_R;
    float rel_err_R;
    float X;
    float err_X;

    tree->SetBranchAddress("gross_area", &gross_area);
    tree->SetBranchAddress("net_area", &net_area);
    tree->SetBranchAddress("time", &time);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("err_x", &err_x);

    auto berr_net_area = tree->Branch("err_net_area", &err_net_area);
    auto bR = tree->Branch("R", &R);
    auto berr_R = tree->Branch("err_R", &err_R);
    auto brel_err_R = tree->Branch("rel_err_R", &rel_err_R);
    auto bX = tree->Branch("X", &X);
    auto berr_X = tree->Branch("err_X", &err_X);
    
    for(Long64_t i = 0; i < tree->GetEntries(); i++)   
    {
        tree->GetEvent(i);
        err_net_area = sqrt(net_area + gross_area);
        R = net_area/time;
        err_R = err_net_area / time;
        rel_err_R = err_R / R;
        X = x * rho;
        err_X = err_x * rho;

        berr_net_area->Fill();
        bR->Fill();
        berr_R->Fill();
        brel_err_R->Fill();
        bX->Fill();
        berr_X->Fill();
    }
    tree->Write("", TObject::kOverwrite);

    //cout << endl<< name << ": " << endl;
    tree->Scan("");

    TCanvas * canvas1 = new TCanvas("X", "R", 500, 5, 500, 600);

    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*exp(-[1]*x)", 0, 15);
    tf1->SetParName(0, "R_{0}");
    tf1->SetParName(1, "mi");
    tf1->SetParameters(78.6, 0.1015);

    Long64_t N1 = tree->Draw("x:R:err_x:err_R", "", "goff");
    TGraphErrors * graph1 = new TGraphErrors(N1, tree->GetV1(), tree->GetV2(), tree->GetV3(), tree->GetV4());
    graph1->GetXaxis()->SetTitle("x [cm]");
    graph1->GetYaxis()->SetTitle("R [s^{-1}]");
    
    graph1->Fit(tf1, "m");
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/rate.png");

    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    float ref_mi_rho = 0.1035;
    cout << endl;
    z_test(tf1->GetParameter("mi")/rho, ref_mi_rho, tf1->GetParError(1)/rho);
    cout << endl;

    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf1", "[0]*exp(-[1]*x)", 0, 15);
    tf2->SetParName(0, "R_{0}");
    tf2->SetParName(1, "mi/rho");
    tf2->SetParameters(78.6, 0.1015);

    N1 = tree->Draw("X:R:err_X:err_R", "", "goff");
    TGraphErrors * graph2 = new TGraphErrors(N1, tree->GetV1(), tree->GetV2(), tree->GetV3(), tree->GetV4());
    graph2->GetXaxis()->SetTitle("X [g cm^{-2}]");
    graph2->GetYaxis()->SetTitle("R [s^{-1}]");
    
    graph2->Fit(tf2, "m");
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/rate.png");

    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << 
    " (Probability: " << tf2->GetProb() << ")." << endl;

    ref_mi_rho = 0.1035;
    z_test(tf2->GetParameter("mi/rho"), ref_mi_rho, tf1->GetParError(1));

    tfile.Close();
}