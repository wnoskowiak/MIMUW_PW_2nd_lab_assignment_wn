//
// Created by wnoskowiak on 11.01.24.
//

#include <stdio.h>
#include <stdlib.h>
#include "sendRecieveHandler.h"
#include "channel.h"
#include "packetHandler.h"
#include "types.h"



void sendPacket(char * packet, channelT channel) {
    if (chsend(channel.WRITE, packet, BUFFER_SIZE) == -1) {
        perror("chsend error");
        exit(EXIT_FAILURE);
//      TODO: dodać sprzątanie po failu
    }
}

void recievePacket(char * buffer, channelT channel) {
    if (chrecv(channel.READ, buffer, BUFFER_SIZE) == -1) {
        perror("chrecv error");
        exit(EXIT_FAILURE);
//      TODO: dodać sprzątanie po failu
    }
}