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
#ifndef CFGRAMMAR_H
#define	CFGRAMMAR_H

#include <list>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

// part of a rule, either a string or a rule (index)
struct RuleFragment {
    bool isRule;
    int ruleIndex;
    string str;
    //char *str;
};

class CfgRule {        
       
public: 
    CfgRule();    
    ~CfgRule();
    
    void addFragment(RuleFragment f); 
    int numFragments();
    RuleFragment getFragment(int i);    
    void deleteFragments();    
    
private:    
    
    vector<RuleFragment> fragments;
    
};

// context free grammar encoding a finite length char string
class CfGrammar {
public:    
    CfGrammar(int numRules);  
    virtual ~CfGrammar();
    
    void addRule(int indeks, CfgRule rule);
    string toString();
    string expand(int rule = 0);    
    void printSize(ostream& out);    
    
private:
       
    string ruleTitle(int i);
    string ruleString(int i);
    string fragmentString(RuleFragment f);
    
    vector<CfgRule> rules;
    int numRules;
        
    int numTerms;
    int numNonterms;    
    
    void calcSize();
    
};


#endif	/* CFGRAMMAR_H */

