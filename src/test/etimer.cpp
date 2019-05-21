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
#include <iomanip>

#include "etimer.h"

clock_t getNowTime() {
    struct tms buf;
    times(&buf);
    return buf.tms_utime;
    //return clock();
}

void printNowTime(string message) {    
    clock_t time = getNowTime();
    if (message.length() != 0) {
        cout << message << " : " << setprecision(7) << time << endl;
    }
    else {
        cout << setprecision(7) << time << endl;
    }
}

long getTicks() {
    return sysconf(_SC_CLK_TCK);
//    return CLOCKS_PER_SEC; 
}

void startEvent(string eventName) {            
     clock_t t = getNowTime();
     events[eventName].start = t;
     events[eventName].end = t - 1;
}

void endEvent(string eventName) {
     clock_t t = getNowTime();
     events[eventName].end = t;
}

void printEvents() {
    map<string, TRange>::iterator it;
    for (it = events.begin(); it != events.end(); ++it) {
        TRange rng = it->second;
        double time =  ((double)(rng.end - rng.start)) / getTicks();
        cout << it->first << ": " << setprecision(7) << time << endl;
    }
}

double getEventTime(string event) {
    TRange rng = events[event];
    return ((double)(rng.end - rng.start)) / getTicks();
}

void clearEvents() {
    events.clear();
}
