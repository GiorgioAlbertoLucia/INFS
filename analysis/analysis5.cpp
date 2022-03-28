#include <TFile.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
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
 * @brief This function add a column of floats (with their description as top line) to an existing .txt file
 * @param file_name path to the file
 * @param col_name description of the column data
 * @param column data to add in the column
 */
void append_column(const char * file_name, const char * col_name, vector<float> column)
{
    fstream file(file_name, ios::in);
    string line;
    vector<string> file_lines;
    while(getline(file, line))  file_lines.push_back(line);     // fill a vector with the file content
    file.close();

    file.open(file_name, ios::out);
    for (int i = 0; i < file_lines.size(); i++)
    {
        if (i == 0)     file << file_lines.at(i) << "\t\t" << col_name << endl;
        else            file << file_lines.at(i) << "\t\t\t" << column.at(i-1) << endl;                          
    }
    file.close();
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
        //cout << bname << "\t";
        z_test(exp, ref, err);
    }
    tfile.Close();
}
*/

void analysis5()
{   
    //freopen("../output/analysis5.txt", "w", stdout);
    fstream file("../data/coeff_ass.txt", fstream::in);

    vector<int> n_spessori;
    vector<float> x, err_x, gross_area, net_area, live_time, err_live_time;
    int entry1;
    float entry2, entry3, entry4, entry5, entry6, entry7, entry8, entry9, entry10, entry11, entry12, entry13;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6 >> entry7 >> entry8 >> entry9 >> entry10 >> entry11
            >> entry12 >> entry13)
    {
        n_spessori.push_back(entry1);
        x.push_back(entry2);
        err_x.push_back(entry3);
        gross_area.push_back(entry4);
        net_area.push_back(entry5);
        live_time.push_back(entry6);
        err_live_time.push_back(entry7);
    }
    
    vector<float> err_net_area, R, err_R, rel_err_R, X, err_X;            //rate - rel_err = relative error
    float rho = 11.34;  // g/cm^3
    
    for (int i = 0; i < n_spessori.size(); i++)
    {   
        entry8 = sqrt(net_area.at(i) + gross_area.at(i));
        err_net_area.push_back(entry8);
        
        entry9 = net_area.at(i) / (live_time.at(i) * 1000); //kBq
        R.push_back(entry9);
        entry10 = sqrt(pow(err_net_area.at(i)/(live_time.at(i)*1000), 2) 
                    + pow(net_area.at(i)*err_live_time.at(i)/1000, 2)/pow(live_time.at(i), 4));
        err_R.push_back(entry10);

        entry11 = err_R.at(i) / R.at(i);
        rel_err_R.push_back(entry11);

        entry12 = x.at(i) * rho;    // g/cm^2
        X.push_back(entry12);
        entry13 = err_x.at(i) * rho;
        err_X.push_back(entry13);
    }
    /*
    string str_err_net_area("err_net"), str_R("R[kBq]"), str_err_R("err_rate[kBq]"), str_rel_err_R("rel_e_rate"), 
            str_X("X[g/cm^2]"), str_err_X("err_X[g/cm^2]");
    if(line.find(str_err_net_area) == string::npos) append_column("../data/coeff_ass.txt", "err_net", err_net_area);
    if(line.find(str_R) == string::npos)            append_column("../data/coeff_ass.txt", "R[kBq]", R);
    if(line.find(str_err_R) == string::npos)        append_column("../data/coeff_ass.txt", "err_rate[kBq]", err_R);
    if(line.find(str_rel_err_R) == string::npos)    append_column("../data/coeff_ass.txt", "rel_e_rate", rel_err_R);
    if(line.find(str_X) == string::npos)            append_column("../data/coeff_ass.txt", "X[g/cm^2]", X);
    if(line.find(str_err_X) == string::npos)        append_column("../data/coeff_ass.txt", "err_X[g/cm^2]", err_X);
    */

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < n_spessori.size(); i++)   
        cout << n_spessori.at(i) << "\t\t" << x.at(i) << "\t\t" << err_x.at(i) << "\t\t" << gross_area.at(i) << "\t\t"
        << net_area.at(i) << "\t\t" << live_time.at(i) << "\t\t" << err_live_time.at(i) << "\t\t" << err_net_area.at(i) << "\t\t"
        << R.at(i) << "\t\t" << err_R.at(i) << "\t\t" << rel_err_R.at(i) << "\t\t" << X.at(i) << "\t\t" << err_X.at(i) << endl;
    cout << endl;

    file.close();

    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    TCanvas * canvas1 = new TCanvas("canvas1", "R", 500, 500, 500, 600);

    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]*exp(-[1]*x)", 0, 15);
    tf1->SetParName(0, "R_{0}");
    tf1->SetParName(1, "#mu");
    tf1->SetParameters(78.6, 0.1015);

    TGraphErrors * graph1 = new TGraphErrors(x.size(), &x[0], &R[0], &err_x[0], &err_R[0]);
    graph1->GetXaxis()->SetTitle("x [cm]");
    graph1->GetYaxis()->SetTitle("R [s^{-1}]");
    
    graph1->Fit(tf1, "m");
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/rate_x.png");

    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    float ref_mi_rho = 0.1035;
    cout << endl <<"Test z su mi: " << endl;
    z_test(tf1->GetParameter("#mu")/rho, ref_mi_rho, tf1->GetParError(1)/rho);
    cout << endl;

    TCanvas * canvas2 = new TCanvas("canvas2", "R", 500, 500, 500, 600);
    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf1", "[0]*exp(-[1]*x)", 0, 25);
    tf2->SetParName(0, "R_{0}");
    tf2->SetParName(1, "#frac{#mu}{#rho}");
    tf2->SetParameters(78.6, 0.1015);

    TGraphErrors * graph2 = new TGraphErrors(X.size(), &X[0], &R[0], &err_X[0], &err_R[0]);
    graph2->GetXaxis()->SetTitle("X [g cm^{-2}]");
    graph2->GetYaxis()->SetTitle("R [s^{-1}]");
    
    graph2->Fit(tf2, "m");
    graph2->Draw("ap");
    canvas2->SaveAs("../graphs/rate_x_rho.png");

    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << 
    " (Probability: " << tf2->GetProb() << ")." << endl;

    cout << endl <<"Test z su mi/rho: " << endl;
    z_test(tf2->GetParameter("#frac{#mu}{#rho}"), ref_mi_rho, tf1->GetParError(1));
}