//
// Created by wnoskowiak on 12.01.24.
//

#include "envVariableHandlers.h"
#include <string.h>
#include <stdlib.h>

#define WORLD_SIZE_VARIABLE "mimpiWorldSizeVAriable"
#define WORLD_RANK_VARIABLE "mimpiRankVAriable"



int readVariable(const char * variable) {
    const char *env_value = (const char *) getenv(variable);
    return (int)strtol(env_value, NULL, 10);
}

int readWorldSize() {
    return readVariable(WORLD_SIZE_VARIABLE);
};

int readRank() {
    return readVariable(WORLD_RANK_VARIABLE);
};
