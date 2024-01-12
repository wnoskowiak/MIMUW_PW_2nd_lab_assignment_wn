//
// Created by wnoskowiak on 10.01.24.
//

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_PACKETHANDLER_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_PACKETHANDLER_H

#define BUFFER_SIZE 511

#define BUFF_DATA_SIZE  (BUFFER_SIZE - 4 * sizeof(int))

int packetsNum(int count);

void encodePacket(int *id, int * tag, int * count, int *i, void * source, void * packet);

void * decodePacket(int *id, int * tag, int * count, int *i, void * message);

void decodeBarrierPacket(int *id, void * message);

void constructFailPacket(void * message);

void constructWakePacket(bool success, bool * arrived, void * message)

#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_PACKETHANDLER_H
