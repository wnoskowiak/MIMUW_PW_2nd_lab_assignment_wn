//
// Created by wnoskowiak on 11.01.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "channel.h"
#include "packetHandler.h"
#include "types.h"
#include "sendRecieveHandler.h"
#include "cascadeWakeupHandler.h"
#include "channelDefinitionService.h"
#include "envVariableHandlers.h"

#include "killExitListenerFunctions.h"


bool arrivedIds[16];
bool handlingFailure;
int arrivedCount;
int currentId;

pthread_mutex_t helperMutexUno = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t helperMutexDos = PTHREAD_MUTEX_INITIALIZER;

opT commonlyAccessed;

threadIdsT threadIds;

void resetMemory() {
    memset(&arrivedIds, false, 16);
    arrivedCount = 0;
}

void *ptpListenerFunction(void *arg) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    listenerArgsT *args = (listenerArgsT *) arg;
    char buffer[BUFFER_SIZE];
    int id;

    channelT channel = serverChannelInputChannel(args->source, BARRIER);

    while (true) {

        recievePacket(&(buffer[0]), channel);
        pthread_mutex_lock(&commonlyAccessed.accessMutex);
        decodeBarrierPacket(&id, buffer);

        commonlyAccessed.mType = SYNCH_INST_ENTER;
        commonlyAccessed.readChannel = args->source;
        commonlyAccessed.readId = id;
        commonlyAccessed.doneProcessing = false;
        commonlyAccessed.messageIn = true;

        pthread_cond_signal(&commonlyAccessed.processCondition);
        pthread_mutex_lock(&helperMutexUno);

        while (!commonlyAccessed.doneProcessing) {
            pthread_cond_wait(&commonlyAccessed.doneProcessingCondition, &helperMutexUno);
        }

        pthread_mutex_unlock(&helperMutexUno);
        commonlyAccessed.messageIn = false;
        pthread_mutex_unlock(&commonlyAccessed.accessMutex);

    }

}


int main() {


    int worldSize = readWorldSize();

    char buffer[BUFFER_SIZE];
    bool imAlive = true;

    killExitArgsT aargs = {BARRIER, &commonlyAccessed, &helperMutexUno};


    if (pthread_create(&threadIds.killDetectionId, NULL, killingListenerFunction, &aargs) != 0) {
        printf("Failed to create kill detection thread\n");
//        TODO: dopisać failowanie
        return 1;
    }

    if (pthread_create(&threadIds.exitDetectionId, NULL, exitListenerFunction, &aargs) != 0) {
        printf("Failed to create exit detection thread\n");
//        TODO: dopisać failowanie
        return 1;
    }

    for (int i = 0; i < worldSize; i++) {

        listenerArgsT args = {i};

        if (pthread_create(&(threadIds.monitorThreadsIds[i]), NULL, ptpListenerFunction, &args) != 0) {
            printf("Failed to create message handler detection thread\n");
//        TODO: dopisać failowanie
            return 1;
        }
    }

//    TODO: handle initialization errors

    pthread_mutex_init(&helperMutexUno, NULL);
    pthread_mutex_init(&helperMutexDos, NULL);
    pthread_mutex_init(&commonlyAccessed.accessMutex, NULL);
    pthread_cond_init(&commonlyAccessed.processCondition, NULL);
    pthread_cond_init(&commonlyAccessed.doneProcessingCondition, NULL);

    channelT channel;

    while (imAlive) {

        pthread_mutex_lock(&helperMutexDos);

        while (!commonlyAccessed.messageIn) {
            pthread_cond_wait(&commonlyAccessed.processCondition, &helperMutexDos);
        }

        pthread_mutex_unlock(&helperMutexDos);

        switch (commonlyAccessed.mType) {

            case MIMPI_BLOCK_EXIT:

                handlingFailure = true;
                constructWakePacket(false, &arrivedIds[0], &buffer);
                channel = serverChannelOutputChannel(getFirst(arrivedIds), BARRIER);
                sendPacket(&buffer[0], channel);
                resetMemory();
                break;

            case SYNCH_INST_ENTER:

                if (arrivedCount == 0) {
                    currentId = commonlyAccessed.readId;
                }

                if (commonlyAccessed.readId < currentId || handlingFailure) {
                    constructFailPacket(&buffer);
                    channel = serverChannelOutputChannel(commonlyAccessed.readChannel, BARRIER);
                    sendPacket(&buffer[0], channel);
                    break;
                }

                arrivedIds[commonlyAccessed.readChannel] = true;
                arrivedCount++;

                if (arrivedCount == worldSize) {
                    constructWakePacket(true, &arrivedIds[0], &buffer);
                    channel = serverChannelOutputChannel(0, BARRIER);
                    sendPacket(&buffer[0], channel);
                    resetMemory();
                }

                break;

            case KILL_SIGNAL:

                imAlive = false;
                break;

        }

        commonlyAccessed.doneProcessing = true;
        pthread_cond_signal(&commonlyAccessed.doneProcessingCondition);

    }

    if (pthread_cancel(threadIds.killDetectionId) != 0) {
        printf("Failed to cancel thread\n");
        return 1;
    }

    if (pthread_cancel(threadIds.exitDetectionId) != 0) {
        printf("Failed to cancel thread\n");
        return 1;
    }

    for (int i = 0; i < worldSize; i++) {
        if (pthread_cancel(threadIds.monitorThreadsIds[i]) != 0) {
            printf("Failed to cancel thread\n");
            return 1;
        }
    }


    if (pthread_join(threadIds.killDetectionId, NULL) != 0) {
        printf("Failed to cancel thread\n");
        return 1;
    }

    if (pthread_join(threadIds.exitDetectionId, NULL) != 0) {
        printf("Failed to cancel thread\n");
        return 1;
    }

    for (int i = 0; i < worldSize; i++) {
        if (pthread_join(threadIds.monitorThreadsIds[i], NULL) != 0) {
            printf("Failed to cancel thread\n");
            return 1;
        }
    }

    pthread_mutex_destroy(&helperMutexUno);
    pthread_mutex_destroy(&helperMutexDos);
    pthread_mutex_destroy(&commonlyAccessed.accessMutex);
    pthread_cond_destroy(&commonlyAccessed.processCondition);
    pthread_cond_destroy(&commonlyAccessed.doneProcessingCondition);

    return EXIT_SUCCESS;
}


//void *killingListenerFunction(void *arg) {
//
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    char buffer[BUFFER_SIZE];
//
//    channelT channel = serverKillChannel(BARRIER);
//
//    recievePacket(&(buffer[0]), channel);
//    pthread_mutex_lock(&commonlyAccessed.accessMutex);
//
//    commonlyAccessed.mType = KILL_SIGNAL;
//    commonlyAccessed.doneProcessing = false;
//    commonlyAccessed.messageIn = true;
//
//    pthread_cond_signal(&commonlyAccessed.processCondition);
//    pthread_mutex_lock(&helperMutexUno);
//
//    while (!commonlyAccessed.doneProcessing) {
//        pthread_cond_wait(&commonlyAccessed.doneProcessingCondition, &helperMutexUno);
//    }
//
//    pthread_mutex_unlock(&helperMutexUno);
//    commonlyAccessed.messageIn = false;
//    pthread_mutex_unlock(&commonlyAccessed.accessMutex);
//
//}

//void *exitListenerFunction(void *arg) {
//
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    char buffer[BUFFER_SIZE];
//
//    channelT channel = exitToServerChannel(BARRIER);
//
//    recievePacket(&(buffer[0]), channel);
//    pthread_mutex_lock(&commonlyAccessed.accessMutex);
//
//    commonlyAccessed.mType = MIMPI_BLOCK_EXIT;
//    commonlyAccessed.doneProcessing = false;
//    commonlyAccessed.messageIn = true;
//
//    pthread_cond_signal(&commonlyAccessed.processCondition);
//    pthread_mutex_lock(&helperMutexUno);
//
//    while (!commonlyAccessed.doneProcessing) {
//        pthread_cond_wait(&commonlyAccessed.doneProcessingCondition, &helperMutexUno);
//    }
//
//    pthread_mutex_unlock(&helperMutexUno);
//    commonlyAccessed.messageIn = false;
//
//}

