#include "defs.h"

int main() {
    srand(time(NULL));

    // INITIALIZATION
    HouseType* house = (HouseType*)malloc(sizeof(HouseType));
    GhostType* ghost = (GhostType*)malloc(sizeof(GhostType));
    initSimulation(house, ghost); // intitialize everything in the simulation

    // THREADS
    runSimulation(house, ghost);

    // FINISH
    finishSimulation(house, ghost); // print results and clean up
}

/**
 * initialize all entities in the simulation
 * Params:
 *     in/out: HouseType* 	house: The house
 *     in/out: GhostType* 	ghost: The ghost
*/
void initSimulation(HouseType* house, GhostType* ghost) {
    // clear log file
    FILE* log = fopen(LOG_FILE, "w"); // just write nothing to it to clear it
    fclose(log);
    // create and populate a house
    initHouse(house);
    populateRooms(house);
    // get the names of our hunters from the user
    createHunters(&house->hunters, house);
    // place hunters in the van
    placeHunters(house);
    // Place and init the ghost in a random room
    placeGhost(house, ghost);
}

/**
 * Runs the simulation, with a thread for each character
 * Params:
 *     in/out: HouseType* 	house: The house
 *     in/out: GhostType* 	ghost: The ghost
*/
void runSimulation(HouseType* house, GhostType* ghost) {
    pthread_t ghostThread;
    pthread_t hunterThreads[NUM_HUNTERS];
    pthread_create(&ghostThread, NULL, ghostThreadFunc, ghost);
    // create a thread for each hunter
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_create(&hunterThreads[i], NULL, hunterThreadFunc, &house->hunters.elements[i]);
    }
    // wait for threads to exit
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_join(hunterThreads[i], NULL);
    }
    pthread_join(ghostThread, NULL);
}

/**
 * Prints the results of the simulation, cleans everything up
 * Params:
 *     in/out: HouseType* 	house: The house
 *     in/out: GhostType* 	ghost: The ghost
*/
void finishSimulation(HouseType* house, GhostType* ghost) {
    printf(MCS""BS""ULS"\nResults:\n"RS); // note: constants here represent ANSI escape codes
    // print results
    printHuntersStatus(&house->hunters);

    // print all evidence collected
    printf(BS""MCS""ULS"\nEvidence collected:\n"RS);
    printEvidence(&house->sharedEvidences);

    // identify the ghost if 3 pieces of evidence were collected
    // determine the ghost
    GhostClass ghostClass = determineGhostClass(&house->sharedEvidences);
    char ghostName[MAX_STR];
    ghostToString(ghostClass, ghostName);
    printf(CCS""BS"The hunters think the ghost class is: "IS""MCS"%s\n"RS, ghostName);

    // determine if the correct ghost was concluded
    char correctGhostName[MAX_STR];
    ghostToString(ghost->ghostClass, correctGhostName);
    printf(CCS""BS"The ghost class is actually: "IS""MCS"%s\n"RS, correctGhostName);

    char identifiedCorrectly = strcmp(ghostName, correctGhostName) == 0;
    if (identifiedCorrectly) {
        printf(GCS""BS"The ghost was correctly identified!\n"RS);
    } else {
        printf(RCS""BS"The ghost was incorrectly identified!\n"RS);
    }

    // free all dynamic memory
    cleanupHouse(house);
    free(ghost);
}

/**
 * Traverses and prints a linked list of evidence
 * Params:
 *    in:     EvidenceListType* list    the list to print
*/
void printEvidence(EvidenceListType* list) {
    // start at the head
    EvidenceNodeType* currNode = list->head;
    // break if its null
    if (currNode == NULL) {
        printf(RCS"No evidence collected\n"RS);
        return;
    }
    // traverse the list and print each node
    int i = 0;
    while (currNode != NULL) {
        char evidence[MAX_STR];
        evidenceToString(currNode->type, evidence);
        printf(CCS"%d. "IS"%s\n"RS, ++i, evidence);
        currNode = currNode->next;
    }
}

/*
    Takes an already initalized array of hunters and adds hunters with names the user wants
    Params:
        in:     HunterArrayType* hunters    the array to add to
*/
void createHunters(HunterArrayType* hunters, HouseType* house) {
    // prompt the user for names of hunters to create
    // then initialize them and add to the array
    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Please enter the name of hunter #%d: ", i+1);
        char* name = hunters->elements[i].name;
        fgets(name, MAX_STR, stdin);
        name[strlen(name)-1] = 0; // remove newline
        initHunter(name, &hunters->elements[i], i, &house->sharedEvidences);
        hunters->size++;
    }
}

/**
 * Determines the ghost class based on the evidence in the list
 * Params:
 *     in:     EvidenceListType* list    the list of evidence
 * returns:
 *     the ghost determined ghost class, as an int
*/
int determineGhostClass(EvidenceListType* evidence) {

    // traverse evidence list
    int evidenceSum = 0;
    int evidenceCount = 0;
	EvidenceNodeType* currEvidenceNode = evidence->head;
	while(currEvidenceNode != NULL){
		evidenceSum += currEvidenceNode->type; // running total
        evidenceCount++; // count how many there are
		currEvidenceNode = currEvidenceNode->next;
	}

    // we need 3 to conclude anything
    if (evidenceCount < 3) return GH_UNKNOWN;
    
    // return ghost class, or unknown if the sum isnt enough
	switch (evidenceSum) {
        case 6:
            return PHANTOM;
        case 5:
            return BULLIES;
        case 4:
            return BANSHEE;
        case 3:
            return POLTERGEIST;
        default:
            return GH_UNKNOWN;
	}
    // this works because the evidence is always unique,
    // and we know there are at least 3
}

/**
 * Count how many hunters are bored or afraid, determine the winner of the simulation
 * Params:
 *     in:     HunterArrayType* hunters    the array of hunters to check and print
*/
void printHuntersStatus(HunterArrayType* hunters) {
    int numBored = 0;
    int numFear = 0;
    for (int i = 0; i < NUM_HUNTERS; i++) {
        if (hunters->elements[i].fear >= FEAR_MAX) {
            printf(RCS"[AFRAID]: "CCS"[%s]\n"RS, hunters->elements[i].name);
            numFear++;           
        } else if (hunters->elements[i].boredom >= BOREDOM_MAX) {
            numBored++;
            printf(MCS"[BORED]: "CCS"[%s]\n"RS, hunters->elements[i].name);
        } else {
            printf(GCS"[OK]: "CCS"[%s]\n"RS, hunters->elements[i].name);
        }
    }
    if (numBored + numFear >= NUM_HUNTERS) {
        printf(IS""BS""RCS"Hunters have failed!\n"RS);
        printf(IS""BS""GCS"Ghost has won!\n"RS);
    } else {
        printf(IS""BS""GCS"Hunters have won!\n"RS);
        printf(IS""BS""RCS"Ghost has failed!\n"RS);
    }
}