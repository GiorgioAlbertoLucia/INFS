#include "include_root.cpp"
#include "useful_functions.cpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string> 

using namespace std;

void analysis1()
{   
    //////////////// VERIFICA DELLA LINEARITÀ DELLA CATENA ELETTRONICA /////////////////////////

    ///////////////////// SET OUTPUT IN A FILE AND OTHER GENERAL SETTINGS ///////////////////////
    //freopen("../output/analysis1.txt", "w", stdout);
    gROOT->SetStyle("Plain");
    gStyle->SetOptFit(1110);
    gStyle->SetFitFormat("2.2e");

    ///////////////////// READ DATA FROM A FILE ////////////////////////////////////////////////

    const char * path1 = "../data/catena_elettronica.txt";
    int first_line = comment_lines(path1);
    ifstream file(path1);
    for(int i=0; i<first_line; i++) file.ignore(10000, '\n');    

    vector<float> Vin, Vout, CHN, err_Vin, err_Vout, err_CHN;
    float entry1, entry2, entry3, entry4, entry5, entry6;
    string names;
    getline(file, names);                                            // store the names of the variables

    while (file >> entry1 >> entry2 >> entry3 >> entry4 >> entry5 >> entry6)
    {
        Vin.push_back(entry1);
        Vout.push_back(entry2);
        CHN.push_back(entry3);
        err_Vin.push_back(entry4);
        err_Vout.push_back(entry5);
        err_CHN.push_back(entry6);
    }

    file.close();

    cout << endl << "Dati:" << endl << names << endl;
    for (int i = 0; i < Vin.size(); i++)   
        cout << Vin.at(i) << "\t" << Vout.at(i) << "\t" << CHN.at(i) << "\t" << err_Vin.at(i) << "\t" << 
        err_Vout.at(i) << "\t\t" << err_CHN.at(i) << endl;
    cout << endl;
    
    ////////////////// FIT 1 ///////////////////////

    TCanvas * canvas1 = new TCanvas("Vin", "Vout", 500, 5, 500, 600);
    canvas1->SetGrid();
    
    TF1 * tf1 = new TF1("tf1", "[0]+[1]*x", -15, 15);
    tf1->SetLineColor(38);

    TGraphErrors * graph1 = new TGraphErrors(Vin.size(), &Vin[0], &Vout[0], &err_Vin[0], &err_Vout[0]);
    graph1->SetTitle("#splitline{Catena Elettronica}{y = p_{0} + p_{1} x};Vin [V];Vout [V]");
    std_graph_settings(*graph1);
    
    graph1->Fit(tf1, "ER");
    graph1->Draw("ap");
    canvas1->SaveAs("../graphs/linearità1.pdf");
    
    cout << "Chi^2:" << tf1->GetChisquare() << ", number of DoF: " << tf1->GetNDF() << 
    " (Probability: " << tf1->GetProb() << ")." << endl;

    ////////////////// FIT 2 ///////////////////////

    TF1 * tf2 = new TF1("tf2", "[0]+[1]*x", -15, 15);
    tf2->SetLineColor(38);

    TGraphErrors * graph2 = new TGraphErrors(Vin.size(), &Vin[0], &CHN[0], &err_Vin[0], &err_CHN[0]);
    graph2->SetTitle("#splitline{Catena Elettronica}{y = p_{0} + p_{1} x};Vin [V];CHN");
    std_graph_settings(*graph2);
    
    graph2->Fit(tf2, "ER");
    graph2->Draw("ap");
    canvas1->SaveAs("../graphs/linearità2.pdf");

    cout << "Chi^2:" << tf2->GetChisquare() << ", number of DoF: " << tf2->GetNDF() 
    << " (Probability: " << tf2->GetProb() << ")." << endl;

}