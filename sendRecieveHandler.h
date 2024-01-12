//
// Created by wnoskowiak on 11.01.24.
//

#include "types.h"

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_SENDRECIEVEHANDLER_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_SENDRECIEVEHANDLER_H

void sendPacket(char * packet, channelT channel);

void recievePacket(char * buffer, channelT channel);

#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_SENDRECIEVEHANDLER_H
