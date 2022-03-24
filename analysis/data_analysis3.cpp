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
void add_energies(const char * char_name)
{   
    TString name = char_name;
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path, "update");

    TTree * tree;
    gDirectory->GetObject(name, tree);
    if (tree)    tree->Print();
    else cerr << "Could not find the tree in: " << gDirectory->GetName() << endl;

    float CHN;
    float err_CHN;
    float E;
    float err_E;

    float a = -1.50904e-02;
    float b = 1.65597e-03;
    float c = 4.05620e-08;
    float err_a = 8.39254e-04;
    float err_b = 4.48769e-06;
    float err_c = 4.84062e-09;    
    
    auto bE = tree->Branch("E", &E);
    auto berr_E = tree->Branch("err_E", &err_E);
    tree->SetBranchAddress("centroid", &CHN);
    tree->SetBranchAddress("err_centroid", &err_CHN);
    for(Long64_t i = 0; i < tree->GetEntries(); i++)   
    {
        tree->GetEvent(i);
        E = a + b * CHN + c * CHN * CHN;
        err_E = sqrt(err_a*err_a + CHN*err_b*err_b + CHN*CHN*err_c*err_c + (b + c*CHN)*err_CHN*err_CHN);
        bE->Fill();
        berr_E->Fill();
    }
    tree->Write("", TObject::kOverwrite);

    cout << endl<< name << ": " << endl;
    tree->Scan("peak_name:E:err_E");

    tfile.Close();
}

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

/**
 * @brief This function takes a file in ../doc (no extension) where a TTree is stored. Executes a Z Test over all the values
 * stored in a specific branch in respect to the reference values stored in another branch (uncertainties are also stored in
 * some other branch).
 * 
 * @param file_name name of the .root file in ../doc (no extension)
 * @param exp_branch name of the branch where the experimental values are stored
 * @param ref_branch name of the branch where the reference values are stored
 * @param err_branch name of the branch where the experimental uncertainties are stored
 * @param name_branch name of the branch where the description of the data in the line is stored
 */
void z_test_branch(const char * file_name)
{
    TString name = file_name;
    TString file_path = "../doc/" + name + ".root";
    TFile tfile(file_path, "update");

    //TTree * tree = (TTree*)tfile.Get("tree");
    TTree * tree;
    gDirectory->GetObject(name, tree);
    if (tree)    tree->Print();
    else cerr << "Could not find the tree in: " << gDirectory->GetName() << endl;

    char * bname[1000];
    float exp;
    float ref;
    float err;
    tree->SetBranchAddress("peak_name", bname); 
    tree->SetBranchAddress("E", &exp);
    tree->SetBranchAddress("ref_E", &ref);
    tree->SetBranchAddress("err_E", &err);
    cout << endl;
    for(Long64_t i = 0; i < tree->GetEntries(); i++)   
    {
        tree->GetEvent(i);
        //cout << * bname << "\t";
        z_test(exp, ref, err);
    }
    tfile.Close();
}



void data_analysis3()
{
    
    add_energies("picchi_Na22");
    add_energies("picchi_Co57");
    add_energies("picchi_Co60");
    add_energies("picchi_Cs137");
    add_energies("picchi_fondo");
    add_energies("picchi_sasso");

    z_test_branch("picchi_Na22");
    z_test_branch("picchi_Co57");
    z_test_branch("picchi_Co60");
    z_test_branch("picchi_Cs137");
    z_test_branch("picchi_fondo");
    z_test_branch("picchi_sasso");
    
}