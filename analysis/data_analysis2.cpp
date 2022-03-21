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
int nmisure = 7;

float centroide[] = {798.13, 798.79, 798.96, 798.55, 798.33, 798.49, 799.19};
float FWHM[] = {43.3848, 43.1706, 40.4902, 42.3192, 41.8016, 42.6636, 41.5574};
float std_dev;

float mean_centr = 0;
for(int i=0; i<nmisure; i++)
{
    mean_centr += centroide[i];
}
mean_centr = mean_centr / (nmisure);


for(int i=0; i<nmisure; i++)
{
    std_dev += pow(mean_centr - centroide[i], 2);
}
std_dev = sqrt(std_dev / (nmisure-1));

cout << mean_centr << endl;
cout << std_dev << endl;

// VALORI DELLE CENTROIDI DI: Co60 (2 picchi), Cs137, Na22, Co57(2 picchi)

int nmisure2 = 5;

float centroide2[] = {799.19, 704.21, 404.96, 315.49, 82.51};               // chn
float centroide_noti[] = {1.333, 1.173, 0.6617, 0.511, 0.1221};             // MeV
float std_dev_2[nmisure2];
for(int i=0; i<nmisure2; i++)   std_dev_2[i] = std_dev;
float FWHM_centroide[] = {41.5572, 38.3889, 30.1233, 25.8185, 0.368323};    // chn    -- non utilizzare  

TCanvas * canvas1 = new TCanvas("CHN", "E[MeV] ", 500, 5, 500, 600);
canvas1->SetFillColor(0);

TF1 * tf1 = new TF1("tf1", "[0]+x*[1]", 0, 15);
tf1->SetLineColor(1);
tf1->SetParameter(0, -0.02);
tf1->SetParameter(1, 0.0017);

TGraphErrors * graph1 = new TGraphErrors(nmisure2, centroide2, centroide_noti, std_dev_2);
graph1->SetMarkerSize(0.6);
graph1->SetMarkerStyle(21);
graph1->GetXaxis()->SetTitle("CHN");
graph1->GetYaxis()->SetTitle("E [MeV]");
graph1->Fit(tf1);
graph1->Draw("ap");
canvas1->SaveAs("../graphs/calibrazione.jpg");
cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << " (Probability: " << tf1->GetProb() << ")." << endl;
cout << ",-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-" << endl;

TF1 * tf2 = new TF1("tf2", "[0]+x*[1]+x*x*[2]", 0, 15);
tf2->SetLineColor(1);
tf2->SetParameter(0, -0.02);
tf2->SetParameter(1, 0.0017);

TGraphErrors * graph2 = new TGraphErrors(nmisure2, centroide2, centroide_noti, std_dev_2);
graph2->SetMarkerSize(0.6);
graph2->SetMarkerStyle(21);
graph2->GetXaxis()->SetTitle("CHN");
graph2->GetYaxis()->SetTitle("E [MeV]");
graph2->Fit(tf2);
graph2->Draw("ap");
canvas1->SaveAs("../graphs/calibrazione_quadr.jpg");
cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << " (Probability: " << tf2->GetProb() << ")." << endl;
cout << ",-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-,-" << endl;

float chn = 0;
conversionCHNtoE(chn, tf1->GetParameter(0), tf1->GetParameter(1));
conversionCHNtoE(chn, tf1->GetParameter(0), tf1->GetParameter(1), tf1->GetParameter(2));




/*
TCanvas * canvas1 = new TCanvas("Vin", "Vout", 500, 5, 500, 600);
canvas1->SetFillColor(0);

TH1 * hist1 = new TH1F("hist1", "Calibrazione Co60", nmisure, 0, 1023);
*/

}