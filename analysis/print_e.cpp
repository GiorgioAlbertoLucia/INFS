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
 * @brief This function adds a TBranch to an existing tree. The new branch contains energies and the error on said energies,
 * whereas the existing tree shoiuld contain the channel information the energies are evaluated from (parameters of the 
 * conversion are results of a previous interpolation).
 * @param char_name name of the .root file in ../doc/ where the tree is stored (no extension)
 */
void print_e()
{   
    TString name = "picchi_sasso";
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path, "update");

    TTree * tree;
    gDirectory->GetObject(name, tree);
    if (tree)    tree->Print();
    else cerr << "Could not find the tree in: " << gDirectory->GetName() << endl;

    tree->Scan("E");
}