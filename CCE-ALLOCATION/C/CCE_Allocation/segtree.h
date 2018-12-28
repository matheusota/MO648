#ifndef SEGTREE_H
#define SEGTREE_H
#include <bits/stdc++.h>
using namespace std;

// limit for array size
const int N = 100000;

class SegTree
{
    public:
        int n;
        int tree[2 * N];

        SegTree(vector<int> &arr);
        void update(int p, int value);
        int query(int l, int r);
        int getLeaf(int i);
};

#endif // SEGTREE_H
