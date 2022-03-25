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
void add_energies(const char * name)
{   
    ifstream infile(name);

    vector<string> peak_name;
    vector<float> chn_centroid, FWHM, err_centroid, ref_E;
    string entry1;
    float entry2, entry3, entry4, entry5;
    string line;
    getline(infile, line);                                            // skip first line

    while (infile >> entry1 >> entry2 >> entry3 >> entry4 >> entry5)
    {
        peak_name.push_back(entry1);
        chn_centroid.push_back(entry2);
        FWHM.push_back(entry3);
        err_centroid.push_back(entry4);
        ref_E.push_back(entry5);
    }

    cout << endl << "Dati:" << endl;
    for (int i = 0; i < chn_centroid.size(); i++)   
        cout << peak_name.at(i) << "\t\t" << chn_centroid.at(i) << "\t\t" 
        << FWHM.at(i) << "\t\t" << err_centroid.at(i) << ref_E.at(i) << endl;
    cout << endl;

    vector<float> E, err_E;

    float a = -1.50904e-02;
    float b = 1.65597e-03;
    float c = 4.05620e-08;
    float err_a = 1.52583e-03;
    float err_b = 8.15871e-06;
    float err_c = 8.80029e-09;    
    
    for(int i = 0; i < chn_centroid.size(); i++)   
    {
        float Ei = a + b * chn_centroid.at(i) + c *chn_centroid.at(i) *chn_centroid.at(i);
        float err_Ei = sqrt(err_a*err_a + chn_centroid.at(i)*err_b*err_b + chn_centroid.at(i)*chn_centroid.at(i)*err_c*err_c 
                        + (b + c*chn_centroid.at(i)*err_centroid.at(i)*err_centroid.at(i)));
        E.push_back(Ei);
        err_E.push_back(err_Ei);
    }
    infile.close();

    ofstream outfile(name);
    string E_name("E"), err_E_name("err_E");
    if(line.find(E_name) == string::npos)
    {
        outfile << E_name << "\n";
        for (vector<float>::const_iterator i = E.begin(); i != E.end(); i++)    outfile << *i << "\n";
    }
    if(line.find(err_E_name) == string::npos)
    {
        outfile << err_E_name << "\n";
        for (vector<float>::const_iterator i = err_E.begin(); i != err_E.end(); i++)    outfile << *i << "\n";
    }
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
/*
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
*/



void analysis3()
{
    
    add_energies("../data/picchi_Na22.txt");
    add_energies("../data/picchi_Co57.txt");
    add_energies("../data/picchi_Co60.txt");
    add_energies("../data/picchi_Cs137.txt");
    add_energies("../data/picchi_fondo.txt");
    add_energies("../data/picchi_sasso.txt");

    /*
    z_test_branch("../data/picchi_Na22");
    z_test_branch("../data/picchi_Co57");
    z_test_branch("../data/picchi_Co60");
    z_test_branch("../data/picchi_Cs137");
    z_test_branch("../data/picchi_fondo");
    z_test_branch("../data/picchi_sasso");
    */
}