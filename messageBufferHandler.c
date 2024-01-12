struct byId;

typedef struct byId byIdT;

struct byId {
    int id;
    int packetsArrived;
    int packetCount;
    void *data;
    byIdT * next;
};

struct byCount;

typedef struct byCount byCountT;

struct byCount {
    int count;
    byCountT * next;
    byIdT * elem;
};

struct byTag;

typedef struct byTag byTagT;

struct byTag {
    int tag;
    byTagT * next;
    byCountT * further;
};

byIdT getNode(byTagT* memory[], int source, int tag, int count, int id) {
    if(memory[source] == NULL) {

    }
}

