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
/* 
 * File:   SuffixStructCreator.h
 * Author: dam1root
 *
 * Created on March 18, 2014, 4:06 PM
 */

#ifndef SUFFIXSTRUCTCREATOR_H
#define	SUFFIXSTRUCTCREATOR_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <stack>
#include <map>
#include <cstring>
#include <cassert>

#include "SaIsCreator.hpp"

using namespace std;

/* Creates following suffix structures: suffix & inverse suffix array, lcp array, 
 * lpf (longest previous factor) array, and lpfPos (lpf position).
 * Usage: initialize with string, call create methods to create the target array(s)
 * and all array(s) needed to construct it. 
 * Then call delete methods to delete arrays no longer neccessary.  */
class SuffixStructCreator {
    
public:
    typedef int TIndex;    
    typedef char TChar;
    
    SuffixStructCreator(const TChar *str, int l);
    
    TIndex* createSAwithSort();
    TIndex* createSuffixArray();
    void deleteSuffixArray();
    void printSuffixes();
    
    TIndex* createLCPBruteForce();
    TIndex* createLCPArray();
    void deleteLCPArray();
    
    TIndex* createLPFArray();        
    TIndex* createLPFPosArray();
    void deleteLPF();
    
    TIndex* createInverseSA();
    void deleteInverseSA();        
    
private:
    
    static bool fileExists(string file);
    void readSuffixArray();
    void writeSuffixArray();
    TIndex calcLcp(TIndex i1, TIndex i2);

    TIndex N;
    int alphabetSize;

    const TChar* str;
    TIndex* suffArray;
    TIndex* inverseSA;
    TIndex* lcp;
    TIndex* lpf;
    TIndex* lpfPos;    

    /* Position in the suffix array and position where
     * (currently) best lpf for that position starts. */
    struct TLpfInfo {
        TLpfInfo(TIndex p, TIndex lp): pos(p), lpfPos(lp) {}
        TIndex pos, lpfPos;
    };    
    
};

#endif	/* SUFFIXSTRUCTCREATOR_H */

