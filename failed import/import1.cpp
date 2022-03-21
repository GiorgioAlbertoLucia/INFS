#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

/**
 * @brief counts the number of lines (entries) in a txt or csv file
 * @param file_path path to the file
 * @return int 
 */
int                         file_entries(const char * file_path)    
{
    int entries = 0;

    ifstream file;
    file.open(file_path);

    if(file.is_open())
    {
    // count entries
    string line;
    while(getline(file, line))  entries++;

    file.close();
    }
    else    cerr << "Error: unable to open file" << endl;

    return entries;
}

/**
 * @brief Counts how many columns (text separated by a space, tab or comma) there are in a given string.
 * If different lines of the file have a different amount of columns, an error is displayed.
 * @return int number of columns.
 */
int                         count_column(const char * file_path)
{
    int columns = 1;
    int save_columns = 0;       // used to check if each line has the same number of columns

    ifstream file;
    file.open(file_path);

    if(file.is_open())
    {
        for (int j = 0; j < file_entries(file_path); j++)
        {
            string row;
            getline(file, row);

            bool previous_was_space = false;

            for(int i=0; i<row.size(); i++)
            {
                if(row[i] == ' ' || row[i] == '\t' || row[i] == ',')
                {
                    if(! previous_was_space)
                    {
                        columns++;
                        previous_was_space = true;
                    }
                }
                else        previous_was_space = false;
            }

            if (j > 0 && columns != save_columns)
            {
                cerr << "Error: not all the lines have the same amount of columns. First " << j << " lines have "
                << save_columns << " columns (this value will be returned)." << endl;
                return save_columns;
            }
            
            save_columns = columns;
            columns = 1;
        }
        file.close();
    }
    else    cerr << "Error: unable to open file" << endl;
    return columns;
}

/**
 * @brief Extract single words (elements separated by space) from a string.
 * @param input string you want to extraxt words from.
 * @return vector containing single words.
 */
vector<string>              split_words(const string input)
{
    istringstream ss(input);
    string word;
    vector<string> vector1;

    while(ss >> word)   vector1.push_back(word);

    return vector1;
}

/**
 * @brief Checks if the first line of the file contains words. This function is used in txtDataset.cpp to use the words in the 
 * first line as names for the TxtData objects generated.
 * NOTE: "file1" will not be regarded as a number although it does contain a digit in it. 
 * @return true if no number is present in each element of the first line.
 * @return false if a number is found in the first line.
 */
bool                        check_words(const char * file_path)                         
{
    ifstream file;
    file.open(file_path);
    if(file.is_open())
    {
        string first_line;
        getline(file, first_line);

        bool check = true;

        vector<string> words = split_words(first_line);
        for (vector<string>::const_iterator i = words.begin(); i != words.end(); i++)
        {
            string word = *i;
            if (word.find_first_not_of("0123456789") == string::npos)  return false;
        }
        file.close();
        return true;
    }
    cerr << "Error: unable to open file" << endl;
    return false;
}

/**
 * @brief Returns a line from the file. Lines are numbered beginning with zero.
 * @param line line you want to return.
 */
string                      get_line(const char * file_path, const int line)
{
    string str;
    ifstream file;
    file.open(file_path);

    if(file.is_open())
    {   
        if(line >= file_entries(file_path))
        {
            cerr << "Error: line " << line << " does not exist. The file has " << file_entries(file_path) << " lines." << endl;
            return 0;
        }

        int i = 0;
        while(i <= line) getline(file, str);        // reads all the lines until the one you need
        
        file.close();
        return str;
    }
    else    
    {
        cerr << "Error: unable to open file" << endl;
        return str;
    }
}

/**
 * @brief Gets an element from the file as a string.
 * @param line 
 * @param column 
 * @return string 
 */
string                      get_element(const char * file_path, const int line, const int column)
{
    string file_line = get_line(file_path, line);
    vector<string> words = split_words(file_line);
    return words.at(column);
}

/** 
 * @brief  This only works with .txt files using a space (' ') or a tab as delimiter between columns. 
 * Columns are numbered beginning with zero.
 * @param column: number referring to the column you want to print.
 * @param first_row = 0: row you want to start importing from. Lines are numbered from 0.
 */
vector<double>              get_column(const char * file_path, const int column, const int first_row)       
{
    vector<double> vector1;

    ifstream file;
    file.open(file_path);

    if(file.is_open())
    {
        int n_columns = count_column(file_path);

        if(column >= n_columns)
        {
            cerr << "Error: column " << column << " does not exist. The file has " << n_columns << " columns." << endl;
            return vector1;
        }

        // skip lines
        string skip;
        for (int j = 0; j < first_row; j++) getline(file, skip);

        int i = 0;
        while (! file.eof())
        {   
            string column_element;
            file >> column_element;
            if(i == column)
            {
                double element = atof(column_element.c_str());
                vector1.push_back(element);
            }
            i++;
            if(i == n_columns)  i = 0;
        }

        file.close();
    }
    else    cout << "Error: unable to open file" << endl;
    return vector1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FILE

chrono::steady_clock::time_point start = chrono::steady_clock::now();
void import1(){

    char * c_file_path;
    string file_path("test.txt");
    c_file_path = &file_path[0];
    // cout << "Enter the path to the file you want to read: " << endl;
    // cin >> * c_file_path;

    for (int i = 0; i < count_column(c_file_path); i++)
    {
        char name [get_element(c_file_path, 0, i).length()+1];
        strcpy(name, get_element(c_file_path, 0, i).c_str());              // first line is the name
        vector<double> data = get_column(c_file_path, i, 1);               // imports data from the second line

        double min = *min_element(data.begin(), data.end());
        double max = *max_element(data.begin(), data.end());
        TH1D * hist = new TH1D(name, name, file_entries(c_file_path), min, max);

        for (vector<double>::const_iterator i = data.begin(); i != data.end(); i++)
        {
            hist->Fill(*i);
        }
        
        hist->Write();
    }

    string::size_type idx = file_path.rfind('.');
    if (idx != string::npos)    file_path.erase(idx, file_path.length());

    TString name_tfile = file_path.append(".root");
    TFile * tfile = new TFile("../doc/" + name_tfile, "recreate");

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    chrono::duration<double, milli> how_long = end-start;
    cout << how_long.count() << "ms" << endl;

}