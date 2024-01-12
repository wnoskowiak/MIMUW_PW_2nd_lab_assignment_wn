//
// Created by wnoskowiak on 09.01.24.
//

#ifndef MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_MESSAGEBUFFERHANDLER_H
#define MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_MESSAGEBUFFERHANDLER_H

struct byId;

typedef struct byId byIdT;

struct byCount;

typedef struct byCount byCountT;

struct byTag;

typedef struct byTag byTagT;

struct byId {
    byCountT * parent;
    int id;
    int packetsArrived;
    int packetCount;
    void *data;
    byIdT * next;
    byIdT * previous;
};

struct byCount {
    byTagT * parent;
    int count;
    byCountT * next;
    byCountT * previous;
    byIdT * elem;
};

struct byTag {
    int parent;
    int tag;
    byTagT * next;
    byTagT * previous;
    byCountT * further;
};


byTagT** initializeMemory();

bool isComplete(byIdT* node);

byIdT* findNode(byTagT** memory, int source, int tag, int count);

byIdT* getNode(byTagT** memory, int source, int tag, int count, int id, int i, void* data);

void deleteNode(byTagT** memory, byIdT* node);

void clearMemory(byTagT** memory);

#endif //MIMUW_PW_2ND_LAB_ASSIGNMENT_WN_MESSAGEBUFFERHANDLER_H
