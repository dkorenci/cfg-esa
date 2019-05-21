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
#ifndef LONGESTFIRSTSACOMPRESSOR_H
#define	LONGESTFIRSTSACOMPRESSOR_H

#include <list>
#include <vector>
#include <iostream>

#include "suffix/LcpTreeCreator.h"
#include "suffix/SuffixStructCreator.h"
#include "CfGrammar.h"
#include "FastSort.h"

using namespace std;

class LongestFirstSaCompressor {
    
public:
    LongestFirstSaCompressor(const char* s, int l, bool d = false, bool v = false);
    virtual ~LongestFirstSaCompressor();
    
    CfGrammar* compress();
    void printStats(ostream& out);
        
private:

    const char * str;
    int N;
    
    SuffixStructCreator* ssc;
    int *suffixArray;
    LcpTree lcpTree;
    
    bool debug, verbose;
    
    // rule data structures
    static const int UNREPLACED;
    int *subst_table;    
    
    LcpTreeStats treeStats;
    
    struct Rule {
        int begin, end; // start and end within the string
        int prefixRule; // index of the prefix rule, if it exists
        inline int length() { return end - begin + 1; }        
    };
    
    FastSort sorter;
    
    static const int NO_PREFIX_RULE;
    vector<Rule> rules;
    int numRules;
    
    // structures for tracking lcp intervals by descending lcp
    int** descIntervals; 
    int *numIntervals; // number of intervals for a given lcp
    int maxLcp;
    
    // for storing sorted positions within an interval
    int *sorted;
    
    static const int NO_RULE;
    static const int NO_POS;
        
    // if rule == NO_RULE, pos is unreplaced string position
    // if rule != NO_RULE, it is rule index and pos is position within the rule
    struct RulePos {
        int rule; // rule index
        int pos; // position within a rule
    };
    
    // position from a lcp interval that can only be replaced by less than lcp
    struct ShortPos {
        int pos; // position in the string
        int l; // maximal replacement length
    };

    static const int NO_LOCAL_SHORT;
    
    list<int>* localShortPos; // short position within an interval
    // global lists of,  list of short positions each list holds short
    // position from the same lcp interval
    list<list<ShortPos> > *shortPos;     
    int localMin, localMax;
    
    static const int NULL_SUBSTRING;
    struct Substring {
        int start, end;
    };    
    
    // rule construction
    void formRules();
    void processInterval(int index);
    void sortPositions(int *pos, int len);
    inline bool rulePossible(RulePos p1, RulePos p2, int lcp);
    inline bool noOverlap(int p1, int p2, int l);
    RulePos findInRulePosition(int pos);
    RulePos isInRule(int pos);
    void makeReplacements(list<RulePos> replaceList, int lcp);
    int createNewRule(RulePos p, int l);
    Substring writeRule(int rule, RulePos p, int l);    
    string getSubstring(int pos, int len);
    
    void processShortened(const list<ShortPos>&, int len);
    void initLocalShortened();
    inline void addLocalShortened(int pos, int l);
    void processLocalShortened();
    
    void printStructure();
    void printRules();
    
    // CfGrammar construction
    CfGrammar* createGrammar();
    CfgRule stringToCfgRule();
    CfgRule ruleToCfgRule(int r);
    
    // (de)initialization methods
    void createSuffixStructures();   
    void deleteSuffixStructures();
    void initRuleStructures();
    void freeRuleStructures();
    void initDescendingLcp();
    void freeDescendingLcp();    
    
};

#endif	/* LONGESTFIRSTSACOMPRESSOR_H */

