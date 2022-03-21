#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <TNtuple.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

void import()
{
    string str_file_path("catena_elettronica.txt");

    string::size_type idx = str_file_path.rfind('.');
    string str_name = str_file_path;
    if (idx != string::npos)    str_name.erase(idx, str_file_path.length());

    string str_columns("Vin:Vout:CHN:err_Vin:err_Vout:err_CHN"); //

    // string to TString
    TString file_path(str_file_path);
    TString name(str_name);
    TString columns(str_columns);

    TNtuple ntuple(name, name, columns);
    ntuple.ReadFile(file_path);

    TString name_tfile = str_name.append(".root");
    TFile tfile("../doc/" + name_tfile, "recreate");
    tfile.cd();
    ntuple.Write();
    tfile.Close();
    
}