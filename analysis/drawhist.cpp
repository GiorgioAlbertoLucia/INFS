#include <TSystem.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TH1I.h>
#include <TLatex.h>
#include <THStack.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> 
#include <numeric>

using namespace std;

void draw_on_canvas(const char * file_name, TCanvas& canvas, TH1I& hist, const int n)
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

    //cout << endl;
    //for(int i = 0; i < channel.size(); i++) cout << channel.at(i) << data.at(i) << endl;

    hist.Reset();
    hist.SetFillColor(38);
    for (int i = 0; i < channel.size(); i++) hist.Fill(channel.at(i), data.at(i));

    cout << endl << hist.GetMean() << endl;

    canvas.cd(n);
    hist.Draw("b");
    canvas.SetGrid();
    canvas.Update();
}

void drawhist()
{
    TH1I * h1 = new TH1I("h1", "Spectrum Co57", 1024, 0, 1023);         
    TH1I * h2 = new TH1I("h2", "Spectrum Co60", 1024, 0, 1023);         
    TH1I * h3 = new TH1I("h3", "Spectrum Cs137", 1024, 0, 1023);        
    TH1I * h4 = new TH1I("h4", "Spectrum Na22", 1024, 0, 1023);         
    TH1I * h5 = new TH1I("h5", "Spectrum background", 1024, 0, 1023);   
    TH1I * h6 = new TH1I("h6", "Spectrum rock", 1024, 0, 1023);         

    TCanvas * canvas = new TCanvas("canvas", "canvas", 700, 900);
    canvas->Divide(1, 6);
    
    draw_on_canvas("../data_ascii/spectrum_Co57.txt", *canvas, *h1, 1);
    draw_on_canvas("../data_ascii/spectrum_Co60.txt", *canvas, *h2, 2);
    draw_on_canvas("../data_ascii/spectrum_Cs137.txt", *canvas, *h3, 3);
    draw_on_canvas("../data_ascii/spectrum_Na22.txt", *canvas, *h4, 4);
    draw_on_canvas("../data_ascii/spectrum_fondo.txt", *canvas, *h5, 5);
    draw_on_canvas("../data_ascii/spectrum_sasso.txt", *canvas, *h6, 6);

    canvas->SaveAs("../graphs/histograms.png");
    canvas->SaveAs("../graphs/histograms.pdf");

}