//
// Created by wnoskowiak on 11.01.24.
//

#include <stdbool.h>

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CASCADEWAKEUPHANDLER_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CASCADEWAKEUPHANDLER_H

int getLength(const bool present[]);

int getFirst(const bool present[]);

int getIndexOfNext(const bool present[], int current, int hop);

#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CASCADEWAKEUPHANDLER_H
