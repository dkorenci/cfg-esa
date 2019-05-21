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
#ifndef FAST_SORT_H
#define	FAST_SORT_H


class FastSort {

public:    
    FastSort();
    virtual ~FastSort();
    
    void sort(int *a, const int N, int cutoff);    
    static void testSort();
    
private:
    // range in an integer array
    struct Range { 
        int *l; // start 
        int n; // size 
    };        
    
    static const int initStackSize;
    int stackSize;
    Range* stack;    
    
    inline void twoSort(int *a, int l);
    inline void insertionSort(int *a, const int l);
    static bool isSorted(int *a, int l);
    
    void enlargeStack();
    
    static void testShortArrays();
    static void testLongArrays();
    static void printArray(int *a, int l);
    static void getRandomArray(int *a, int len, int max, bool negative);
    
};

#endif	/* FAST_SORT_H */

