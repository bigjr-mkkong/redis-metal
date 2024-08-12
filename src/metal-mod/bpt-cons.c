#include "stdlib.h"
#include "metal-def.h"

long long data[12] = {1035, 1040, 1045, 1055, 1060, 1065, 1070, 1080, 1085};

BPTreeNode *BPTreeInitialize()
{
    BPTreeNode *tree;
    tree = (BPTreeNode *)malloc(5 * sizeof(BPTreeNode));
    tree[0] = (BPTreeNode){{data[0], data[3], data[6]}, {&(tree[1]), &(tree[2]), &(tree[3])}, 3, 0};
    // leaf level is a single linked list
    tree[1] = (BPTreeNode){{data[0], data[1], data[2]}, {data + 0, data + 1, data + 2}, 3, 1};
    tree[2] = (BPTreeNode){{data[3], data[4], data[5]}, {data + 3, data + 4, data + 5}, 3, 1};
    tree[3] = (BPTreeNode){{data[6], data[7], data[8]}, {data + 6, data + 7, data + 8}, 3, 1};

    return tree;
}

