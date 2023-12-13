#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        6 // decreased to favor the ghost, as it would not win often
#define LOGGING         C_TRUE
#define MAX_ARR         128
#define RS              "\033[m" // reset
#define BS              "\033[1m" // bold
#define IS              "\033[3m" // italic
#define ULS             "\033[4m" // underline
#define CCS             "\033[36m" // cyan
#define GCS             "\033[92m" // green
#define RCS             "\033[91m" // red
#define MCS             "\033[95m" // magenta
#define LOG_FILE        "log.txt"

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

typedef struct Room RoomType;
typedef struct RoomList RoomListType;
typedef struct Hunter HunterType;
typedef struct Ghost GhostType;
typedef struct RoomNode RoomNodeType;
typedef struct House HouseType;
typedef struct EvidenceList EvidenceListType;
typedef struct EvidenceNode EvidenceNodeType;
typedef struct HunterArray HunterArrayType;
typedef struct HunterThreadParams HunterThreadParamsType;

// Autocomplete assisted by Codeium and Github Copilot, from Nov. 10th to Dec. 3rd, 2023.
struct EvidenceList {
    EvidenceNodeType    *head;
    EvidenceNodeType    *tail;
    sem_t               mutex;
}; 
struct HunterThreadParams {
    
    HouseType* house;
    int hunterIndex;

};
struct RoomList {
    RoomNodeType *head;
    RoomNodeType *tail;
    int size; // list needs a size, so we can pick a random one
};

struct HunterArray {
    HunterType* elements;// array of hunter pointers
    int size;
};

struct Room {
    char name[MAX_STR];
    RoomListType         linkedRooms;
    EvidenceListType     evidences;
    HunterType     *hunters[NUM_HUNTERS];
    int                numHunters;
    GhostType           *ghost;
    sem_t                mutex;
};

struct RoomNode{
    RoomType*       data;
    RoomNodeType*   next;
};

struct House {
    HunterArrayType      hunters;
    RoomListType         rooms;
    EvidenceListType     sharedEvidences;
};

struct EvidenceNode {
    EvidenceType        type;
    EvidenceNodeType    *next;
};

struct Ghost {
    enum GhostClass  ghostClass;
    RoomType*   currRoom;
    int         boredom;

};

struct Hunter {
    RoomType*           currRoom;
    EvidenceType        evidenceDevice;
    char                name[MAX_STR];
    EvidenceListType*   sharedEvidences;
    int                 fear;
    int                 boredom;
    int                 ghostFound;
};

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

// initializers
void initHouse(HouseType*);
void initRoom(char*, RoomType*);
void initRoomList(RoomListType*);
void initEvidenceList(EvidenceListType*);
void initGhost(GhostType*);
void initHunter(char*, HunterType*, int, EvidenceListType*);
void initHunterArray(HunterArrayType*);
void initSimulation(HouseType*, GhostType*);

// main.c
void runSimulation(HouseType*, GhostType*);
void finishSimulation(HouseType*, GhostType*);
void printEvidence(EvidenceListType*); 
void createHunters(HunterArrayType*, HouseType*);
int determineGhostClass(EvidenceListType*);
void printHuntersStatus(HunterArrayType*);

//house.c
void populateRooms(HouseType*);
void cleanupHouse(HouseType*);

// rooms
RoomType* createRoom(char*);
void addRoomToList(RoomType*, RoomListType*);
void connectRooms(RoomType*, RoomType*);
void cleanupRoom(RoomType*);
void cleanupRoomList(RoomListType*);

// hunters.c
void cleanupHunterArray(HunterArrayType*);
void placeHunters(HouseType*);
void* hunterThreadFunc(void*);
void removeEvidenceFromList(EvidenceNodeType*, EvidenceListType*);
void moveHunter(HunterType*, RoomType*);
void removeHunterFromRoom(HunterType*);
void checkEvidence(HunterType*);
void reviewEvidence(HunterType*);


// ghosts.c
void placeGhost(HouseType*, GhostType*);
void* ghostThreadFunc(void*); // accepts a GhostType pointer
EvidenceType getRandomEvidence(GhostClass ghostClass);

//evidence.c
void addEvidenceToList(EvidenceType, EvidenceListType*);
void cleanupEvidenceList(EvidenceListType*);