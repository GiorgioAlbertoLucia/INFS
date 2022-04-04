#include <TSystem.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TH1.h>
#include <TLatex.h>
#include <TLegend.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> 
#include <numeric>

using namespace std;

void analysis2()
{   
    //freopen("../output/analysis2.txt", "w", stdout);
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    gStyle->SetFitFormat("2.2e");

    ifstream file("../data/picco_calib.txt");

    vector<float> chn_centroid, FWHM, ref_centroid, err_centroid, err_FWHM, dE, err_dE, R, err_R;
    float entry1, entry2, entry3, entry4, entry5, entry6, entry7, entry8, entry9, entry10;
    string line;
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4)
    {
        chn_centroid.push_back(entry1);
        FWHM.push_back(entry2);
        ref_centroid.push_back(entry3);
        err_centroid.push_back(entry4);
    }

    float mean_FWHM = accumulate(FWHM.begin(), FWHM.end(), 0LL) / FWHM.size();
    for (int i = 0; i < FWHM.size(); i++)   entry5 = pow(FWHM.at(i) - mean_FWHM, 2)/FWHM.size();
    entry5 = sqrt(entry5);

    float a = -1.50904e-02;
    float b = 1.65597e-03;
    float c = 4.05620e-08;
    float err_a = 1.52583e-03;
    float err_b = 8.15871e-06;
    float err_c = 8.80029e-09;    
    
    cout << "Errori b,t " << endl;
    for(int i = 0; i < chn_centroid.size(); i++)   
    {
        err_FWHM.push_back(entry5);

        float t_value = chn_centroid.at(i) + FWHM.at(i)/2;
        float t_value_MeV = a + b * t_value + c*t_value*t_value;
        float err_t_value = sqrt(pow(err_centroid.at(i), 2) + pow(err_FWHM.at(i)/2, 2));
        float err_t_MeV = sqrt(err_a*err_a + t_value*t_value*err_b*err_b + t_value*t_value*t_value*t_value*err_c*err_c 
                        + (b*b + 4*c*c*t_value*t_value)*err_t_value*err_t_value);

        float b_value = chn_centroid.at(i) - FWHM.at(i)/2;
        float b_value_MeV = a + b * b_value + c*b_value*b_value;
        float err_b_value = sqrt(pow(err_centroid.at(i), 2) + pow(err_FWHM.at(i)/2, 2));
        float err_b_MeV = sqrt(err_a*err_a + b_value*b_value*err_b*err_b + b_value*b_value*b_value*b_value*err_c*err_c 
                        + (b*b + 4*c*c*b_value*b_value)*err_b_value*err_b_value);

        cout << b_value_MeV << "+-" << err_b_MeV << ", " <<  t_value_MeV << "+-" << err_t_MeV << endl;

        entry6 = t_value_MeV - b_value_MeV;
        dE.push_back(entry6);
        entry7 = sqrt(pow(err_t_MeV, 2) + pow(err_b_MeV, 2));
        err_dE.push_back(entry7);
    }

    for (int i = 0; i < FWHM.size(); i++)
    {
        entry8 = dE.at(i) / ref_centroid.at(i);
        R.push_back(entry8);

        entry9 = err_dE.at(i) / ref_centroid.at(i);
        err_R.push_back(entry9);
    }

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < chn_centroid.size(); i++)   
        cout << chn_centroid.at(i) << "\t\t" << FWHM.at(i) << "\t\t" << ref_centroid.at(i) << "\t\t\t" << err_centroid.at(i) 
        << "\t\t" << err_FWHM.at(i) << "\t\t" << R.at(i) << "\t\t" << err_R.at(i) << endl;
    cout << endl;

    TCanvas * canvas1 = new TCanvas("centroide [CHN]", "E [MeV]", 500, 5, 500, 600);
    canvas1->SetGrid();
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]+[1]*x+[2]*x*x", 0, 15);
    tf1->SetLineColor(38);
    tf1->SetParameter(0, -0.02);
    tf1->SetParameter(1, 0.0017);

    TGraphErrors * graph1 = new TGraphErrors(chn_centroid.size(), &chn_centroid[0], &ref_centroid[0], &err_centroid[0]);
    graph1->SetTitle("#splitline{Calibrazione}{E = p_{0} + p_{1} x + p_{2} x^{2}}; centroide [CHN]; E [MeV]");
    gPad->SetTopMargin(0.15);
    graph1->SetMarkerStyle(21);
    graph1->SetMarkerSize(0.3);
    
    graph1->Fit(tf1);
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/calibrazione.png");
    canvas1->SaveAs("../graphs/calibrazione.pdf");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;
    
    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf2", "[0] + [1]/sqrt(x)", 0, 15);
    tf2->SetLineColor(38);
    tf2->SetParameter(0, 0);
    tf2->SetParameter(1, 1);

    TGraphErrors * graph2 = new TGraphErrors(ref_centroid.size(), &ref_centroid[0], &R[0], 0, &err_R[0]);
    graph2->SetTitle("#splitline{Risoluzione}{R = p_{0} + #frac{p_{1}}{#sqrt{E}}};E [MeV];R");
    gPad->SetTopMargin(0.20);
    graph2->SetMarkerStyle(21);
    graph2->SetMarkerSize(0.3);
    
    graph2->Fit(tf2);
    graph2->Draw("ap");

    canvas1->SaveAs("../graphs/risoluzione.png");
    canvas1->SaveAs("../graphs/risoluzione.pdf");
    
    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << 
    " (Probability: " << tf2->GetProb() << ")." << endl;
    
}