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
#include "LongestFirstSaCompressor.h"
#include "radix_sort.h"
#include "test/etimer.h"

#include <climits>
#include <cassert>
#include <cstring>
#include <iomanip>

LongestFirstSaCompressor::LongestFirstSaCompressor(const char* s, int l, bool d, bool v): 
        str(s), N(l), debug(d), verbose(v) { }
    
LongestFirstSaCompressor::~LongestFirstSaCompressor() {
}

CfGrammar* LongestFirstSaCompressor::compress() {    
    startEvent("core_algo");
    createSuffixStructures();
    initRuleStructures();   
    initDescendingLcp();
    formRules();
    deleteSuffixStructures();
    endEvent("core_algo");
    CfGrammar* grammar = createGrammar();
    // free rest of algorithm's allocated memory
    freeRuleStructures();
    freeDescendingLcp();    
    return grammar;
}

// do actual compression
void LongestFirstSaCompressor::formRules() {
    sorted = new int[N]; 
    for (int l = maxLcp; l >= 2; --l) {
    // process lcp intervals
        if (numIntervals[l] > 0) {            
            for (int i = 0; i < numIntervals[l]; ++i) {
                int interval = descIntervals[l][i];
                processInterval(interval);
                if (debug) {
                    printStructure();
                    printRules();
                }                                
            }
        }
        // process shortened positions
        if (shortPos[l].empty() == false) {
            while (shortPos[l].empty() == false) {
                processShortened(shortPos[l].front(), l);
                shortPos[l].pop_front();
                if (debug) {
                    printStructure();
                    printRules();
                }                            
            }
        }        
    }
    delete [] sorted;
}

// process lcp interval: traverse the positions and form rule
// that replaces substring contained in the interval 
void LongestFirstSaCompressor::processInterval(int index) {
    LcpTreeNode node = lcpTree.nodes[index];
    initLocalShortened();
    if (verbose) {
        cout << "lcp int: " << node.left << " " << node.right << " " << node.lcp 
             << " " << getSubstring(suffixArray[node.left], node.lcp) << endl;
    }
    const int len = node.right - node.left + 1;
    const int lcp = node.lcp;
    // copy and sort suffix positions in the interval    
    for (int i = 0; i < len; ++i) sorted[i] = suffixArray[node.left + i];
    sortPositions(sorted, len);
    // traverse the interval positions and form rules
    RulePos first; first.pos = NO_POS;
    // replaceOk is true if new rule can be formed, ie at least two lcp length
    // substrings not are found that do not overlap with other rules and each other 
    bool replaceOk = false; 
    list<RulePos> replaceList; // list of positions where new rule can be form
    for (int i = 0; i < len; ++i) {
        int bpos = sorted[i]; // beginning of substring
        if (verbose) cout << "position: " << bpos << endl;
        // perform shallow position calculation, only RULE/NO_RULE
        // without calculating specific rule
        RulePos b = isInRule(bpos);
        RulePos e = isInRule(bpos+lcp-1);        
        if (verbose) {
            cout<<"pos: "<<b.rule<<" "<< b.pos<<" , "<< e.rule <<" "<< e.pos << endl;
        }
        if (b.rule == NO_RULE && e.rule == NO_RULE) {
            if (replaceOk) replaceList.push_back(b);
            else {
                if (first.pos == NO_POS) { 
                    first = b; 
                    // must be added as possible position for shortened replacement (only once)
                    addLocalShortened(bpos, lcp); 
                }
                else {                    
                    if (rulePossible(first, b, lcp)) {                        
                        replaceOk = true;
                        replaceList.push_back(first);
                        replaceList.push_back(b);
                    }
                }
            }
        }
        else if (b.rule == NO_RULE && e.rule != NO_RULE) { // shortened position
            // find first unreplaced position after b
            int upos = bpos+lcp-1; // initial position of e
            while (subst_table[upos] != UNREPLACED) {
                // middle of rule, jump to start
                if (subst_table[upos] <= 0) upos = -subst_table[upos];
                else upos--; // start of rule
            }
            assert(upos >= bpos);
            int l = upos - bpos + 1;
            int plcp = lcpTree.nodes[node.parent].lcp; // parent lcp
            if (l <= plcp || l < 2) continue; // too short for replacement
            addLocalShortened(bpos, l);
        }
        else if (b.rule != NO_RULE && e.rule != NO_RULE) {
        // both b and e are within rules, they must be within the same rule
        // calculate full rule positions
            b = findInRulePosition(bpos);
            e = findInRulePosition(bpos+lcp-1);            
            if (b.rule == e.rule && e.pos == b.pos+lcp-1) {
                if (replaceOk) replaceList.push_back(b);
                else {
                    if (first.pos == NO_POS) { 
                        first = b; 
                        // must be added as possible position for shortened replacement (only once)
                        addLocalShortened(bpos, lcp); 
                    }
                    else {                        
                        if (rulePossible(first, b, lcp)) {
                            replaceOk = true;
                            replaceList.push_back(first);
                            replaceList.push_back(b);
                        }
                    }
                }
            }
        }
        // case: (b.rule != NO_RULE && e.rule == NO_RULE) is ignored
        // this segment, shortened from beginning, will be traversed in later lcp interval          
    }
    
    if (replaceOk) makeReplacements(replaceList, lcp);   
    processLocalShortened();
}

// create a rule that replaces positions within the list
// at least one replacement is guaranteed, between first and second element of the list
// other replacements could overlap and in that case only some of them will be replaced
void LongestFirstSaCompressor::makeReplacements(list<RulePos> replaceList, int lcp) {
    list<RulePos>::iterator it = replaceList.begin();
    RulePos pos1 = *it; ++it;
    int rule = createNewRule(pos1, lcp);
    for (; it != replaceList.end(); ++it) {
        writeRule(rule, *it, lcp);
    }
}

// print substitution table
void LongestFirstSaCompressor::printStructure() {    
    for (int i = 0; i < N; ++i) cout << setw(3) << str[i]; cout << endl;
    for (int i = 0; i < N; ++i) { 
        cout << setw(3); 
        if (subst_table[i] == UNREPLACED) cout << "U";
        else cout << subst_table[i];         
    }
    cout << endl;
}

void LongestFirstSaCompressor::printRules() {
    for (int i = 1; i < numRules; ++i) {
        cout << "rule " << i << ":";
        for (int j = rules[i].begin; j <= rules[i].end; ++j) cout << str[j];
        cout << endl;
    }
}

string LongestFirstSaCompressor::getSubstring(int pos, int len) {
    char ss[len+1];
    int i;
    for (i = 0; i < len; ++i) ss[i] = str[pos+i];
    ss[i] = 0;
    return ss;
}

// create new rule of length l starting at position p
int LongestFirstSaCompressor::createNewRule(RulePos p, int l) {
    int rule = numRules++;
    Substring ss = writeRule(rule, p, l);
    assert(ss.start != NULL_SUBSTRING);
    if (rules.size() < numRules) rules.resize(rules.size()*2);
    rules[rule].begin = ss.start;
    rules[rule].end = ss.end;
    rules[rule].prefixRule = NO_PREFIX_RULE;
    if (verbose) {
    cout << "new rule: " << rule << " " << ss.start << " " << ss.end 
         << " " << getSubstring(ss.start, ss.end-ss.start+1) << endl;
    }
    return rule;
}

const int LongestFirstSaCompressor::NULL_SUBSTRING = -1;

// write rule of length l to position p if possible (no other rule
// occupies that interval). if rule is a prefix rule, write as prefix.
// return substring (exact string positions) to which the rule expands
LongestFirstSaCompressor::Substring LongestFirstSaCompressor::writeRule(
                                        int rule, RulePos p, int l) {     
    if (verbose) {
    cout << "write rule: " << rule << " at: " << p.rule << " " << p.pos << endl;         
    }    
    Substring ss; ss.start = ss.end = NULL_SUBSTRING;
    // check overlap with existing rules
    if (p.rule == NO_RULE) { // writing on unreplaced positions        
        for (int i = p.pos; i <= p.pos+l-1; ++i) if (subst_table[i] != UNREPLACED) return ss;    
        // write index of the rule at first rule position
        subst_table[p.pos] = rule;
        // write negative first rule position at subsequent positions
        for (int i = p.pos+1; i <= p.pos+l-1; ++i) subst_table[i] = -p.pos;
        
        ss.start = p.pos; ss.end = p.pos+l-1;
        return ss;
    }
    else { // writing inside another rule
        // check overlap with prefix rule
        int prefix = rules[p.rule].prefixRule; // prefix rule index
        int plen = 1; // prefix length
        if (prefix != NO_PREFIX_RULE) {
            // calc prefix length
            plen = rules[prefix].end - rules[prefix].begin + 1; 
            if (p.pos < plen) return ss; // overlap found
        }        
        // start position in the string where rule should be written    
        const int spos = rules[p.rule].begin + p.pos;  
        const int begin = rules[p.rule].begin;       
        // check other subrules, skip prefix and/or first rule position
        // since they are already checked
        for (int i = spos; i <= spos+l-1; ++i) {
            if (i > begin && subst_table[i] > 0 || // start of subrule  
               (subst_table[i] <= 0 && -subst_table[i] != begin)) return ss; // middle of subrule
        }
        
        ss.start = spos; ss.end = spos + l - 1;
        if (p.pos == 0) {// write as subrule
            rules[p.rule].prefixRule = rule;           
        }
        else {
            subst_table[spos] = rule;
            for (int i = spos+1; i <= spos+l-1; ++i) subst_table[i] = -spos;
        }
        return ss;
    }
}

// return true if substrings starting at p1 and p2 with length lcp
// can be replaced with a new rule, ie they are not overlapping
// and are not identical positions within the same rule
// assumes that substrings of length lcp at both positions 
// are within the same rule as the position
bool LongestFirstSaCompressor::rulePossible(RulePos p1, RulePos p2, int lcp) {    
    if (p1.rule != NO_RULE && p2.rule != NO_RULE) {
        if (p1.rule != p2.rule) return true; // different rules, replaceable
        else { // same rule
            return noOverlap(p1.pos, p2.pos, lcp);
        }
    }
    else if (p1.rule == NO_RULE && p2.rule == NO_RULE) {
        return noOverlap(p1.pos, p2.pos, lcp);
    }
    else return true;
}

// return true if substrings starting at p1 and p2 with length l do not overlap
bool LongestFirstSaCompressor::noOverlap(int p1, int p2, int l) {    
    int minPos = (p1 < p2) ? p1 : p2;
    int maxPos = (p1 > p2) ? p1 : p2;    
    return (minPos + l - 1 < maxPos);
}

const int LongestFirstSaCompressor::NO_RULE = INT_MIN;
const int LongestFirstSaCompressor::NO_POS = INT_MIN;

LongestFirstSaCompressor::RulePos LongestFirstSaCompressor::isInRule(int pos) {
    RulePos result;     
    if (subst_table[pos] == UNREPLACED) { 
        result.rule = NO_RULE; 
        result.pos = pos;
    }
    else { result.rule = 1; result.pos = 0; } // only to mark that rule exists    
    return result;    
}

// for a position within a string, find rule containing it 
// and its relative position within the that rule. 
// the rule must be at lowest level, ie no other sub-rules containing the position
LongestFirstSaCompressor::RulePos LongestFirstSaCompressor::findInRulePosition(int pos) {
    RulePos result; 
    // check if pos is contained within no rule    
    if (subst_table[pos] == UNREPLACED) {
        result.pos = pos; result.rule = NO_RULE;
        return result;
    }
    
    // pos is within the rule    
    // get rule containing pos
    int ruleStart;
    if (subst_table[pos] <= 0) ruleStart = -subst_table[pos];                    
    else ruleStart = pos;
    int rule = subst_table[ruleStart]; // get index of the rule
    pos = pos - ruleStart; // set pos to relative position within the rule    
    
    // check if pos is contained in subrules of the rule     
    // expand to the lowes-level subrule
    while (true) {        
        const Rule r = rules[rule];                
        // absolute (string) position, this is index of subst_table containing rule info
        int apos = r.begin + pos; 
        
        // if relative position within the rule is 0, just expand prefix rule
        if (pos == 0) {
            while (rules[rule].prefixRule != NO_PREFIX_RULE) {
                rule = rules[rule].prefixRule;
            }
            result.rule = rule; result.pos = 0;
            break;
        }
        
        // check if pos is within a non-prefix subrule
        // if it is, continue procedure with changed rule and pos        
        if (subst_table[apos] <= 0) {
            if (-subst_table[apos] != r.begin) { 
            // case middle of subrule or r                
                ruleStart = -subst_table[apos];
                rule = subst_table[ruleStart];
                pos = apos - ruleStart;
                continue;
            }
        }
        else if (subst_table[apos] != rule) { 
        // case start of subrule of r
            rule = subst_table[apos];
            pos = 0; // pos = apos - apos, (apos == ruleStart)
            continue;
        }       
        
        // check if pos is within prefix rule
        if (r.prefixRule == NO_PREFIX_RULE) {
            result.pos = pos; result.rule = rule;
            break;
        }
        else {
            Rule prefix = rules[r.prefixRule];
            int plen = prefix.end - prefix.begin + 1;
            // pos is also relative position within prefix rule
            if (pos < plen) { // check if it is within prefix rule
                rule = r.prefixRule;
                // pos stays the same
                continue;
            }
            else {
                result.pos = pos; result.rule = rule;
                break;                
            }
        }                
    }
    
    return result;
}

void LongestFirstSaCompressor::sortPositions(int* pos, int len) {
    sorter.sort(pos, len, 20);
    //radix_sort(pos, len);
}

const int LongestFirstSaCompressor::NO_LOCAL_SHORT = -1;

// before processing lcp interval, initialize shortened positions data
void LongestFirstSaCompressor::initLocalShortened() {
    localMin = localMax = NO_LOCAL_SHORT;
}

// add shortened position to local (one lcp interval) list
void LongestFirstSaCompressor::addLocalShortened(int pos, int l) {    
    localShortPos[l].push_back(pos);
    if (localMin == NO_LOCAL_SHORT || l < localMin) localMin = l;
    if (localMax == NO_LOCAL_SHORT || l > localMax) localMax = l;
}

// after processing lcp interval, add shortened position to global bookkeeping
// if there exists more than one position
void LongestFirstSaCompressor::processLocalShortened() {
    if (localMin == NO_LOCAL_SHORT) return; // no shortened positions
    // add positions to list, descending by length
    // keep record of top 2 lengths    
    list<ShortPos> l;
    for (int i = localMax; i >= localMin; --i) {
        if (localShortPos[i].empty() == false) {            
            ShortPos s; s.l = i;
            list<int>::iterator it;
            for (it = localShortPos[i].begin(); it != localShortPos[i].end(); ++it) {
                s.pos = *it;
                l.push_back(s);
            }
            localShortPos[i].clear();
        }
    }
    if (l.size() > 1) {
        // longest possible replacement within the list is length of second longest        
        list<ShortPos>::iterator it = l.begin(); it++;   
        shortPos[it->l].push_back(l);
    }
}

// attempt to construct new rules from a list of shortened segments 
// coming from same lcp interval
void LongestFirstSaCompressor::processShortened(const list<ShortPos> &spos, int len) {    
    // traverse the shortened positions and form rules
    RulePos first; first.pos = NO_POS;
    ShortPos firstShort;
    // replaceOk is true if new rule can be formed, ie at least two shortened
    // substrings not are found that do not overlap with other rules and each other 
    bool replaceOk = false; 
    list<RulePos> replaceList; // list of positions where new rule can be form
    if (verbose) { cout << "list with replace length " << len << endl; }
    list<ShortPos>::const_iterator it; 
    for (it = spos.begin(); it != spos.end() && (it->l >= len); ++it) {
        if (verbose) { 
            cout << "spos: " << it->pos << " slen: " << it->l 
                 << " " << getSubstring(it->pos, it->l) << endl; 
        }
        // get in rule position of beginning and end of substring 
        int bpos = it->pos;      
        int epos = bpos + len - 1;
        // perform shallow position calculation, only RULE/NO_RULE
        // without calculating specific rule
        RulePos b = isInRule(bpos);
        RulePos e = isInRule(epos);   
        if (verbose) {
            cout<<"pos: "<<b.rule<<" "<< b.pos<<" , "<< e.rule <<" "<< e.pos << endl;
        }        
        if (b.rule == NO_RULE && e.rule == NO_RULE) {
            if (replaceOk) replaceList.push_back(b);
            else {
                if (first.pos == NO_POS) { 
                    first = b; 
                    firstShort = *it; 
                }
                else {                    
                    if (rulePossible(first, b, len)) {                        
                        replaceOk = true;
                        replaceList.push_back(first);
                        replaceList.push_back(b);
                    }
                }
            }
        }        
        else if (b.rule != NO_RULE && e.rule != NO_RULE) {
        // both b and e are within rules, they must be within the same rule
        // calculate full rule positions            
            b = findInRulePosition(bpos);
            e = findInRulePosition(epos);             
            if (b.rule == e.rule && e.pos == b.pos+len-1) {
                if (replaceOk) replaceList.push_back(b);
                else {
                    if (first.pos == NO_POS) { 
                        first = b; 
                        firstShort = *it; 
                    }
                    else {                        
                        if (rulePossible(first, b, len)) {
                            replaceOk = true;
                            replaceList.push_back(first);
                            replaceList.push_back(b);
                        }
                    }
                }
            }
        }
        // case: (b.rule == NO_RULE && e.rule != NO_RULE) is impossible
        // since rules are formed descending by length and this case would 
        // mean that rule that again shortens the shortened rule must encompass
        // original shortener, ie. must be longer than it but it is processed after it 
        // case: (b.rule != NO_RULE && e.rule == NO_RULE) is ignored
        // this segment, shortened from beginning, will be traversed in later lcp interval                
    }
    
    if (replaceOk) makeReplacements(replaceList, len);        
        
    // put the rest of the list back to lists to be processed
    if (it == spos.end()) return; // no more positions left
    list<ShortPos> rest;    
    int newLen; // replacement length of the rest of the list
    for (; it != spos.end(); ++it) rest.push_back(*it);    
    if (first.pos != NO_POS) {
        // there is one position from old list (with length >= len) that can be
        // used to replace shorter positions
        newLen = rest.front().l;
        rest.push_front(firstShort);
        shortPos[newLen].push_back(rest);
    }
    else { // only shortened list remains, calculate max. replace length
        if (rest.size() < 2) return; // nothing to replace
        // read length of second longest position (positions are sorted descending by length)
        it = rest.begin(); ++it; newLen = it->l;
        shortPos[newLen].push_back(rest);
    }    
}

// create suffix array and lcp interval tree
void LongestFirstSaCompressor::createSuffixStructures() {
    ssc = new SuffixStructCreator(str, N);
    suffixArray = ssc->createSuffixArray();
    ssc->createInverseSA(); // needed for lcp array creation
    int * lcpArray = ssc->createLCPArray();
    ssc->deleteInverseSA();
    lcpTree = LcpTreeCreator::createLcpTree(lcpArray, N);
    treeStats = LcpTreeCreator::getStats(lcpTree);
    ssc->deleteLCPArray();
}

void LongestFirstSaCompressor::printStats(ostream& out) {
    out << "string_size: " << N;
    out << " interval_tree_size: " << treeStats.size;
    out << " sum_of_tree_depths: " << treeStats.p;
    double alpha = treeStats.p/(double)N;
    out << " alpha: " << alpha << endl;        
}

// delete suffix array and suffix struct creator
void LongestFirstSaCompressor::deleteSuffixStructures() {
    ssc->deleteSuffixArray();
    delete ssc;
    lcpTree.freeMemory();
}

// traverse lcp interval tree and store intervals in descending order
void LongestFirstSaCompressor::initDescendingLcp() {    
    // calculate max. lcp
    maxLcp = -1;
    for (int i = 0; i < lcpTree.size; ++i) {
        int lcp = lcpTree.nodes[i].lcp;
        if (lcp > maxLcp) maxLcp = lcp;        
    }
    int intPos[maxLcp+1];
    // calculate number of intervals per lcp
    numIntervals = new int[maxLcp+1];
    for (int i = 0; i <= maxLcp; ++i) { 
        numIntervals[i] = 0; intPos[i] = 0;
    }
    for (int i = 0; i < lcpTree.size; ++i) {
        int lcp = lcpTree.nodes[i].lcp;
        numIntervals[lcp]++;        
    }   
    // allocate space for intervals
    descIntervals = new int* [maxLcp+1];    
    for (int i = 0; i <= maxLcp; ++i ) {
        if (i > 1 && numIntervals[i] > 0) descIntervals[i] = new int[numIntervals[i]];
        else descIntervals[i] = 0;
    }    
    // write intervals to the array
    for (int i = 0; i < lcpTree.size; ++i) {
        int lcp = lcpTree.nodes[i].lcp; 
        if (lcp > 1) descIntervals[lcp][intPos[lcp]++] = i;
    }    
    // short positions bookkeeping
    shortPos = new list<list<ShortPos> > [maxLcp+1];
    localShortPos = new list<int> [maxLcp+1];
}

// free descIntervals data
void LongestFirstSaCompressor::freeDescendingLcp() {    
    for (int i = 0; i <= maxLcp; ++i) {
        if (descIntervals[i] != 0) delete descIntervals[i];
    }
    delete [] descIntervals;
    delete [] numIntervals;
    delete [] shortPos;
    delete [] localShortPos;
}

const int LongestFirstSaCompressor::UNREPLACED = INT_MAX;
const int LongestFirstSaCompressor::NO_PREFIX_RULE = INT_MIN;

// allocate and initialize structures for rule related structures
void LongestFirstSaCompressor::initRuleStructures() {
    subst_table = new int[N];    
    for (int i = 0; i < N; ++i) {
        subst_table[i] = UNREPLACED;        
    }        
    numRules = 1; // index zero is reserved for the "entire string rule"
    rules.resize(10);
}

// free memory of rule related structures
void LongestFirstSaCompressor::freeRuleStructures() {
    delete [] subst_table;    
    rules.clear();
}
    
// create (explicit) context free grammar from internal representation
CfGrammar* LongestFirstSaCompressor::createGrammar() {    
    CfGrammar* grammar = new CfGrammar(numRules);    
    grammar->addRule(0, stringToCfgRule());
    for (int i = 1; i < numRules; ++i) {        
        grammar->addRule(i, ruleToCfgRule(i));        
    }
    return grammar;
}

// convert whole string (top level rule) to rule fragments
CfgRule LongestFirstSaCompressor::stringToCfgRule() {
    CfgRule cfgRule;
    int l; // length of the string to be skipped at each step
    for (int i = 0; i < N; i += l) {        
        RuleFragment frag;
        if (subst_table[i] == UNREPLACED) { // create fragment containing a string
            // get length of unreplaced part
            for (l = 1; i+l < N && subst_table[i+l] == UNREPLACED; ++l);
            char substr[l+1];
            for (int j = 0; j < l; ++j) substr[j] = str[i+j];
            substr[l] = 0;
            frag.isRule = false;
            frag.str = substr;            
        }
        else { // create fragment containing a rule
            assert(subst_table[i] > 0); // must be a start of the rule
            int rule = subst_table[i];
            frag.isRule = true;
            frag.ruleIndex = rule;        
            // skip entire length of the rule
            l = rules[rule].length();      
        }        
        cfgRule.addFragment(frag);        
    }
    return cfgRule;
}

// convert one rule to fragments
CfgRule LongestFirstSaCompressor::ruleToCfgRule(int rule) {    
    CfgRule cfgRule;
    int b = rules[rule].begin, i;    
    // check for prefix subrule, add to fragments
    if (rules[rule].prefixRule != NO_PREFIX_RULE) {
        RuleFragment frag;
        int prefix = rules[rule].prefixRule;
        frag.isRule = true;
        frag.ruleIndex = prefix;
        i = b + rules[prefix].length(); // skip prefix        
        cfgRule.addFragment(frag);        
    }    
    else i = b;
        
    int l, e = rules[rule].end;
    for (; i <= e; i += l) {
        RuleFragment frag;
        if (i==b || subst_table[i] <= 0 && -subst_table[i] == b) { // not a subrule
            // get length of the non subrule part
            for (l = 1; i+l <= e && subst_table[i+l] <= 0 && -subst_table[i+l] == b; ++l);
            char substr[l+1];
            for (int j = 0; j < l; ++j) substr[j] = str[i+j];
            substr[l] = 0;
            frag.isRule = false;    
            frag.str = substr;            
        }
        else { // create fragment containing a subrule
            assert(subst_table[i] > 0); // must be a start of the subrule
            int rule = subst_table[i];
            frag.isRule = true;
            frag.ruleIndex = rule;        
            // skip entire length of the rule
            l = rules[rule].length();      
        }        
        cfgRule.addFragment(frag);
    }   
    return cfgRule;
}
