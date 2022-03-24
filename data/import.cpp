#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>
#include <Ttree.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

void import()
{
    string str_file_path("coeff_ass.txt");

    string::size_type idx = str_file_path.rfind('.');
    string str_name = str_file_path;
    if (idx != string::npos)    str_name.erase(idx, str_file_path.length());

    string str_columns("n_spessori:x:err_x:gross_area:net_area:time");   // peak_name/C:centroid/F:FWHM:err_centroid:ref_E
                            // "spessori:gross_area:net_area:time"

    // string to TString
    TString file_path(str_file_path);
    TString name(str_name);
    TString columns(str_columns);

    TTree tree(name, name);
    tree.ReadFile(file_path, columns);

    TString name_tfile = str_name.append(".root");
    TFile tfile("../doc/" + name_tfile, "recreate");
    tfile.cd();
    tree.Write();
    tfile.Close();
    
}