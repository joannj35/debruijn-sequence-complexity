
#include "SequenceGenerator.h"
#include <iostream>
#include <vector>
#include "ComplexityToDebruijn.h"
#include <fstream>
#include <chrono>
#include <omp.h>
using namespace std::chrono;
using namespace std;
ll total = 0;

vector<string> recovering(string filename){
    std::ifstream file(filename); // Open the file with the name "sample.txt"

    if (!file) { // Check if the file was opened successfully
        std::cerr << "Unable to open file";
        return {}; // Return with an error code
    }
    const std::string prefix = "Debruijn Sequences generated by the sub sequence ";
    size_t startPos = prefix.length();
    vector<string> sequences;
    std::string line;
    while (getline(file, line)) { // Read the file line by line
        if (line[0] == 'D') {
            size_t endPos = line.find(" : ", startPos);
            if (endPos != std::string::npos) {
                std::string binarySequence = line.substr(startPos, endPos - startPos);
                sequences.insert(sequences.begin(),binarySequence);
            }
        } else if(line[0] == 't' && line[1] == 'h' && line[2] == 'e'){
            const std::string prefix_t = "the number of Debruijn sequences: ";
            size_t startPos_t = prefix_t.length();
            size_t endPos_t = line.end() - line.begin();
            if (endPos_t != std::string::npos) {
                std::string total_seq_num = line.substr(startPos_t, endPos_t - startPos_t);
                total += stoll(total_seq_num);
            }
        }
    }
    file.close(); // Close the file
    return sequences;
}

int main(){
    omp_set_num_threads(16);
    cout << "Starting..." << endl;
    /*auto start = high_resolution_clock::now();
    SequenceGenerator se(21);
    auto d = se.getSequences();
    cout << se.getNumOfSeq() << endl;
    for (int i = 0; i < d.size(); ++i) {
        cout << d[i] << endl;
    }
    auto end = high_resolution_clock::now();
    auto duration= duration_cast<seconds>(end - start);
    cout << duration.count() << " seconds" << endl;
    return 0;*/

    int order = 7, start_complexity, end_complexity;
    cout << "Please provide the computation order:" << endl;
    cin >> order;

    cout << "Please provide the start complexity:" << endl;
    cin >> start_complexity;

    cout << "Please provide the end complexity:" << endl;
    cin >> end_complexity;

    for(int c = start_complexity; c <= end_complexity; c++){
        cout << "for complexity " << c << endl;
        cout << "please choose:" << endl;
        cout << "1. read small sequences from file" << endl;
        cout << "2. generate small sequences" << endl;
        int choice;
        cin >> choice;
        cout << "Would you like to continue a previously paused computation? (y/n)" << endl;
        vector<string> recovered;
        string continueComputation;
        cin >> continueComputation;
        if(continueComputation == "y" || continueComputation == "Y" || continueComputation == "yes" || continueComputation == "Yes"){
            string continueComputation_file = "order_"+to_string(order)+"_complexity_"+to_string(c)+"_omp.txt";
            recovered = recovering(continueComputation_file);
        }
        bool read_file = true;
        int complexity = c;
        if (choice == 2){
            read_file = false;
        }
        ComplexityToDebruijn C(complexity,order,recovered,read_file, total);
        C.compute();
    }
    cout << "Done with all complexities!!!" << endl;
}