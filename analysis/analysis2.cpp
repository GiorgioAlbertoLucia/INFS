#include "include_root.cpp"
#include "useful_functions.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> 
#include <numeric>

using namespace std;

void analysis2()
{   
    ///////////////////// SET OUTPUT IN A FILE AND OTHER GENERAL SETTINGS ///////////////////////
    //freopen("../output/analysis2.txt", "w", stdout);
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1111);
    gStyle->SetFitFormat("2.2e");

    ///////////////////// ERR FWHM ////////////////////////////////////////////////
    const char * path0 = "../data/spectrumCo60.txt";
    int first_line = comment_lines(path0);
    ifstream file(path0);
    for(int i=0; i<first_line; i++) file.ignore(10000, '\n'); 

    vector<float> chn_centroid, FWHM, ref_centroid, err_centroid, err_FWHM, dE, err_dE, R, err_R;
    float entry1, entry2, entry3, entry4, entry5, entry6, entry7, entry8, entry9;
    string names;
    getline(file, names);                                            // skip first line

    while (file >> entry1 >> entry2)
    {
        chn_centroid.push_back(entry1);
        FWHM.push_back(entry2);
    }

    float start = 0;
    float mean_FWHM = accumulate(FWHM.begin(), FWHM.end(), start) / FWHM.size();
    for (int i = 0; i < FWHM.size(); i++)   entry5 += pow(FWHM.at(i) - mean_FWHM, 2)/FWHM.size();
    entry5 = sqrt(entry5);

    file.close();
    chn_centroid.clear();
    FWHM.clear();

    ///////////////////// READ DATA FROM A FILE ////////////////////////////////////////////////
    const char * path1 = "../data/picco_calib.txt";
    first_line = comment_lines(path1);
    file.open(path1);
    for(int i=0; i<first_line; i++) file.ignore(10000, '\n'); 

    getline(file, names);                                            // skip first line

    if(count_column(path1) == 4)
    {
        while (file >> entry1 >> entry2 >> entry3 >> entry4)
        {
            chn_centroid.push_back(entry1);
            FWHM.push_back(entry2);
            ref_centroid.push_back(entry3);
            err_centroid.push_back(entry4);
        }
    }
    else if(count_column(path1) == 9)
    {
        while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6 >> entry7 >> entry8 >> entry9)
        {
            chn_centroid.push_back(entry1);
            FWHM.push_back(entry2);
            ref_centroid.push_back(entry3);
            err_centroid.push_back(entry4);
        }
    }

    ///////////////////////////// ADD DATA ///////////////////////////////////////////////////////

    float a = -1.50904e-02;
    float b = 1.65597e-03;
    float c = 4.05620e-08;
    float err_a = 1.52583e-03;
    float err_b = 8.15871e-06;
    float err_c = 8.80029e-09;    
    
    cout << "Errori b,t " << endl;
    for(int i = 0; i < chn_centroid.size(); i++)   
    {
        err_FWHM.push_back(entry5);

        float t_value = chn_centroid.at(i) + FWHM.at(i)/2;
        float t_value_MeV = a + b * t_value + c*t_value*t_value;
        float err_t_value = sqrt(pow(err_centroid.at(i), 2) + pow(err_FWHM.at(i)/2, 2));
        float err_t_MeV = sqrt(err_a*err_a + t_value*t_value*err_b*err_b + t_value*t_value*t_value*t_value*err_c*err_c 
                        + pow(b + 2*c*t_value, 2)*err_t_value*err_t_value);

        float b_value = chn_centroid.at(i) - FWHM.at(i)/2;
        float b_value_MeV = a + b * b_value + c*b_value*b_value;
        float err_b_value = sqrt(pow(err_centroid.at(i), 2) + pow(err_FWHM.at(i)/2, 2));
        float err_b_MeV = sqrt(err_a*err_a + b_value*b_value*err_b*err_b + b_value*b_value*b_value*b_value*err_c*err_c 
                        + pow(b + 2*c*b_value, 2)*err_b_value*err_b_value);

        cout << b_value_MeV << "±" << err_b_MeV << ", " <<  t_value_MeV << "±" << err_t_MeV << endl;

        entry6 = t_value_MeV - b_value_MeV;
        dE.push_back(entry6);
        entry7 = sqrt(pow(err_t_MeV, 2) + pow(err_b_MeV, 2));
        err_dE.push_back(entry7);
    }

    for (int i = 0; i < FWHM.size(); i++)
    {
        entry8 = dE.at(i) / ref_centroid.at(i);
        R.push_back(entry8);

        entry9 = err_dE.at(i) / ref_centroid.at(i);
        err_R.push_back(entry9);
    }

    string str5, str6, str7, str8, str9;
    str5 = "\terr_FWHM[CHN]";
    str6 = "\tdE[MeV]";
    str7 = "\terr_dE[MeV]";
    str8 = "\tR";
    str9 = "\terr_R";
    if(names.find(str5) == string::npos)
    {
        names += str5;
        append_column(path1, str5.c_str(), err_FWHM);
    }    
    if(names.find(str6) == string::npos)
    {
        names += str6;
        append_column(path1, str6.c_str(), dE);
    }    
    if(names.find(str7) == string::npos)
    {
        names += str7;
        append_column(path1, str7.c_str(), err_dE);
    }    
    if(names.find(str8) == string::npos)
    {
        names += str8;
        append_column(path1, str8.c_str(), R);
    }    
    if(names.find(str9) == string::npos)
    {
        names += str9;
        append_column(path1, str9.c_str(), err_R);
    }    

    cout << endl << "Dati:" << endl << names << endl;
    for (int i = 0; i < chn_centroid.size(); i++)   
        cout << chn_centroid.at(i) << "\t\t" << FWHM.at(i) << "\t\t" << ref_centroid.at(i) << "\t\t\t" << err_centroid.at(i) 
        << "\t\t" << err_FWHM.at(i) << "\t\t" << R.at(i) << "\t\t" << err_R.at(i) << endl;
    cout << endl;

    TCanvas * canvas1 = new TCanvas("centroide [CHN]", "E [MeV]", 500, 5, 500, 600);
    canvas1->SetGrid();
    
    ////////////////// FIT 1 ///////////////////////
    TF1 * tf1 = new TF1("tf1", "[0]+[1]*x+[2]*x*x", 0, 1300);
    tf1->SetLineColor(38);
    tf1->SetParameter(0, -0.02);
    tf1->SetParameter(1, 0.0017);

    TGraphErrors * graph1 = new TGraphErrors(chn_centroid.size(), &chn_centroid[0], &ref_centroid[0], &err_centroid[0]);
    graph1->SetTitle("#splitline{Calibrazione}{E = p_{0} + p_{1} x + p_{2} x^{2}}; centroide [CHN]; E [MeV]");
    std_graph_settings(*graph1);
    
    graph1->Fit(tf1);
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/calibrazione.pdf");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;
    
    ////////////////// FIT 2 ///////////////////////
    TF1 * tf2 = new TF1("tf2", "[0]/sqrt(x)", 0, 1300);
    tf2->SetLineColor(38);

    TGraphErrors * graph2 = new TGraphErrors(ref_centroid.size(), &ref_centroid[0], &R[0], 0, &err_R[0]);
    graph2->SetTitle("#splitline{Risoluzione}{R = #frac{p_{1}}{#sqrt{E}}};E [MeV];R");
    std_graph_settings(*graph2);
    
    graph2->Fit(tf2);
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/risoluzione.pdf");
    
    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() << 
    " (Probability: " << tf2->GetProb() << ")." << endl;
    
}