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


void analysis6()
{   
    //freopen("../output/analysis6.txt", "w", stdout);
    fstream file("../data/picco_sommaCo60.txt", fstream::in);

    vector<string> peak;
    vector<float> gross_area, net_area, live_time, err_live_time;
    string entry1;
    float entry2, entry3, entry4, entry5;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5)
    {
        peak.push_back(entry1);
        gross_area.push_back(entry2);
        net_area.push_back(entry3);
        live_time.push_back(entry4);
        err_live_time.push_back(entry5);
    }
    
    vector<float> err_net_area;
    float entry6, Ssum1, err_Ssum1, Ssum2, err_Ssum2;            
    float dt = 0.000001;                   // s
    float err_dt = 0.000001;

    float Aold = 19.9;                  // kBq
    float err_Aold = 1;
    float time = 1514 * 24 * 3600;      // s (from 30/1/2018 to 23/3/2022)
    float halflife = 5.26*365*24*3600;  // s
    float tau = log(2) * halflife;
    float A = Aold * exp(-time/tau);
    float err_A = err_Aold * exp(-time/tau);

    cout << "A: " << A << " +/- " << err_A << endl;
    
    for (int i = 0; i < peak.size(); i++)
    {   
        entry6 = sqrt(2*gross_area.at(i) - net_area.at(i));
        err_net_area.push_back(entry6);     
    }

    Ssum1 = net_area.at(0) * net_area.at(1) / (A*1000 * live_time.at(0)); 
    Ssum2 = net_area.at(0) * net_area.at(1) * dt / live_time.at(0);

    err_Ssum1 = sqrt(pow(err_net_area.at(0)*net_area.at(1)/(A*1000*live_time.at(0)),2) + 
                    pow(net_area.at(0)*err_net_area.at(1)/(A*1000*live_time.at(0)),2) + 
                    pow(err_A*1000*net_area.at(0)*net_area.at(1)/(A*1000*A*1000*live_time.at(0)),2) +
                    pow(err_live_time.at(0)*net_area.at(0)*net_area.at(1)/(A*1000*live_time.at(0)*live_time.at(0)),2));
    err_Ssum2 = sqrt(pow(err_net_area.at(0)*net_area.at(1)*dt/live_time.at(0),2) + 
                    pow(net_area.at(0)*err_net_area.at(1)*dt/live_time.at(0),2) + 
                    pow(net_area.at(0)*net_area.at(1)*err_dt/live_time.at(0),2) +
                    pow(err_live_time.at(0)*net_area.at(0)*net_area.at(1)*dt/(live_time.at(0)*live_time.at(0)),2));

    /*
    string str_err_net_area("err_net"), str_R("R[kBq]"), str_err_R("err_rate[kBq]"), str_rel_err_R("rel_e_rate"), 
            str_X("X[g/cm^2]"), str_err_X("err_X[g/cm^2]");
    if(line.find(str_err_net_area) == string::npos) append_column("../data/coeff_ass.txt", "err_net", err_net_area);
    if(line.find(str_R) == string::npos)            append_column("../data/coeff_ass.txt", "R[kBq]", R);
    if(line.find(str_err_R) == string::npos)        append_column("../data/coeff_ass.txt", "err_rate[kBq]", err_R);
    if(line.find(str_rel_err_R) == string::npos)    append_column("../data/coeff_ass.txt", "rel_e_rate", rel_err_R);
    if(line.find(str_X) == string::npos)            append_column("../data/coeff_ass.txt", "X[g/cm^2]", X);
    if(line.find(str_err_X) == string::npos)        append_column("../data/coeff_ass.txt", "err_X[g/cm^2]", err_X);
    

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < n_spessori.size(); i++)   
        cout << n_spessori.at(i) << "\t\t" << x.at(i) << "\t\t" << err_x.at(i) << "\t\t" << gross_area.at(i) << "\t\t"
        << net_area.at(i) << "\t\t" << live_time.at(i) << "\t\t" << err_live_time.at(i) << "\t\t" << err_net_area.at(i) << "\t\t"
        << R.at(i) << "\t\t" << err_R.at(i) << "\t\t" << rel_err_R.at(i) << "\t\t" << X.at(i) << "\t\t" << err_X.at(i) << endl;
    cout << endl;
    */

    file.close();

    cout << endl << "Experimental value: " << net_area.at(2) << " +/- " << err_net_area.at(2) << endl;

    float err1 = sqrt(err_net_area.at(2)*err_net_area.at(2) + err_Ssum1*err_Ssum1);
    cout << endl <<"Test z (S1*S2/(A*t)): " << endl << Ssum1 << " +/- " << err_Ssum1 << endl;
    z_test(net_area.at(2), Ssum1, err1);
    cout << endl;

    float err2 = sqrt(err_net_area.at(2)*err_net_area.at(2) + err_Ssum2*err_Ssum2);
    cout << endl <<"Test z (S1*S2*dt/t): " << endl << Ssum2 << " +/- " << err_Ssum2 << endl;
    z_test(net_area.at(2), Ssum2, err2);
    cout << endl;
}