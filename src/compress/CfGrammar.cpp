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
#include <vector>
#include <sstream>

#include "CfGrammar.h"

CfGrammar::CfGrammar(int n): rules(n), numRules(n) {}
    
void CfGrammar::addRule(int i, CfgRule rule) {
    rules[i] = rule;
}

// create string representation of the grammar
string CfGrammar::toString() {
    string s = "";    
    for (int i = 0; i < rules.size(); ++i) {
        s += ruleTitle(i)+":";
        s += ruleString(i)+"\n";        
    }
    return s;
}

// expand rule r to string
string CfGrammar::expand(int r) {
    ostringstream ss;
    CfgRule rule = rules[r]; int N = rule.numFragments();
    for (int i = 0; i < N; ++i) {
        RuleFragment frag = rule.getFragment(i);
        if (frag.isRule) ss << expand(frag.ruleIndex);
        else ss << frag.str;
    }
    return ss.str();
}

// get string "title" of the rule with index i
string CfGrammar::ruleTitle(int i) {
    ostringstream ss;
    ss << "[R" << i << "]";
    return ss.str();
}

// get string representation of the rule with index i
string CfGrammar::ruleString(int i) {
    string s = "";
    CfgRule rule = rules[i];    
    for (int i = 0; i < rule.numFragments(); ++i) {
        s += fragmentString(rule.getFragment(i));
    }    
    return s;
}

string CfGrammar::fragmentString(RuleFragment f) {
    string s;
    if (f.isRule) s = ruleTitle(f.ruleIndex);
    else s = f.str;
    return s;
}

void CfGrammar::calcSize() {
    numNonterms = 0;
    numTerms = 0;
    for(int i = 0; i < numRules; ++i) {
        CfgRule rule = rules[i];
        for (int j = 0; j < rule.numFragments(); ++j) {
            RuleFragment frag = rule.getFragment(j);
            if (frag.isRule) numNonterms++;
            else numTerms += frag.str.size();
        }
    }
}

void CfGrammar::printSize(ostream& out) {
    calcSize();
    out << "num_rules: " << numRules << " num_non_terminals: " << numNonterms 
         << " num_terminals: " << numTerms;
}

CfGrammar::~CfGrammar() {
    for (vector<CfgRule>::iterator it = rules.begin(); it != rules.end(); ++it) {
        it->deleteFragments();
    }    
}


/******** CfgRule **********/

CfgRule::CfgRule() { }

CfgRule::~CfgRule() { }

void CfgRule::deleteFragments() {    
//    if (fragments == 0) return;
//    for (int i = 0; i < fragments->size(); ++i) {
//        RuleFragment f = fragments->at(i);
//        if (f.isRule == false) delete [] f.str;
//    }
//    delete fragments;
//    fragments = 0;
}

void CfgRule::addFragment(RuleFragment f) {
    fragments.push_back(f);
}

int CfgRule::numFragments() { return fragments.size(); }

RuleFragment CfgRule::getFragment(int i) { return fragments.at(i); }

