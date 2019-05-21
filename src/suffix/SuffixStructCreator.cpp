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
#include <limits.h>

#include "SuffixStructCreator.h"

SuffixStructCreator::SuffixStructCreator(const TChar* s, int l) {
    str = s;
    suffArray = 0;
    inverseSA = 0;
    lcp = 0;
    lpf = 0;
    lpfPos = 0;
    N = l;
}


/** Create suffix array by brute force suffix sort. */
SuffixStructCreator::TIndex* SuffixStructCreator::createSAwithSort() {
    suffArray = new TIndex[N];
    for (TIndex i = 0; i < N; ++i) suffArray[i] = i;       
    sort(suffArray, suffArray + N);                
    return suffArray;
}

/** Create suffix array using SAIS induced sorting algorithm */
SuffixStructCreator::TIndex* SuffixStructCreator::createSuffixArray() {   
    if (suffArray != 0) return suffArray;        
    // calculate min. char
    char minCh = CHAR_MAX;
    for (int i = 0; i < N; ++i) { if (str[i] < minCh) minCh = str[i]; }
    // convert chars to aux array to prepare for SA_IS method, 
    // ordering is preserved, but chars are shifted to avoid chars <= 0    
    int delta = (minCh <= 0) ? 1 - minCh : 0;
    int *aux = new int[N+1]; int max = INT_MIN;
    for (int i = 0; i < N; ++i) {
        aux[i] = str[i] + delta;
        if (aux[i] > max) max = aux[i];
        //cout << aux[i] << endl;
    }
    //cout << (int)minCh << endl;
    aux[N] = 0;
    // create suffix array
    // TODO, warning: SA_IS is indexing with TChar type, which is unsafe
    // if this is char, to be safe char string should be converted
    // to string of unsigned integers, keeping the ordering
    suffArray = new TIndex[N+1];
    SA_IS<int, TIndex>(aux, suffArray, N+1, max);    
    delete [] aux;
    // last suffix (sentinel char) must be lexicographically smallest
    assert(suffArray[0] == N);
    // remove sentinel char
    for (TIndex i = 1; i <= N; ++i) suffArray[i-1] = suffArray[i];

    return suffArray;
}

void SuffixStructCreator::printSuffixes() {
    for (int i = 0; i < N; ++i) {
        for (int j = suffArray[i]; j < N; ++j) cout << str[j];
        cout <<"$"<<endl;
    }
}

void SuffixStructCreator::deleteSuffixArray() { delete [] suffArray; }

/* Create inverse suffix array. requires: suffix array */
SuffixStructCreator::TIndex* SuffixStructCreator::createInverseSA() {
    if (inverseSA != 0) return inverseSA;

    inverseSA = new TIndex[N];
    for (TIndex i = 0; i < N; ++i)
        inverseSA[suffArray[i]] = i;

    return inverseSA;
}

void SuffixStructCreator::deleteInverseSA() { delete [] inverseSA; }

SuffixStructCreator::TIndex* SuffixStructCreator::createLCPBruteForce() {
    lcp = new TIndex[N+1];    
    lcp[0] = 0;
    for (TIndex i = 1; i < N; ++i) {
        lcp[i] = calcLcp(suffArray[i-1], suffArray[i]);        
    }
    lcp[N] = 0;

    return lcp;
}

/** Create LCP array using Kasai(et.al.)'s algorithm. 
 * Requires: suffix array, inverse suffix array. */
SuffixStructCreator::TIndex* SuffixStructCreator::createLCPArray() {
    if (lcp != 0) return lcp;
    
    lcp = new TIndex[N+1];    

    TIndex l = 0, sai;
    for (TIndex i = 0 ; i < N ; ++i) {
        sai = inverseSA[i];
        if (sai > 0) {
            TIndex k = suffArray[sai-1];
            while ( i+l < N && k+l < N && str[i+l] == str[k+l] ) l++;
        }
        else {
            l = 0;
        }
        lcp[sai] = l;
        l = l > 1 ? l-1 : 0;
    }
    lcp[N] = 0;

    return lcp;
}
 
void SuffixStructCreator::deleteLCPArray() { delete [] lcp; }

/** Given indexes of two suffixes in the node array, calculate their
 * longest common prefix (non-overlapping). */
SuffixStructCreator::TIndex SuffixStructCreator::calcLcp(TIndex i1, TIndex i2) {
    if (i2 < i1) { TIndex t = i1; i1 = i2; i2 = t; }

    TIndex lcp = 0;

    while (i2 < N && str[i1] == str[i2]) {
        i1++; i2++; lcp++;
    }

    return lcp;
}

/** Calculate longest previous factors for all the positions
 * in the array using suffix array and lcp array. 
 * requires: suffix array, lcp array */
SuffixStructCreator::TIndex* SuffixStructCreator::createLPFArray() {
    if (lpf != 0) return lpf;
    
    lpf = new TIndex[N];
    lpfPos = new TIndex[N];
    /* Indicator of no previous factor, just for valitation, it
     * should occur only when lpf is set to 0 for a position, could
     * produce an error with a large array. */
    const TIndex NO_PREV = numeric_limits<TIndex>::max();
    stack<TLpfInfo> S;
    TLpfInfo init(0, NO_PREV); S.push(init);
    lcp[N] = 0;

    for (TIndex i = 1; i <= N; ++i) {
        // data for current position that will be pushed after processing
        TLpfInfo newpos(i, NO_PREV);

        while (S.empty() == false) {
            bool end = (i == N);
            TIndex top = S.top().pos;
            if ( (end || suffArray[i] < suffArray[top]) ||
                 (suffArray[i] > suffArray[top]) && (lcp[i] <= lcp[top]) ) {

                if (end || suffArray[i] < suffArray[top]) {
                    if (lcp[top] > lcp[i]) {
                        lpf[suffArray[top]] = lcp[top];
                        lpfPos[suffArray[top]] = S.top().lpfPos;
                    }
                    else {
                        lpf[suffArray[top]] = lcp[i];
                        lpfPos[suffArray[top]] = suffArray[i];
                    }
                    lcp[i] = min(lcp[top], lcp[i]);
                }
                else {
                    lpf[suffArray[top]] = lcp[top];
                    lpfPos[suffArray[top]] = S.top().lpfPos;

                    if (lpf[suffArray[top]] == 0)
                        assert(lpfPos[suffArray[top]] == NO_PREV);
                }
            }
            else {
                newpos.lpfPos = suffArray[top];
                break;
            }

            S.pop();

            if (i < N && S.empty()) {
                assert(lcp[i] == 0);
            }
        }

        if (i < N) {
            S.push(newpos);
        }
    }

    double lpfAvg = 0;
    //validation
    for (TIndex i = 0; i < N; ++i) {
        if (lpf[i] != 0) {
            assert(lpfPos[i] < i);
            assert(calcLcp(lpfPos[i], i) == lpf[i]);
        }
        lpfAvg += lpf[i];
    }

    return lpf;
}

// delete LPF and LPFpos arrays
void SuffixStructCreator::deleteLPF() { 
    delete [] lpf; 
    delete [] lpfPos;
}

SuffixStructCreator::TIndex* SuffixStructCreator::createLPFPosArray() {
    if (lpfPos != 0) return lpfPos;
    createLPFArray();
    return lpfPos;
}
