#include "ComplexityToDebruijn.h"
#include "set"
#include <fstream>
#include <chrono>
#include <omp.h>
#include <cstdio>
#include <cstring>

using namespace std::chrono;
map<string, int> bin_to_dec1;
#define MAX_SIZE 4194304
//When order == 6 DB_SIZE = 65
#define DB_SIZE 129

#pragma omp declare target
int n;
#pragma omp end declare target

ComplexityToDebruijn::ComplexityToDebruijn(int complexity, int order) : order(order), total_seq_num(0) {
    this->complexity = complexity;
    this->sub_complexity = this->complexity - pow(2,order - 1);
}

#pragma omp declare target
static int binaryToDecimal(const char binaryString[]) {
    int decimalValue = 0;
    int length = strlen(binaryString);

    for (int i = 0; i < length; ++i) {
        if (binaryString[i] == '1') {
            decimalValue += pow(2, length - 1 - i);
        }
    }

    return decimalValue;
}
#pragma omp end declare target

#pragma omp declare target
bool ComplexityToDebruijn::isRotation(const char* s1, const char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    if (len1 != len2) {
        return false;
    }

    if (strcmp(s1, s2) == 0) {
        return true;
    }

    char* temp = new char[2 * len1 + 1];  // +1 for the null terminator
    strcpy(temp, s1);
    strcat(temp, s1);

    bool result = (strstr(temp, s2) != nullptr);

    delete[] temp;  // free the allocated memory
    return result;
}
#pragma omp end declare target

static vector<char**> convert_to_char_ptrs(int num_batches ,vector<string> batches) {
    vector<char**> results;
    int batch_size = MAX_SIZE;
    for (int j = 0; j < num_batches; j++) {
        int start = j * batch_size;
        char** batch_data = new char*[batch_size];
        for (size_t i = 0; i < batch_size; ++i) {
            batch_data[i] = new char[batches[start+i].size() + 1];
            strcpy(batch_data[i], batches[start + i].c_str());
        }
        results.push_back(batch_data);
    }
    return results;
}

void ComplexityToDebruijn::compute() {
    n = this->order;
    cout << "small sequence start..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    SequenceGenerator sub_sequences(this->sub_complexity);
    auto sub_seq =sub_sequences.getSequences();
    ll* subseq_to_db = new ll[sub_seq.size()];
    this->up_to_1000 = vector<vector<string>>(sub_seq.size());
    int i;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration= duration_cast<std::chrono::seconds>(end - start);
    cout << "sub seq calc done in " << duration.count() << " seconds" << endl;
    std::ofstream fileout("order_"+ to_string(order)+"_complexity_"+ to_string(complexity) +"_omp.txt");
    cout << "For order "<< order << " complexity "<< complexity<< ":" << endl;
    start = std::chrono::high_resolution_clock::now();

    //-------------------GPU prep-----------------
    int num_of_batches = ceil(sub_seq.size()/MAX_SIZE);
    char* up_to_1000_batch[MAX_SIZE][1000];
    vector<char**> seq_batches = convert_to_char_ptrs(num_of_batches,sub_seq);
    ll sub_seq_to_debruijn[MAX_SIZE];
    for (auto batch : seq_batches){
        #pragma omp parallel for schedule(dynamic) shared(batch,subseq_to_db,sub_seq,n,sub_seq_to_debruijn,up_to_1000_batch) private(i) default(none)
        for(i = 0; i < MAX_SIZE; i++) {
            char* seq = batch[i];
            if (seq == nullptr) continue;
            char x[65];
            strcpy(x, seq);
            strcat(x, seq);

            if (strlen(seq) <= 16) {
                strcat(x, seq);
                strcat(x, seq);
            }
            if (strlen(seq) == 8) {
                strcat(x, seq);
                strcat(x, seq);
                strcat(x, seq);
                strcat(x, seq);
            }
            char db_seq[MAX_SIZE][DB_SIZE];
            ll num = fromSubseqToDebruijn(x,db_seq);
            sub_seq_to_debruijn[i] = num;
            for (int j = 0; j < 1000; j++) {
                up_to_1000_batch[i][j] = db_seq[j];
            }
        }
        for (int j = 0; j < this->subseq_to_debruijn.size(); ++j) {
            string seq = batch[j];
            ll num = sub_seq_to_debruijn[j];
            auto db_seq = up_to_1000_batch[j];
            fileout << "Debruijn Sequences generated by the sub sequence " << seq << " : " << endl;
            for (int k = 0; (k < 1000 && k < num); k++) {
                string str = db_seq[k];
                fileout << str << endl;
            }
            fileout << "the number of Debruijn sequences: " << num << endl << endl;
            this->total_seq_num += num;
            cout << "Sequence #" << i << ": " << seq << " - " << num << endl;
        }
    }

    fileout << "number of small sequences with complexity " << complexity - pow(2,order - 1)<< " is: " << sub_seq.size() << endl;
    fileout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    end = high_resolution_clock::now();
    duration= duration_cast<seconds>(end - start);
    cout << "total number of sequences of complexity " << complexity << " is: " << total_seq_num << endl;
    if(duration.count() < 1) {
        cout << "overall execution time is " << duration_cast<milliseconds>(end - start).count() << " milliseconds"
             << endl;
        fileout << "overall execution time is " << duration_cast<milliseconds>(end - start).count() << " milliseconds"
                << endl;
    } else if(duration.count() > 60 && duration.count() < 3600){
        cout << "overall execution time is " << duration_cast<minutes>(end - start).count() << " minutes" << endl;
        fileout << "overall execution time is " << duration_cast<minutes>(end - start).count() << " minutes" << endl;
    } else if(duration.count() >= 3600) {
        cout << "overall execution time is " << duration_cast<hours>(end - start).count() << " hours, " << duration_cast<minutes>(end - start).count() % 60 << " minutes" << endl;
        fileout << "overall execution time is " << duration_cast<hours>(end - start).count() << " hours, " << duration_cast<minutes>(end - start).count() % 60 << " minutes" << endl;
    }
    else {
        cout << "overall execution time is " << duration.count() << " seconds" << endl;
        fileout << "overall execution time is " << duration.count() << " seconds" << endl;
    }
    delete[] subseq_to_db;
}

#pragma omp declare target
static char* substring(const char* source, int start, int length) {
    char* result = new char[length + 1];  // +1 for the null terminator
    strncpy(result, source + start, length);
    result[length] = '\0';  // null terminate the resulting substring
    return result;
}
#pragma omp end declare target

#pragma omp declare target
static bool find_opposite(char options_a[][64], char options_b[][64],char a[], char b[], ll options_size){
    for (int i = 0; i < options_size; ++i) {
        if ((strcmp(options_a[i], b) == 0) && ((strcmp(options_b[i], a) == 0)))
            return true;
    }
    return false;
}
#pragma omp end declare target

#pragma omp declare target
void ComplexityToDebruijn::generateXORStrings(const char s[], char a[], char b[], int index, char options[][DB_SIZE], bool check[], char db_seq[][DB_SIZE], ll &options_size) {
    if (index == strlen(s)) {
        char a_b[DB_SIZE];
        strcpy(a_b,a);
        strcat(a_b,b);
        char b_a[DB_SIZE];
        strcpy(b_a,b);
        strcat(b_a,a);
        for (int i = n - 1; i > 0; i--) {
            char* a_b_substr = substring(a_b, index - i, n);
            char* b_a_substr = substring(b_a, index - i, n);
            int a_sub = binaryToDecimal(a_b_substr);
            int b_sub = binaryToDecimal(b_a_substr);
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
                check[a_sub] = true;
                check[b_sub] = true;
            } else {
                delete b_a_substr;
                delete a_b_substr;
                return;
            }
            delete b_a_substr;
            delete a_b_substr;
        }
        for (int op = 0; op < options_size; op++) {
            if (isRotation(a_b, options[op])) {
                return;
            }
        }
        if (options_size < 1000){
            strcpy(db_seq[options_size],a_b);
        }
        strcpy(options[options_size],a_b);
        options_size++;
        return;
    }

    const int MAX_COMBINATIONS = 2;
    const int CHARS_PER_COMBINATION = 2;

    char combinations[MAX_COMBINATIONS][CHARS_PER_COMBINATION];

    if (s[index] == '0') {
        combinations[0][0] = '0'; combinations[0][1] = '0';
        combinations[1][0] = '1'; combinations[1][1] = '1';
    } else if (s[index] == '1') {
        combinations[0][0] = '1'; combinations[0][1] = '0';
        combinations[1][0] = '0'; combinations[1][1] = '1';
    }


    for (int c = 0; c < MAX_COMBINATIONS; ++c) {
        char* combination = combinations[c];

        // Append the combination characters to a and b
        size_t len_a = strlen(a);
        size_t len_b = strlen(b);
        a[len_a] = combination[0];
        b[len_b] = combination[1];
        a[len_a + 1] = '\0';
        b[len_b + 1] = '\0';

        if (strlen(a) >= n) {
            auto a_sub = binaryToDecimal(&a[strlen(a) - n]);
            auto b_sub = binaryToDecimal(&b[strlen(b) - n]);
            if (!check[a_sub] && !check[b_sub] && a_sub != b_sub) {
                check[a_sub] = true;
                check[b_sub] = true;
                generateXORStrings(s, a, b, index + 1, options, check, db_seq, options_size);
                check[a_sub] = false;
                check[b_sub] = false;
            }
        } else {
            generateXORStrings(s, a, b, index + 1, options, check, db_seq, options_size);
        }

        // Remove last character from a and b
        a[len_a] = '\0';
        b[len_b] = '\0';
    }

}
#pragma omp end declare target

/*
 * TODO:
 * remove rotations in an efficient way
 */
#pragma omp declare target
void ComplexityToDebruijn::getAllXORStrings(const char s[], char db_seq[MAX_SIZE][DB_SIZE],ll &options_size) {

    char options[MAX_SIZE][DB_SIZE];
    char a[65], b[65];
    a[0] = '\0';
    b[0] = '\0';
    bool check[DB_SIZE];
    for (bool & i : check) {
        i = false;
    }
    //#pragma omp task private(s, a, b, options, bin_to_dec1,check) default(none)
    generateXORStrings(s, a, b, 0, options, check,db_seq, options_size);
}
#pragma omp end declare target

ll ComplexityToDebruijn::fromSubseqToDebruijn(const char seq[], char db_seq[MAX_SIZE][DB_SIZE]) {
    ll count = 0;
    vector<pair<string,string>> options;
    ll options_size = 0;
    getAllXORStrings(seq,db_seq,options_size);

    return options_size;
}

const vector<pair<string, ll>> &ComplexityToDebruijn::getSubseqToDebruijn() const {
    return subseq_to_debruijn;
}

const vector<vector<string>> &ComplexityToDebruijn::getUpTo1000() const {
    return up_to_1000;
}

ll ComplexityToDebruijn::getTotalSeqNum() const {
    return total_seq_num;
}
