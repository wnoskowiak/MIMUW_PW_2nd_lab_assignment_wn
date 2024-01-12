#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "messageBufferHandler.h"
#include "packetHandler.h"


byTagT** initializeMemory() {
    return calloc(0,16* sizeof(byTagT));
}

bool isComplete(byIdT* node) {
    return  node->packetsArrived == node->packetCount;
}

byIdT* findNode(byTagT** memory, int source, int tag, int count) {
    byTagT* currentTag = memory[source];


    while(true) {

        if(currentTag == NULL) {
            return NULL;
        }

        if(currentTag->tag == tag) {
            break;
        }

        currentTag = currentTag->next;

    }

    byCountT * currentCount = currentTag->further;

    while(true) {

        if(currentCount == NULL) {
            return NULL;
        }

        if(currentCount->count == count) {
            break;
        }

        currentCount = currentCount->next;

    }

    byIdT * currentNode = currentCount->elem;

    while(true) {

        if(currentNode == NULL) {
            return NULL;
        }

        if(currentNode->packetCount == currentNode->packetsArrived) {
            return currentNode;
        }

        currentNode = currentNode->next;

    }

}

byIdT* getNode(byTagT** memory, int source, int tag, int count, int id, int i, void* data) {

    if(memory[source] == NULL) {
        memory[source] = calloc(0,sizeof(byTagT));
//      TODO : dodać fail
        memory[source]->parent = source;
        memory[source]->tag = tag;
    }

    byTagT* tempTag = memory[source];
    byTagT* lastTag;

    while(true) {

        if(tempTag == NULL) {
            tempTag = calloc(0,sizeof(byTagT));
//          TODO : dodać fail
            tempTag->parent = source;
            tempTag->tag = tag;
            tempTag->previous = lastTag;
            lastTag->next = tempTag;
            break;
        }

        if(tempTag->tag == tag) {
            break;
        }

        lastTag = tempTag;
        tempTag = tempTag->next;

    }

    if(tempTag->further == NULL) {
        tempTag->further = calloc(0,sizeof(byCountT));
//      TODO: dodać fail
        tempTag->further->count = count;
        tempTag->further->parent = tempTag;

    }

    byCountT * tempCount = tempTag->further;
    byCountT * lastCount;

    while(true) {

        if(tempCount == NULL) {
            tempCount = calloc(0,sizeof(byCountT));
//          TODO: dodać fail
            tempCount->parent = tempTag;
            tempCount->count = count;
            tempCount->previous = lastCount;
            lastCount->next = tempCount;
            break;
        }

        if(tempCount->count == count) {
            break;
        }

        lastCount = tempCount;
        tempCount = tempCount->next;

    }

    if(tempCount->elem == NULL) {
        tempCount->elem = calloc(0,sizeof(byIdT));
        tempCount->elem->packetCount = packetsNum(count);
        tempCount->elem->data = calloc(0, tempCount->elem->packetCount * BUFF_DATA_SIZE);
//      TODO: dodać fail
        tempCount->parent = tempTag;
        tempCount->elem->id = id;
    }

    byIdT * tempId = tempCount->elem;
    byIdT * lastId;

    while(true) {

        if(tempId == NULL) {
            tempId = calloc(0, sizeof(byIdT));
            tempId->packetCount = packetsNum(count);
            tempId->data = calloc(0, tempId->packetCount * BUFF_DATA_SIZE);
//          TODO: dodać fail
            tempId->id = id;
            tempId->previous = lastId;
            lastId->next = tempId;
            break;
        }

        if(tempId->id == id) {
            break;
        }

        lastId = tempId;
        tempId = tempId->next;

    }

    memcpy(tempId->data + i*BUFF_DATA_SIZE, data, BUFF_DATA_SIZE);
    tempId->packetCount++;

    return tempId;

}

void deleteNode(byTagT** memory, byIdT* node) {

    byIdT* previous = node->previous;
    byIdT* next = node->next;
    byCountT * nodeCount = node->parent;

    free(node->data);
    free(node);

    if(next != NULL) {
        next->previous = previous;
    }

    if(previous != NULL) {
        previous->next = next;
        return;
    }

    if(next != NULL) {
        nodeCount->elem = next;
        return;
    }

    byCountT * prevCount = nodeCount->previous;
    byCountT * nextCount = nodeCount->next;
    byTagT * nodeTag = nodeCount->parent;

    free(nodeCount);

    if(nextCount != NULL) {
        nextCount->previous = prevCount;
    }

    if(prevCount != NULL) {
        prevCount->next = nextCount;
        return;
    }

    if(nextCount != NULL) {
        nodeTag->further = nextCount;
        return;
    }

    byTagT * prevTag = nodeTag->previous;
    byTagT * nextTag = nodeTag->next;
    int parent = nodeTag->parent;

    free(nodeTag);

    if(nextTag != NULL) {
        nextTag->previous = nextTag;
    }

    if(prevTag != NULL) {
        prevTag->next = nextTag;
        return;
    }

    if(nextTag != NULL) {
        memory[parent] = nextTag;
        return;
    }

    memory[parent] = NULL;

}

void clearMemory(byTagT** memory) {

    for (int i = 0; i < 16; i++) {

        byTagT *current = memory[i];

        if(current != NULL) {
            byTagT * currentTag = current;
            byTagT * previousTag;
            while(currentTag != NULL) {

                byCountT * currentCount = currentTag->further;
                byCountT *previousCount;
                while(currentCount != NULL) {

                    byIdT * currentId = currentCount->elem;
                    byIdT * previousId;
                    while(currentId != NULL) {

                        free(currentId->data);
                        previousId = currentId;
                        currentId = currentId->next;
                        free(previousId);
                    }

                    previousCount = currentCount;
                    currentCount = currentCount->next;
                    free(previousCount);

                }
                previousTag = currentTag;
                currentTag = currentTag->next;
                free(previousTag);
            }

            memory[i] = NULL;

        }
    }

    free(memory);

}