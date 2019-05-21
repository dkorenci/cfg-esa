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
#ifndef TIMER_H
#define	TIMER_H

#include <map>
#include <string>
#include <ctime>
#include <iostream>
#include <sys/times.h>
#include <unistd.h>

using namespace std;

struct TRange {
    TRange(): start(0), end(0) {}

    clock_t start;
    clock_t end;
};

static map<string, TRange> events;

void startEvent(string eventName);
void endEvent(string eventName);

double getEventTime(string event);

void printEvents();
void clearEvents();

void printNowTime(string message="");
clock_t getNowTime();
long getTicks();

#endif	/* TIMER_H */

