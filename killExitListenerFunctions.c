//
// Created by wnoskowiak on 12.01.24.
//

#include <pthread.h>
#include <stdbool.h>
#include "channel.h"
#include "packetHandler.h"
#include "types.h"
#include "sendRecieveHandler.h"
#include "cascadeWakeupHandler.h"
#include "channelDefinitionService.h"

#include "killExitListenerFunctions.h"

void *killingListenerFunction(void *arg) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    killExitArgsT *args = (killExitArgsT *) arg;
    char buffer[BUFFER_SIZE];

    channelT channel = serverKillChannel(args->serv);

    recievePacket(&(buffer[0]), channel);
    pthread_mutex_lock(&(args->comms->accessMutex));

    args->comms->mType = KILL_SIGNAL;
    args->comms->doneProcessing = false;
    args->comms->messageIn = true;

    pthread_cond_signal(&(args->comms->processCondition));
    pthread_mutex_lock(args->helper);

    while (!args->comms->doneProcessing) {
        pthread_cond_wait(&(args->comms->doneProcessingCondition), args->helper);
    }

    pthread_mutex_unlock(args->helper);
    args->comms->messageIn = false;
    pthread_mutex_unlock(&args->comms->accessMutex);

}

void *exitListenerFunction(void *arg) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    killExitArgsT *args = (killExitArgsT *) arg;
    char buffer[BUFFER_SIZE];

    channelT channel = exitToServerChannel(args->serv);

    recievePacket(&(buffer[0]), channel);
    pthread_mutex_lock(&(args->comms->accessMutex));

    args->comms->mType = MIMPI_BLOCK_EXIT;
    args->comms->doneProcessing = false;
    args->comms->messageIn = true;

    pthread_cond_signal(&(args->comms->processCondition));
    pthread_mutex_lock(args->helper);

    while (!args->comms->doneProcessing) {
        pthread_cond_wait(&args->comms->doneProcessingCondition, args->helper);
    }

    pthread_mutex_unlock(args->helper);
    args->comms->messageIn = false;
    pthread_mutex_unlock(&args->comms->accessMutex);
}
