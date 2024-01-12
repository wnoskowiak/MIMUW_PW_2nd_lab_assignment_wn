/**
 * This file is for implementation of MIMPI library.
 * */

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "channel.h"
#include "mimpi.h"
#include "mimpi_common.h"
#include "messageBufferHandler.h"
#include "packetHandler.h"



int worldSize;
int worldRank;

int messageId = 0;
int sendCount = 0;

bool entered = false;

pthread_mutex_t memoryMutex;
pthread_mutex_t helperMutex;
pthread_mutex_t helperMutex;
byTagT **memoryBuffer;

typedef struct listenerArgs {
    int channel;
    int source;
} listenerArgsT;

typedef struct waitingFor {
    bool waiting;
    int source;
    int tag;
    int count;
    byIdT *foundOne;
    pthread_cond_t *condition;

} waitingForT;

waitingForT recieveWaitingInfo;


void *ptpListenerFunction(void *arg) {

    // Enable cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    // Set cancellation type to asynchronous
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    listenerArgsT *args = (listenerArgsT *) arg;

    char buffer[BUFFER_SIZE];

    int id, tagRead, countRead, i;

    while (true) {

        if (chrecv((int) args->channel, &(buffer), BUFFER_SIZE) == -1) {
            perror("chrecv error");
            exit(EXIT_FAILURE);
//          TODO: dodać sprzątanie po failu
        }

        pthread_mutex_lock(&memoryMutex);


        void *rawData = decodePacket(&id, &tagRead, &countRead, &i, &buffer);

        byIdT *currentNode = getNode(memoryBuffer, args->source, tagRead, countRead, id, i, rawData);

        if (isComplete(currentNode)) {
            if (recieveWaitingInfo.waiting) {
                if (args->source == recieveWaitingInfo.source & tagRead == recieveWaitingInfo.tag &
                    countRead == recieveWaitingInfo.count) {
                    recieveWaitingInfo.foundOne = currentNode;
                    recieveWaitingInfo.waiting = false;
                    pthread_cond_signal(recieveWaitingInfo.condition);
                }
            }
        }

        pthread_mutex_unlock(&memoryMutex);

    }

}

void MIMPI_Init(bool enable_deadlock_detection) {
    entered = true;
    channels_init();

    worldSize = (int) result;
    env_value = (const char *) getenv("MIMPI_RANK");
    result = strtol(env_value, NULL, 10);
    worldRank = (int) result;

    pthread_mutex_init(&memoryMutex, NULL);
    pthread_mutex_init(&helperMutex, NULL);
    memoryBuffer = initializeMemory();


    if (pthread_cond_init(recieveWaitingInfo.condition, NULL) != 0) {
        fprintf(stderr, "Error initializing condition variable\n");
        exit(EXIT_FAILURE);
    }

    // TODO: dopisać failowanie


}

void MIMPI_Finalize() {
    entered = false;
    int status;

    for (int i = 0; i < sendCount; i++) {
        wait(&status);
    }

    channels_finalize();
}

int MIMPI_World_size() {
    return worldSize;
}

int MIMPI_World_rank() {
    return worldRank;
}

MIMPI_Retcode MIMPI_Send(
        void const *data,
        int count,
        int destination,
        int tag
) {

    if (!entered) {
        // TODO: I co teraz?
    }

    if (destination < worldSize) {
        return MIMPI_ERROR_NO_SUCH_RANK;
    }

    if (destination == worldRank) {
        return MIMPI_ERROR_ATTEMPTED_SELF_OP;
    }

    int me = worldRank;
    int channel = (16 * me + destination) + 20;
    char dataCopy[count];
    memcpy(&dataCopy, data, count);

    pid_t pid = fork();
    ASSERT_SYS_OK(pid);
    if (!pid) {
        //child
        char buffer[BUFFER_SIZE];
        int packetNum = packetsNum(count);
        for (int i = 0; i < packetNum; i++) {
            encodePacket(&messageId, &tag, &count, &i, &dataCopy, &buffer);
            if (chsend(channel, &buffer, BUFFER_SIZE) == -1) {
                perror("chsend error");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }

    messageId++;
    sendCount++;

    return MIMPI_SUCCESS;

}

MIMPI_Retcode MIMPI_Recv(
        void *data,
        int count,
        int source,
        int tag
) {

    if (!entered) {
        // TODO: I co teraz?
    }

    if (source < worldSize) {
        return MIMPI_ERROR_NO_SUCH_RANK;
    }

    if (source == worldRank) {
        return MIMPI_ERROR_ATTEMPTED_SELF_OP;
    }

    int channel = (16 * source + worldRank) + 20 + 1;

    pthread_mutex_lock(&memoryMutex);

    byIdT *foundNode = findNode(memoryBuffer, source, tag, count);

    if (foundNode == NULL) {

        recieveWaitingInfo.waiting = true;
        recieveWaitingInfo.source = source;
        recieveWaitingInfo.tag = tag;
        recieveWaitingInfo.count;
        pthread_mutex_unlock(&memoryMutex);
        pthread_mutex_lock(&helperMutex);
        while (recieveWaitingInfo.waiting == true) {
            pthread_cond_wait(recieveWaitingInfo.condition, &helperMutex);
        }
        pthread_mutex_unlock(&helperMutex);
        pthread_mutex_lock(&memoryMutex);
        foundNode = recieveWaitingInfo.foundOne;
    }

    memcpy(data, foundNode->data, count);
    deleteNode(memoryBuffer, foundNode);

    pthread_mutex_unlock(&memoryMutex);

    return MIMPI_SUCCESS;

}

MIMPI_Retcode MIMPI_Barrier() {
    TODO
}

MIMPI_Retcode MIMPI_Bcast(
        void *data,
        int count,
        int root
) {
    TODO
}

MIMPI_Retcode MIMPI_Reduce(
        void const *send_data,
        void *recv_data,
        int count,
        MIMPI_Op op,
        int root
) {
    TODO
}