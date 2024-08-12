#ifndef __METAL_DEF_H__
#define __METAL_DEF_H__

#include <stdint.h>
#define MAX_KEYS    3

typedef struct BPTreeNode
{   
        // 24 bytes
        uint64_t keys[MAX_KEYS];

        // 48 bytes
        void *pointers[MAX_KEYS];

        // 49 bytes
        uint8_t level;

        // 50 bytes
        uint8_t isleaf;

        // 52 bytes
        uint16_t padding_2;
} __attribute__ ((packed)) BPTreeNode;

typedef struct BPTree
{
    BPTreeNode *root;
} BPTree;

BPTreeNode *BPTreeInitialize();
#endif
