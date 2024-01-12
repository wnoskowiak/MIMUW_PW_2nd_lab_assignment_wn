//
// Created by wnoskowiak on 11.01.24.
//

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_TYPES_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_TYPES_H

typedef enum  {

    MIMPI_BLOCK_EXIT = 1,
    SYNCH_INST_ENTER = 2,
    KILL_SIGNAL = 3,

} messageType;

typedef enum {

    READ = 1,
    WRITE = 2,

} pipeEnd;

typedef enum {

    EXIT = 1,
    BARRIER = 2,
    BCAST = 3,
    REDUCE = 4,

} server;

typedef struct {
    int READ;
    int WRITE;
} channelT;

typedef struct op {

    pthread_mutex_t accessMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t processCondition = PTHREAD_COND_INITIALIZER;
    pthread_cond_t doneProcessingCondition = PTHREAD_COND_INITIALIZER;

    bool doneProcessing;
    bool messageIn;

    messageType mType;
    int readChannel;
    int readId;

} opT;

typedef struct threadIds {

    pthread_t killDetectionId;
    pthread_t exitDetectionId;
    pthread_t monitorThreadsIds[16];

} threadIdsT;

typedef struct killExitArgs {
    server serv;
    opT * comms;
    pthread_mutex_t * helper;
} killExitArgsT;


typedef struct listenerArgs {
    int source;
} listenerArgsT;


#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_TYPES_H
