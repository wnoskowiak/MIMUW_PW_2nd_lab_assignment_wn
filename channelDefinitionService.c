//
// Created by wnoskowiak on 12.01.24.
//

#include "types.h"
#include "channelDefinitionService.h"

#define OFFSET 20
#define TOP_CHANNEL 1023

channelT ptpCommunicationChannel(int source, int destination) {

    channelT result;

    if(source == destination) {
        result.READ = -1;
        result.WRITE = -1;
    }

    result.READ = OFFSET + 2*(16*source + destination);
    result.WRITE = OFFSET + 2*(16*source + destination) + 1;

    return result;
}

channelT serverChannelInputChannel(int source, server ser) {

    channelT result;

    switch (ser) {

        case EXIT:

            result.READ = OFFSET + 2*(16*source + source);
            result.WRITE = OFFSET + 2*(16*source + source) + 1;
            break;

        case BARRIER:

            result.READ = OFFSET + 16*32 + 2*source;
            result.WRITE = OFFSET + 16*32 + 2*source + 1;
            break;


        case BCAST:

            result.READ = OFFSET + 17*32 + 2*source;
            result.WRITE = OFFSET + 17*32 + 2*source + 1;
            break;

        case REDUCE:

            result.READ = OFFSET + 18*32 + 2*source;
            result.WRITE = OFFSET + 18*32 + 2*source + 1;
            break;
    }

    return result;

}

channelT serverChannelOutputChannel(int source, server ser) {

    channelT result;

    switch (ser) {

        case EXIT:

            result.READ = OFFSET + 19*32 + 2*source;
            result.WRITE = OFFSET + 19*32 + 2*source + 1;
            break;

        case BARRIER:

            result.READ = OFFSET + 20*32 + 2*source;
            result.WRITE = OFFSET + 20*32 + 2*source + 1;
            break;


        case BCAST:

            result.READ = OFFSET + 21*32 + 2*source;
            result.WRITE = OFFSET + 21*32 + 2*source + 1;
            break;

        case REDUCE:

            result.READ = OFFSET + 22*32 + 2*source;
            result.WRITE = OFFSET + 22*32 + 2*source + 1;
            break;
    }

    return result;

}

channelT serverKillChannel(server ser) {

    channelT result;

    switch (ser) {

        case EXIT:

            result.READ = TOP_CHANNEL - 1;
            result.WRITE = TOP_CHANNEL;
            break;

        case BARRIER:

            result.READ = TOP_CHANNEL - 3;
            result.WRITE = TOP_CHANNEL - 2;
            break;


        case BCAST:

            result.READ = TOP_CHANNEL - 5;
            result.WRITE = TOP_CHANNEL - 4;
            break;

        case REDUCE:

            result.READ = TOP_CHANNEL - 7;
            result.WRITE = TOP_CHANNEL - 6;
            break;
    }

    return result;
    
}

channelT exitToServerChannel(server ser) {

    channelT result;

    switch (ser) {

        case EXIT:

            result.READ = -1;
            result.WRITE = -1;
            break;

        case BARRIER:

            result.READ = TOP_CHANNEL - 9;
            result.WRITE = TOP_CHANNEL - 8;
            break;


        case BCAST:

            result.READ = TOP_CHANNEL - 11;
            result.WRITE = TOP_CHANNEL - 10;
            break;

        case REDUCE:

            result.READ = TOP_CHANNEL - 13;
            result.WRITE = TOP_CHANNEL - 12;
            break;
    }

    return result;
}
