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
#ifndef TESTS_H
#define	TESTS_H

#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>

#include "compress/CfGrammar.h"
#include "compress/LongestFirstSaCompressor.h"

using namespace std;

class Tests {
public:
    Tests();
    
    void runTests();    
    
private:
    static const string testFile;
    static const string commentPrefix;
    ifstream file;

    string readLine();    
    string trim(string);
    bool isComment(string str);
    int strToInt(string str);
    
};

#endif	/* TESTS_H */

