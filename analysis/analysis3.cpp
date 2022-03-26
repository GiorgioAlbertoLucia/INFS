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
 * @brief This function adds a TBranch to an existing tree. The new branch contains energies and the error on said energies,
 * whereas the existing tree shoiuld contain the channel information the energies are evaluated from (parameters of the 
 * conversion are results of a previous interpolation).
 * @param char_name name of the .root file in ../doc/ where the tree is stored (no extension)
 */
void add_energies(const char * name)
{   
    fstream file(name, fstream::in);

    vector<string> peak_name;
    vector<float> chn_centroid, FWHM, err_centroid, ref_E;
    string entry1;
    float entry2, entry3, entry4, entry5, entry6, entry7;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6 >> entry7)
    {
        peak_name.push_back(entry1);
        chn_centroid.push_back(entry2);
        FWHM.push_back(entry3);
        err_centroid.push_back(entry4);
        ref_E.push_back(entry5);
    }

    cout << endl << "Dati da " << name << endl << line << endl;
    for (int i = 0; i < chn_centroid.size(); i++)   
        cout << peak_name.at(i) << "\t\t\t" << chn_centroid.at(i) << "\t\t\t" << FWHM.at(i) << 
        "\t\t\t" << err_centroid.at(i) << "\t\t\t" << ref_E.at(i) << endl;
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
    file.close();

    // if there are no columns named E and err_E, respective values are appended to the file
    string str_E("E"), str_err_E("err_energy");
    if(line.find(str_E) == string::npos)        append_column(name, "E", E);
    if(line.find(str_err_E) == string::npos)    append_column(name, "err_energy", err_E);
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
    ifstream file(file_name);

    string name;
    float entry1, entry2, entry3, ref, exp, err;

    string line;
    getline(file, line);    // skip first line

    cout << endl << "test z dal file " << file_name << endl;
    while (file >> name >> entry1 >> entry2 >> entry3 >> ref >> exp >> err)
    {
        cout << name << "\t";
        z_test(exp, ref, err);
    }
    file.close();
}




void analysis3()
{
    
    add_energies("../data/picchi_Na22.txt");
    add_energies("../data/picchi_Co57.txt");
    add_energies("../data/picchi_Co60.txt");
    add_energies("../data/picchi_Cs137.txt");
    add_energies("../data/picchi_fondo.txt");
    add_energies("../data/picchi_sasso.txt");

    
    z_test_branch("../data/picchi_Na22.txt");
    z_test_branch("../data/picchi_Co57.txt");
    z_test_branch("../data/picchi_Co60.txt");
    z_test_branch("../data/picchi_Cs137.txt");
    z_test_branch("../data/picchi_fondo.txt");
    z_test_branch("../data/picchi_sasso.txt");
    
}