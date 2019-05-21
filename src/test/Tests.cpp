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
#include "Tests.h"

const string Tests::testFile = "src/test/tests.txt";
const string Tests::commentPrefix = "//";

Tests::Tests(): file(testFile.c_str()) { }

// read (string, correct grammars) pairs from file, 
// run compression and compare results
void Tests::runTests() {
    const int numTests = strToInt(readLine());        
    for (int i = 0; i < numTests; ++i) {
        // read string and compress
        string str = trim(readLine()); 
        cout << str << endl;        
        const char *s = str.c_str();
        LongestFirstSaCompressor compressor(s, strlen(s));        
        CfGrammar* g = compressor.compress();
        // read solution        
        const int grammarSize = strToInt(readLine());
        //cout << grammarSize << endl;
        ostringstream ss;
        for (int j = 0; j < grammarSize; ++j) {
            string gline = trim(readLine());
            //cout << gline << endl;
            ss << gline << endl;
        }
        // get grammar string representation and compare
        string result = g->toString(); 
        bool gmiss = false;
        if (ss.str() == result) cout << "grammar match";
        else { cout << "!grammar mismatch"; gmiss= true; }
        // get grammar expansion and compare with input string
        bool emiss = false;
        string exp = g->expand();
        if (exp == str) cout << " expansion match";
        else { cout << " !expansion mismatch"; emiss = true; }
        cout << endl;                
        
        if (gmiss) {
            cout << "correct grammar: " << endl;
            cout << ss.str();
            cout << "result grammar: " << endl;
            cout << result;            
        }
        
        if (emiss) {
            cout << "result expansion: " << endl;
            cout << exp;
        }
        
        delete g;
        cout<<endl;
    }
}

int Tests::strToInt(string str) {
    return atoi(str.c_str());
}

/** skip all lines that are comments and whitespace, 
 * read and return first non-commented line. 
 * whitespaces are cleared from the beginning of the line. 
 * in case of error return empty string */
string Tests::readLine() {
    const int buffLen = 1000; char lineBuff[buffLen]; 
    string s;    
    while (true) {
        if (file.good() == false) return "";
        file >> ws;
        if (file.good() == false) return "";
        file.getline(lineBuff, buffLen);
        if (file.good() == false && file.eof() == false) return "";        
        if (isComment(lineBuff) == false) break;
    }    
    return lineBuff;
}

// returns true if str starts with commentPrefix
bool Tests::isComment(string str) {
    const int strBegin = str.find_first_not_of(" \t"); // skip trailing whitespace    
    return (str.substr(strBegin, commentPrefix.length()) == commentPrefix);
}

string Tests::trim(string str) {
    if (str == "") return str;
    string ws = " \t";    
    const int start = str.find_first_not_of(ws);
    const int end = str.find_last_not_of(ws);
    const int len = end - start + 1;
    return str.substr(start, len);        
}