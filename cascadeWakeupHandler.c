//
// Created by wnoskowiak on 11.01.24.
//

#include <stdbool.h>
#include "cascadeWakeupHandler.h"

int getLength(const bool present[]) {

    int result = 0;

    for (int i = 0; i < 16; i++) {
        if (present[i]) {
            result++;
        }
    }

    return result;
};

int getFirst(const bool present[]) {

    for (int i = 0; i < 16; i++) {
        if (present[i]) {
            return i;
        }
    }

    return -1;

};


int getIndexOfNext(const bool present[], int current, int hop) {
    if (hop == 0) {
        return current;
    }
    int found = 0;
    for (int i = current + 1; i < 16; i++) {
        if (present[i]) {
            found++;
        }
        if (found == hop) {
            return current + i;
        }
    }
    return -1;
}
