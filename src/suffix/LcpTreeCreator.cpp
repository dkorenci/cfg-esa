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
#include "LcpTreeCreator.h"

void  LcpTree::freeMemory() { free(nodes); }

LcpTree LcpTreeCreator::createLcpTree(TIndex* lcp, TIndex N) {
    // init to largest possible size
    LcpTreeNode *lcpTree = (LcpTreeNode *)malloc(N * sizeof(LcpTreeNode));
    // start size is 1 to include the base interval at the beginning
    TIndex lcpTreeSize = 1;
    lcpTree[0].lcp = lcpTree[0].parent = 0;    
    lcpTree[0].left = 0; lcpTree[0].right = N-1;
    
    stack<TLcpInterval> lcpStack;
    TLcpInterval base(0, 0, 0);
    lcpStack.push(base);

    for (TIndex i = 1; i <= N; ++i) {
        TIndex leftBoundary = i - 1;
        TIndex newNodeIndex;
        bool intervalClosed = false;

        while (lcp[i] < lcpStack.top().lcp) {
            // the interval being closed
            TLcpInterval closedInt = lcpStack.top(); lcpStack.pop();
            // set interval boundaries
            leftBoundary = closedInt.left;                         
            TIndex rightBoundary = i - 1;            

            // create new tree node corresponding to the closed interval
            LcpTreeNode newNode;
            newNodeIndex = closedInt.nodeIndex;
            newNode.lcp = closedInt.lcp;
            newNode.parent = lcpStack.top().nodeIndex;
            newNode.left = closedInt.left;
            newNode.right = rightBoundary;
            lcpTree[newNodeIndex] = newNode;
            intervalClosed = true;                                    
        }

        if (lcp[i] > lcpStack.top().lcp) {
            // create new interval, nodeIndex is first free position in the lcpTree
            TLcpInterval newInt(leftBoundary, lcp[i], lcpTreeSize++);
            lcpStack.push(newInt);
            // if an interval was just closed in the previous loop,
            // than the interval just opened is it's parent
            if (intervalClosed) {
                lcpTree[newNodeIndex].parent = lcpStack.top().nodeIndex;
            }
        }
    }
    assert(lcpTreeSize <= N);
    // resize lcpTree array to actual number of nodes
    lcpTree = (LcpTreeNode *)realloc(lcpTree, lcpTreeSize * sizeof(LcpTreeNode));    
        
    LcpTree tree;
    tree.nodes = lcpTree;
    tree.size = lcpTreeSize;
    return tree;
}

LcpTreeStats LcpTreeCreator::getStats(LcpTree tree) {        
    long p = 0;
    for (int i = 0; i < tree.size; ++i) {
        LcpTreeNode n = tree.nodes[i];
        if (n.lcp >= 2) p += (n.right - n.left + 1);
    }    
    LcpTreeStats s; s.p = p; s.size = tree.size;
    return s;
}

void LcpTreeCreator::printIntervalTree(LcpTree tree) {
    for (int i = 0; i < tree.size; ++i) {
        LcpTreeNode node = tree.nodes[i];
        cout<<"l: "<<node.left<<" r: "<< node.right<<" lcp: "<< node.lcp 
                <<" p: "<< node.parent << endl;
    }
}
