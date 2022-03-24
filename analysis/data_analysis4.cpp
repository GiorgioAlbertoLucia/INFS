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
    TString name = "attivita";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path);

    TNtuple * tntuple;
    gDirectory->GetObject(name, tntuple);
    if (tntuple)    tntuple->Print();
    else cerr << "Could not find the TNtuple in: " << gDirectory->GetName() << endl;
    
    float Aref_2004 = 37;
    float err_Aref_2004 = 5;

    float halflife_time = 30.15;
    float tau = log(2) *halflife_time; 
    float t = 5;

    float N_0 = Aref_2004 * tau;
    float err_N_0 = tau * err_Aref_2004; 

    float Net_Area;
    float live_time;

    float R;            //rate
    float err_R;
    float Rref;
    float err_Rref;

    tntuple->SetBranchAddress("net_area", &Net_Area);
    tntuple->SetBranchAddress("live_time", &live_time);
    for(Int_t i = 0; i < tntuple->GetEntries(); i++)   
    {
        tntuple->GetEvent(i);
        if(i=0)
        {
            Rref = Net_Area / live_time;

        }
    }
     
    
    

}