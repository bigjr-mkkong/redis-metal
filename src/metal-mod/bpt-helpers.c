#include "stdlib.h"
#include "metal-def.h"

uint32_t key[40] = {10, 15, 20, 25, 55, 60, 65, 70, 100, 110, 120, 130, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 225, 230, 235};
uint64_t num[40] = {5392, 1873, 9284, 6241, 3765, 823, 9506, 3079, 7124, 4598,
                     2013, 8746, 5127, 198, 6453, 1389, 7842, 914, 2746, 4201, 
                     5369, 7077, 9265, 3564, 958, 6822, 5001, 3524, 6875, 9203, 3010, 
                     4562, 2838, 8843, 3632}; //this line has unsed values/nums

BPTreeNode *BPTreeInitialize()
{
    BPTreeNode *tree;

    *tree = (BPTreeNode *)malloc(10 * sizeof(BPTreeNode));
    // root node
    (*tree)[0] = (BPTreeNode){{key[0], key[4], key[8], key[12]}, {&(*tree)[1], &(*tree)[2], &(*tree)[3], &(*tree)[4]}, 1, false};
    // level 2
    (*tree)[1] = (BPTreeNode){{key[0], key[1], key[2],  key[3]},    {num + 0,  num + 1, num + 2,  num + 3},  2, true};
    (*tree)[2] = (BPTreeNode){{key[4], key[5], key[6],  key[7]},    {num + 4,  num + 5, num + 6,  num + 7},  2, true};
    (*tree)[3] = (BPTreeNode){{key[8], key[9], key[10], key[11]},   {num + 8,  num + 9, num + 10, num + 11}, 2, true};
    (*tree)[4] = (BPTreeNode){{key[12], key[13], key[14], key[15]}, {num + 12, num + 13, num + 14, &(*tree)[5]}, 2, false};
    // level 3
    (*tree)[5] = (BPTreeNode){{key[15], key[16], key[17], key[18]}, {num + 15, num + 16, num + 17, &(*tree)[6]}, 3, false};
    // level 4
    (*tree)[6] = (BPTreeNode){{key[18], key[19], key[20], key[21]}, {num + 18, num + 19, num + 20, &(*tree)[7]}, 4, false};
    // level 5
    (*tree)[7] = (BPTreeNode){{key[21], key[22], key[23], key[24]}, {num + 21, num + 22, num + 23, &(*tree)[8]}, 5, false};
    // level 6
    (*tree)[8] = (BPTreeNode){{key[24], key[25], key[26], key[27]}, {num + 24, num + 25, num + 26, &(*tree)[9]}, 6, false};
    // level 7
    (*tree)[9] = (BPTreeNode){{key[27], key[28], key[29], key[30]}, {num + 27, num + 28, num + 29, num + 30}, 7, true};
    printf("[B+ tree log] num: %lld\n", ((*tree)[1].pointers[2]));
    
   (*tree)[3] = (BPTreeNode){{data[6], data[7], data[8]}, {data + 6, data + 7, data + 8}, 3, true};
   // leaf level is a single linked list

   return tree;
}

bpt_node *toleaf(bpt_node *root, int key){
    if(root->isleaf) return root;

    int idx = 0;
    for(; idx<root->num_keys; idx++){
        if(root->keys[idx] > key){
            return toleaf(AS_NODE(root->child[idx]), key);
        }
    }
        
    if(root->child[idx]){
        return toleaf(AS_NODE(root->child[idx]), key);
    }

    printf("toleaf(): Should not be here\n");
    return NULL;
}

uint64_t getval(bpt_node *root, int key){
    bpt_node *node = toleaf(root, key);
    for(int i=0; i<node->num_keys; i++){
        if(node->keys[i] == key) return node->child[i];
    }
                
    printf("getval() cannot find value of key: %d\n", key);
    return -1;
                    
}
