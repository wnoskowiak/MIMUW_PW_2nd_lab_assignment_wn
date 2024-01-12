//
// Created by wnoskowiak on 12.01.24.
//

#include "types.h"

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CHANNELDEFINITIONSERVICE_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CHANNELDEFINITIONSERVICE_H

channelT ptpCommunicationChannel(int source, int destination);

channelT serverChannelInputChannel(int source, server ser);

channelT serverChannelOutputChannel(int source, server ser);

channelT serverKillChannel(server ser);

channelT exitToServerChannel(server ser);

#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_CHANNELDEFINITIONSERVICE_H
