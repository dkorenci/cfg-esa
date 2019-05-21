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
#include "FastSort.h"

#include <cstdlib>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

const int FastSort::initStackSize = 10000;

FastSort::FastSort() {
    stack = (Range *)malloc(sizeof(Range)*initStackSize); 
    stackSize = initStackSize;
}

FastSort::~FastSort() {
    free(stack);
}

void FastSort::testSort() {
    testShortArrays();
    testLongArrays();
}

void FastSort::testShortArrays() {
    vector<int*> a; vector<int> l;
    FastSort sorter;
    int a1[2] = {2,1}; a.push_back(a1); l.push_back(2);
    int a2[6] = {3,2,1,0,-2,-1}; a.push_back(a2); l.push_back(6);
    int a3[10] = {1,2,3,4,5,6,7,8,9,10}; a.push_back(a3); l.push_back(10);
    int a4[8] = {0,0,0,2,2,-1,-1,-1}; a.push_back(a4); l.push_back(8);
    int a5[10] = {-5,-3,10,-1,11,-12,4,6,-7,8}; a.push_back(a5); l.push_back(10);
    int a6[10] = {10,9,8,7,6,5,4,3,2,1}; a.push_back(a6); l.push_back(10);
    int a7[10] = {1,1,1,1,1,1,1,1,1,0}; a.push_back(a7); l.push_back(10);
    for (int i = 0; i < a.size(); ++i) {
        int *arr = a[i]; int len = l[i];
        sorter.sort(arr, len, 5);
        if (!isSorted(arr, len)) {
            cout << "array " <<  i << " not sorted " << endl;
            printArray(arr, len);
        }
    }
}

// fill array a with len radnom integers in range [negative]*-1*{0,...,max-1}
void FastSort::getRandomArray(int *a, int len, int max, bool negative) {
    srand((unsigned)time(0));
    for (int i = 0; i < len; ++i) {
        int r = rand()%max; 
        if (negative) {
            int s = rand()%2;
            if (s) r = -r;
        }
        a[i] = r;
    }
}

void FastSort::testLongArrays() {
    int maxLen = 100000;
    int lengths[5] = {1000, 10000, maxLen}; const int nl = 3;    
    FastSort sorter;
    int *array = new int[maxLen];
    const int iter = 3; // array generations per length
    for (int i = 0; i < nl; ++i) {
        for (int j = 0; j < iter; ++j) {
            int len = lengths[i], max = len*3;
            getRandomArray(array, len, max, false);
            sorter.sort(array, len, 20);
            if (!isSorted(array, len)) {
                cout << "long array not sorted " << endl;
                printArray(array, len);
            }            
        }
    }
    delete [] array;
}

void FastSort::printArray(int *a, int l) {
    int i;
    for (i = 0; i < l-1; ++i) cout<<a[i]<<","; 
    if (l > 0) { cout<<a[i]<<endl; }
}

// return true if array is sorted in ascending order
bool FastSort::isSorted(int* a, int l) {
    for (int i = 0; i < l-1; ++i) if (!(a[i] <= a[i+1])) return false;
    return true;
}

void FastSort::sort(int *a, const int N, int cutoff) {    
    if (N <= cutoff) { 
        insertionSort(a, N);
        return;
    }    
    // init the stack
    int top = 0; 
    stack[top].l = a; stack[top].n = N;
    // quicksort
    while (top >= 0) {                 
        // pop interval off the stack
        int n = stack[top].n; 
        int *l = stack[top].l; int *s = l;
        int *r = l + n - 1;
        //cout << l-a << " " << n << " " << top << " " << (stack != 0) << endl;
        top--;
        // if range is small sort with insertion sort
        if (n <= cutoff) {
            insertionSort(l, n); 
            continue;
        }        
        // pivot
        int p = s[n / 2];
        while (l <= r) {
            if (*l < p) {
                l++;
                continue;
            }
            if (*r > p) {
                r--;
                continue; 
            }
            int t = *l;
            *l++ = *r;
            *r-- = t;
        } 
        // push subintervals to the stack (resize stack if necessary)        
        if (top+3 > stackSize) enlargeStack();
        top++; stack[top].l = s; stack[top].n = r - s + 1; 
        top++; stack[top].l = l; stack[top].n = (s + n) - l;
    }
}

void  FastSort::insertionSort(int *a, const int n) {    
    if (n <= 2) { twoSort(a, n); return; }
    int i, j;
    int value;
    for (i = 1; i < n; i++) {
        value = a[i];
        for (j = i; j > 0 && value < a[j - 1]; j--) {
            a[j] = a[j - 1];
        }
        a[j] = value;
    }
}

// increase stack memory by factor 2
void FastSort::enlargeStack() {
    stackSize *= 2;
    stack = (Range*)realloc(stack, stackSize);
}

// sort array a[0,1], l must be 0, 1 or 2
void FastSort::twoSort(int *a, int l) {
    if (l == 2 && a[0] > a[1]) {
        int tmp = a[0]; a[0] = a[1]; a[1] = tmp;
    }
}

void quick_sort (int *a, const int n) {
    if (n < 2)
        return;
    int p = a[n / 2];
    int *l = a;
    int *r = a + n - 1;
    while (l <= r) {
        if (*l < p) {
            l++;
            continue;
        }
        if (*r > p) {
            r--;
            continue; // we need to check the condition (l <= r) every time we change the value of l or r
        }
        int t = *l;
        *l++ = *r;
        *r-- = t;
    }
    quick_sort(a, r - a + 1);
    quick_sort(l, a + n - l);
}

void two_insertion_sort(int *a, const int n) {
    int i, j;
    int value;
    for (i = 1; i < n; i++) {
        value = a[i];
        for (j = i; j > 0 && value < a[j - 1]; j--) {
            a[j] = a[j - 1];
        }
        a[j] = value;
    }
}



