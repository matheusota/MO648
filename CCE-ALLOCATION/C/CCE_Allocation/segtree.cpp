#include "segtree.h"

// this constructor builds the segtree
SegTree::SegTree(vector<int> arr){
    n = arr.size();

    // insert leaf nodes in tree
    for(int i = 0; i < n; i++)
        tree[n + i] = arr[i];

    // build the tree by calculating parents
    for(int i = n - 1; i > 0; --i)
        tree[i] = tree[i<<1] + tree[i<<1 | 1];
}

// function to update a tree node
void SegTree::update(int p, int value){
    // set value at position p
    tree[p + n] = value;
    p = p + n;

    // move upward and update parents
    for (int i = p; i > 1; i >>= 1)
        tree[i>>1] = tree[i] + tree[i^1];
}

// function to get sum on interval [l, r)
int SegTree::query(int l, int r){
    int res = 0;

    // loop to find the sum in the range
    for (l += n, r += n; l < r; l >>= 1, r >>= 1)
    {
        if (l & 1)
            res += tree[l++];

        if (r & 1)
            res += tree[--r];
    }

    return res;
}

// get a leaf value
int SegTree::getLeaf(int i){
    return tree[n + i];
}
