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
    fstream file("../data/attivita.txt", fstream::in);

    vector<string> element;
    vector<float> total_count, net_area, err_net_area, live_time, err_live_time;
    string entry1;
    float entry2, entry3, entry4, entry5, entry6;

    string line;                                                    // first line
    getline(file, line);                                            // skip first line

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6)
    {
        element.push_back(entry1);
        total_count.push_back(entry2);
        net_area.push_back(entry3);
        err_net_area.push_back(entry4);
        live_time.push_back(entry5);
        err_live_time.push_back(entry6);
    }

    float ref_A = 37;                       // kBq = s^-1
    float err_ref_A = 5;

    float halflife_time = 11004.75;             // t1/2 in days
    float tau = log(2) *halflife_time; 
    float t = 6412;                             // days from when the value was registered (1/5/2004)

    float N_0 = ref_A * tau * 24 * 3600;    // convert days in seconds
    float err_N_0 = err_ref_A * tau * 24 * 3600; 

    vector<float> R, err_R;            //rate
    float rate, err_rate;

    for (int i = 0; i < element.size(); i++)
    {
        if(element.at(i) == string("Cs137-C42"))
        {
            rate = net_area.at(i) / live_time.at(i);
            err_rate = sqrt(pow(err_net_area.at(i)/live_time.at(i), 2)
                         + pow(err_live_time.at(i) * net_area.at(i), 2) / pow(live_time.at(i), 4));
        }
        else if(element.at(i) == string("Cs137-C29"))
        {
            rate = net_area.at(i) / live_time.at(i);
            err_rate = sqrt(pow(err_net_area.at(i)/live_time.at(i), 2)
                         + pow(err_live_time.at(i) * net_area.at(i), 2) / pow(live_time.at(i), 4));
        }
        R.push_back(rate);
        err_R.push_back(err_rate);
    }

    string str_R("R"), str_err_R("err_rate");
    if(line.find(str_R) == string::npos)        append_column("../data/attivita.txt", "R", R);
    if(line.find(str_err_R) == string::npos)    append_column("../data/attivita.txt", "err_rate", err_R);
    
    cout << endl << "Dati:" << endl << line << endl;
    for (int i = 0; i < element.size(); i++)   
        cout << element.at(i) << "\t" << total_count.at(i) << "\t" << net_area.at(i) << 
        "\t" << err_net_area.at(i) << "\t" << live_time.at(i) << "\t" << err_live_time.at(i) 
        << "\t" << R.at(i) << "\t" << err_R.at(i) << endl;
    cout << endl;
    
    float A = ref_A * R.at(1) / R.at(0);
    float err_A = sqrt(pow(err_ref_A*R.at(1)/R.at(0), 2) + pow(ref_A*err_R.at(1)/R.at(0), 2) 
                    + pow(ref_A*R.at(1)*err_R.at(0), 2)/pow(R.at(0), 4));

}