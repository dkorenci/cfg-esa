// Copyright 2014 Damir Korencic
//
// This file is part of cfg_esa - program 
// for longest first context free grammar compression using enhanced suffix array 
//
// The code can be used only for the purpose of reviewing the article 
// "Using Static Suffix Array in Dynamic Application: Case
//  of Text Compression by Longest First Substitution "
// authored by Strahil Ristov and Damir Korencic
// 
// The redistribution of the code is not allowed.
// After the article is published the code will be published
// under an open source licence. 
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <fstream>
#include <cctype>
#include <iomanip>

#include "suffix/SuffixStructCreator.h"
#include "suffix/LcpTreeCreator.h"
#include "compress/radix_sort.h"
#include "compress/LongestFirstSaCompressor.h"
#include "test/Tests.h"
#include "test/etimer.h"
#include "compress/FastSort.h"

using namespace std;

void test_radix_sort() {
   int data[] = { 17, 145, 1, 0, 211, 1424, 5, 93 };
   int len = 8;
   radix_sort(data, len);    
   for (int i = 0; i < len; ++i) cout << data[i] << " ";
   //std::copy(data, data + 8, ostream_iterator<int>(std::cout, " "));    
}

// test creation of suffix array and lcp tree
void test_suffix_structs(const char * str) {    
    SuffixStructCreator cr(str, strlen(str));
    int *sa = cr.createSuffixArray();
    cr.printSuffixes();
    int N = strlen(str);
    for (int i = 0; i < N; ++i) printf("%d ", sa[i]);
    printf("\n");
    cr.createInverseSA();
    int *lcp = cr.createLCPArray();
    cr.deleteInverseSA(); cr.deleteSuffixArray();
    LcpTree tree;
    tree = LcpTreeCreator::createLcpTree(lcp, N);
    cr.deleteLCPArray();
    LcpTreeCreator::printIntervalTree(tree);   
    tree.freeMemory();
}

void experiment() {
    const char * str = "abcdefghijAdefghijBabcdefCabcdefDabcdeEabcde";    
    test_suffix_structs(str);
    LongestFirstSaCompressor comp(str, true, true);    
    CfGrammar* cfg = comp.compress();
    cout << cfg->toString() << endl;    
    delete cfg;
}

void testCompression() {
    Tests t;
    t.runTests();
}

int shell(int argc, char** argv);

int main(int argc, char** argv) {
#ifdef SHELL
    return shell(argc, argv);
#else
//    FastSort::testSort();
//    experiment();
    testCompression();   
//    return 0;                
#endif
}

char *file;
bool stats, verbose, ignorews;

struct StrSize {
    char *str;
    int size;
};

void scanOptions(int argc, char** argv);
StrSize readString(char *file);
void abortShell();

int shell(int argc, char** argv) {
    scanOptions(argc, argv);
    char * str; int l;
    if (file == 0) {
        if (argc < 2) abortShell();
        str = argv[1];
        l = strlen(str);
    }
    else {
        StrSize ss = readString(file);        
        str = ss.str;
        l = ss.size;
    }    
    // compress
    bool d = false, v = false; 
    if (verbose) { d = true; v = true; }
    LongestFirstSaCompressor comp(str, l, d, v);
    CfGrammar* cfg = comp.compress();
    // output grammar
    cout << cfg->toString();    
    free(str);
    if (stats) {
        ofstream ofs("stats.txt");
        ofs << "compression_time: " << setprecision(10) << getEventTime("core_algo") << endl;        
        cfg->printSize(ofs); ofs << endl;
        comp.printStats(ofs);
    }    
    delete cfg;     

    return 0;
}

void printUsage() {
    const char* message = 
    "cfg_esa - longest first grammar compression with suffix array\n"
    "         compress a string, output grammar and additional statistics\n"
    "usage: \n"
    "   cfg_esa string [-s -v -w] - pass string as argument\n"
    "   cfg_esa -f file [-s -v -w] - read string from file\n"
    "   use -s to print compression time and other statistics to stats.txt\n"
    "   use -v option for verbose output of algorithm work\n"
    "   use -w option to ignore whitespace characters when reading from file\n";
    cout<<message<<endl;
}
// abort shell 
void abortShell() { printUsage(); exit(1); }

// scan command line options and set parameter variables
void scanOptions(int argc, char** argv) {
    stats = false; verbose = false; ignorews = false; file = 0;
    for (int i = 1; i < argc; ++i) {
        //cout << argv[i] << endl;
        string s = argv[i];        
        if (s == "-v") verbose = true;
        if (s == "-s") stats = true;
        if (s == "-w") ignorews = true;
        if (s == "-f") {
            if (i < argc-1) file = argv[i+1];
            else abortShell();
        }
    }    
}

// read string from file, consisting of all characters, skipping whitespace
StrSize readString(char *file) {
    ifstream fstr(file); 
    int L = 10000; int pos = -1;
    char *buff = (char*)malloc(L);
    while (true) {
        if (!fstr.good()) {
            cout << "error reading file" << endl;
            abortShell();
        }
        int c = fstr.get();
        if (ignorews && isspace(c)) continue;
        if (c == EOF) break;
        pos++;
        if (pos > L-2) { // allocate more buffer space
            buff = (char*)realloc(buff, 2*L);
            L *= 2;
        }
        //cout<<pos<<" "<<(char)c<<endl;
        buff[pos] = (char)c;
    }        
    buff[pos+1] = 0;
    StrSize s; s.size = pos+1; s.str = buff;
    return s;
}