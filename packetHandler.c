//
// Created by wnoskowiak on 10.01.24.
//

#include <string.h>
#include <stdbool.h>
#include "packetHandler.h"


int packetsNum(int count) {
    return (int) ((count / BUFF_DATA_SIZE )+1);
}

void encodePacket(int *id, int * tag, int * count, int *i, void * source, void * packet) {
    memcpy(packet, id, sizeof(int));
    memcpy(packet + sizeof(int), tag, sizeof(int));
    memcpy(packet + (2 * sizeof(int)) , count, sizeof(int));
    memcpy(packet + (3 * sizeof(int)), i, sizeof(int));
    memcpy(packet + (4 * sizeof(int)), source + (*i)*BUFF_DATA_SIZE, BUFF_DATA_SIZE);
}

void * decodePacket(int *id, int * tag, int * count, int *i, void * message) {
    *id = *(int *)message;
    *tag = *(int *)(message + sizeof(int));
    *count = *(int *)(message + 2*sizeof(int));
    *i = *(int *)(message + 3*sizeof(int));
    return message + 4*sizeof(int);
}

void decodeBarrierPacket(int *id, void * message) {
    *id = *(int *)message;
}


void constructWakePacket(bool success, bool * arrived, void * message) {

    *(bool *)(message) = success;

    memcpy(((bool *)(message))+1, arrived, 16*sizeof(bool));

}

void constructFailPacket(void * message) {

    bool arrived[16] = {false};

    constructWakePacket(false, &arrived[0], message);

}




