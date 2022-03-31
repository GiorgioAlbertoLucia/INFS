#include <TSystem.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TH1I.h>
#include <TLatex.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> 
#include <numeric>

using namespace std;

TH1I* fillhist(const char * file_name)
{
    ifstream file(file_name);

    vector<int> channel, data;
    float entry1, entry2;
    string line;
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2)
    {
        channel.push_back(entry1);
        data.push_back(entry2);
    }

    TH1I * hist = new TH1I("hist", "hist", 1024, 0, 1023);
    for (int i = 0; i < channel.size(); i++) hist->Fill(channel.at(i), data.at(i));

    TCanvas * canvas = new TCanvas("canvas", "canvas", 800, 1000);
    hist->Draw();
    canvas->SaveAs("prova.png");

    return hist;
}

void drawhist()
{
    

    TCanvas * canvas = new TCanvas("canvas", "canvas", 800, 1000);
    canvas->Divide(1, 6);
    
    canvas->cd(1);
    TH1I * hist1 = fillhist("../data_ascii/spectrumCo57");
    hist1->Draw();
    delete hist1;

    canvas->cd(2);
    TH1I * hist2 = fillhist("../data_ascii/spectrumNa22");
    hist2->Draw();
    delete hist2;

    canvas->SaveAs("../graphs/histograms.png");
    canvas->SaveAs("../graphs/histograms.pdf");

}