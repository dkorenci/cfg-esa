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
/** Modified implementation from 
 * http://rosettacode.org/wiki/Sorting_algorithms/Radix_sort */

#include <algorithm>
#include <iostream>
#include <iterator>
 
// Radix sort comparator for positive integers
class radix_test {
    const int bit; // bit position [0..31] to examine
public:
    radix_test(int offset) : bit(offset) {} // constructor
 
    bool operator()(int value) const { // function call operator    
        return !(value & (1 << bit)); // 0 bit to left partition
    }
};

// Most significant digit radix sort (recursive)
void msd_radix_sort(int *first, int *last, int msb) {
    if (first != last && msb >= 0) {
        int *mid = std::partition(first, last, radix_test(msb));
        msb--; // decrement most-significant-bit
        msd_radix_sort(first, mid, msb); // sort left partition
        msd_radix_sort(mid, last, msb); // sort right partition
    }
}

// return most significant bit of a positive integer, or -1 for 0
int most_significant_bit(int num) {
    int msb = -1;
    while (num != 0) { msb++; num >>= 1; }
    return msb;    
}

// sort array of positive integers
void radix_sort(int *array, int len) {
    // find max
    int max = array[0];
    for (int i = 1; i < len; ++i) if (array[i] > max) max = array[i];
    int msb = most_significant_bit(max);
    if (msb == -1) return; // max is 0, therefore all numbers are zero    
    // do sort
    msd_radix_sort(array, array + len, msb);
}