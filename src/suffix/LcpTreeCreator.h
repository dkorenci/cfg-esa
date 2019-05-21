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
#ifndef LCPTREECREATOR_H
#define	LCPTREECREATOR_H

#include <stack>
#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

typedef char TSymbol;
typedef int TIndex;    

// A node in a lcp interval tree
struct LcpTreeNode {
        LcpTreeNode() {}
        LcpTreeNode(TIndex p, TIndex l): parent(p), lcp(l) {}                            
        TIndex parent; // index of the parent interval            
        TIndex lcp; // lcp depth for the node
        TIndex left, right; // left and right boundaries of the interval
};

struct LcpTree {
    LcpTreeNode* nodes;
    TIndex size;
    void freeMemory();
};

struct LcpTreeStats {
    int size; long p;    
};

class LcpTreeCreator {
public:    

    LcpTreeCreator();           
    static LcpTree createLcpTree(TIndex* lcp, TIndex N);
    static void printIntervalTree(LcpTree t);
    static LcpTreeStats getStats(LcpTree t);
      
private:    
    // structure representing lcp interval data used in tree construction algorithm
    struct TLcpInterval {        
        TLcpInterval(TIndex lf, TIndex lc, TIndex ni):left(lf), lcp(lc), nodeIndex(ni) {}        
        TIndex left; // left boundary of the interval
        TIndex lcp;  // lcp value in the interval 
        TIndex nodeIndex; // index of the corresponding lcp tree node
    };   
    
};

#endif	/* LCPTREECREATOR_H */

