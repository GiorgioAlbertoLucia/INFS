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

void analysis4()
{   
    //freopen("../output/analysis4.txt", "w", stdout);
    fstream file("../data/attivita.txt", fstream::in);

    vector<string> element;
    vector<float> gross_area, net_area, live_time, err_live_time;
    string entry1;
    float entry2, entry3, entry4, entry5, entry6, entry7, entry8;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5) // >> entry6 >> entry7 >> entry8)
    {
        element.push_back(entry1);
        gross_area.push_back(entry2);
        net_area.push_back(entry3);
        live_time.push_back(entry4);
        err_live_time.push_back(entry5);
    }

    float ref_A = 37000;                        // Bq = s^-1   
    float err_ref_A = 5000;

    float halflife_time = 11004.75;             // t1/2 in days
    float tau = log(2) * halflife_time; 
    float t = 6412;                             // days from when the value was registered (1/5/2004)

    float ref_A_today = ref_A * exp(- t / tau);         // activity today
    float err_ref_A_today = err_ref_A * exp(- t / tau);

    vector<float> err_net_area, R, err_R;            //rate
    float err_net, rate, err_rate;
    
    for (int i = 0; i < element.size(); i++)
    {   
        err_net = sqrt(2*gross_area.at(i) - net_area.at(i));
        err_net_area.push_back(err_net);
        
        rate = net_area.at(i) / live_time.at(i);
        err_rate = sqrt(pow(err_net_area.at(i)/live_time.at(i), 2)
                         + pow(err_live_time.at(i) * net_area.at(i), 2) / pow(live_time.at(i), 4));
        R.push_back(rate);
        err_R.push_back(err_rate);
    }

    /*
    string str_err_net_area("err_net_area"), str_R("R"), str_err_R("err_rate");
    if(line.find(str_err_net_area) == string::npos) append_column("../data/attivita.txt", "err_net_area", err_net_area);
    if(line.find(str_R) == string::npos)            append_column("../data/attivita.txt", "R", R);
    if(line.find(str_err_R) == string::npos)        append_column("../data/attivita.txt", "err_rate", err_R);
    */

    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < element.size(); i++)   
        cout << element.at(i) << "\t" << gross_area.at(i) << "\t" << net_area.at(i) << 
        "\t" << err_net_area.at(i) << "\t" << live_time.at(i) << "\t" << err_live_time.at(i) 
        << "\t" << R.at(i) << "\t" << err_R.at(i) << endl;
    cout << endl;
    
    float A = ref_A_today * R.at(1) / R.at(0);
    float err_A = sqrt(pow(err_ref_A_today*R.at(1)/R.at(0), 2) + pow(ref_A_today*err_R.at(1)/R.at(0), 2) 
                    + pow(ref_A_today*R.at(1)*err_R.at(0), 2)/pow(R.at(0), 4));

    float ref_AC29 = 18000;                                     // value from 1/7/2017, Bq
    float err_ref_AC29 = 5000;
    float t2 = 1726;                                            // days from 1/7/2017 and 23/3/2022

    float ref_AC29_today = ref_AC29 * exp(- t2 / tau);          // activity today, Bq
    float err_ref_AC29_today = err_ref_AC29 * exp(- t2 / tau);  

    float err = sqrt(err_ref_AC29_today*err_ref_AC29_today + err_A*err_A);
    cout << "Test z sull'attività della sorgente Cs137-C29 (in Bq)" << endl;
    z_test(A, ref_AC29_today, err); 
    cout << endl << "Valore sperimentale con errore (sopra è quello propagato)" << endl;
    cout << "A = (" << A/1000 << " +/- " << err_A/1000 << ") kBq. " << endl;
    cout << endl << "Valore attuale dell'attività (dato di riferimento)" << endl;
    cout << "A = (" << ref_AC29_today/1000 << " +/- " << err_ref_AC29_today/1000 << ") kBq. " << endl;


    //////////////////////////  METODO ASSOLUTO ////////////////////////////
    float eff = 0.25;
    float err_eff = 0.01;
    float fg = 0.851;

    float d = 9.3;          // cm
    float err_d = 0.01;     // cm
    float r = 2.54;
    float G_appr = r*r/(4*d*d);
    float err_G_appr = r*r/(sqrt(2)*d*d*d)*err_d;
    float G = 0.5 * (1 - cos(atan(r/d)));
    float err_G = 0.5 * sin(atan(r/d))/(1+pow(r/d, 2))*r/(d*d) * err_d;

    vector<float> A_abs, err_A_abs, A_abs_appr, err_A_abs_appr;
    float a, err_a;

    for (int i = 0; i < R.size(); i++)
    {
        a = R.at(i) / (eff * G_appr * fg);
        err_a = sqrt( pow(err_R.at(i)/(eff*G_appr*fg), 2) + pow(err_eff*R.at(i)/(eff*eff*G_appr*fg),2)
                + pow(err_G_appr*R.at(i)/(eff*G_appr*G_appr*fg) ,2));
        A_abs_appr.push_back(a);
        err_A_abs_appr.push_back(err_a);

        a = R.at(i) / (eff * G * fg);
        err_a = sqrt( pow(err_R.at(i)/(eff*G*fg), 2) + pow(err_eff*R.at(i)/(eff*eff*G*fg),2)
                + pow(err_G*R.at(i)/(eff*G*G*fg) ,2));
        A_abs.push_back(a);
        err_A_abs.push_back(err_a);
    }

    cout << "Valori dell'attività calcolata con il metodo assoluto: " << endl;
    cout << "G approssimata: " << G_appr << " +/- " << err_G_appr << endl;
    cout << "Cs137-C42 attività (" << A_abs_appr.at(0)/1000 << "+/-" << err_A_abs_appr.at(0)/1000 << ") kBq" << endl;
    cout << "Cs137-C29 attività (" << A_abs_appr.at(1)/1000 << "+/-" << err_A_abs_appr.at(1)/1000 << ") kBq" << endl;

    cout << "G esatta: " << G << " +/- " << err_G << endl;
    cout << "Cs137-C42 attività (" << A_abs.at(0)/1000 << "+/-" << err_A_abs.at(0)/1000 << ") kBq" << endl;
    cout << "Cs137-C29 attività (" << A_abs.at(1)/1000 << "+/-" << err_A_abs.at(1)/1000 << ") kBq" << endl;

    float err2 = sqrt(err_ref_A_today*err_ref_A_today + err_A_abs.at(0)*err_A_abs.at(0));
    cout << endl << "test z: Cs137-C42 -- G corretto " << endl;
    z_test(A_abs.at(0), ref_A_today, err2);
    float err3 = sqrt(err_ref_AC29_today*err_ref_AC29_today + err_A_abs.at(1)*err_A_abs.at(1));
    cout << endl << "test z: Cs137-C29 -- G corretto " << endl;
    z_test(A_abs.at(1), ref_AC29_today, err3);

    float err4 = sqrt(err_ref_A_today*err_ref_A_today + err_A_abs_appr.at(0)*err_A_abs_appr.at(0));
    cout << endl << "test z: Cs137-C42 -- G approssimato " << endl;
    z_test(A_abs_appr.at(0), ref_A_today, err4);
    float err5 = sqrt(err_ref_AC29_today*err_ref_AC29_today + err_A_abs_appr.at(1)*err_A_abs_appr.at(1));
    cout << endl << "test z: Cs137-C29 -- G approssimato " << endl;
    z_test(A_abs_appr.at(1), ref_AC29_today, err5);
}