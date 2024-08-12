#include "../redismodule.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "metal-def.h"

BPTreeNode *root;

int MetalChk_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, (root==NULL)?"Initialization Failed" : "Initialization Successed");
    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    if(RedisModule_Init(ctx, "metal", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if(RedisModule_CreateCommand(ctx, "metal.chk", MetalChk_RedisCommand, "fast",\
                0, 0, 0) == REDISMODULE_ERR){
        return REDISMODULE_ERR;
    }

    fprintf(stdout, "Metal module initializing\n");

    root = BPTreeInitialize();

    return REDISMODULE_OK;
}

int RedisModule_OnUnload(RedisModuleCtx *ctx){
    fprintf(stdout, "Unloading metal module\n");
    free(root);

    return REDISMODULE_OK;
}
