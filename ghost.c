#include "defs.h"

/*
	Initialize a ghost with initial values of 0 for boredom.
	Params:
		out: GhostType* 	ghost: The ghost to be initialized
*/
void initGhost(GhostType* ghost) {
	ghost->ghostClass = randInt(0,3);
	ghost->currRoom = NULL;
	ghost->boredom = 0;
}

/*
	Place the ghost in a random room
	Params:
		in/out:	 HouseType* 	house: The house the ghost is in
		in/out:	 GhostType* 	ghost: The ghost to be placed
*/
void placeGhost(HouseType* house, GhostType* ghost){
	initGhost(ghost); // init the ghost

	// traverse to a random room (but at least once, so it isnt the van)
	RoomNodeType *currNode = NULL;
	currNode = house->rooms.head;
	int room = randInt(1, 12); // pick which room we want
	for (int i = 0; i <= room; i++) {
		currNode = currNode->next; // traverse to the room
	}

	// put the ghost in the room
	ghost->currRoom = currNode->data;
	currNode->data->ghost = ghost;
	l_ghostInit(ghost->ghostClass, ghost->currRoom->name);
}

/**
 * Main function for the ghost behaviour, loops forever while the ghost
 * explores the house and leaves evidence, then exits when bored.
 * This function should run in its own thread, called from main.
 * Params:
 *     in/out:	 void* 	ghostPointer: a void pointer to a ghost, ready to go
 * Returns:
 *     NULL, when finished
*/
void* ghostThreadFunc(void* ghostPointer){
	GhostType* ghost = (GhostType*)ghostPointer;
	// loop forever, exit the thread when done
	// 2.5 exit the thread, if at max boredom
	while (ghost->boredom < BOREDOM_MAX) {
		usleep(GHOST_WAIT); // delay for a small period, to allow the threads to interact more
		int choice;
		if (ghost->currRoom->numHunters > 0) {
			// 2.1 reset boredom and stop moving if in room with hunter
			ghost->boredom = 0;
			choice = randInt(1, 3); // dont pick 0 (move)
		} else {
			// 2.2 otherwise, increase boredom by one
			ghost->boredom++;
			choice = randInt(0, 3); // choose a random action
		}
		// 2.3 if moving to adjacent room, update the ghosts room and the rooms ghosts
		if (choice == 0) {
			// pick a room
			int index = randInt(0, ghost->currRoom->linkedRooms.size);
			RoomNodeType* currNode = ghost->currRoom->linkedRooms.head;
			// traverse to the randomly chosen room in the list
			for (int i = 0; i < index; i++) {
				if(currNode->next != NULL){
					currNode = currNode->next;
				}
			}
			RoomType* roomToMoveTo = currNode->data;
			// lock the room so the hunters wait for this operation to finish
			sem_wait(&ghost->currRoom->mutex);
			ghost->currRoom->ghost = NULL; // remove from current room
			sem_post(&ghost->currRoom->mutex);
			ghost->currRoom = roomToMoveTo;
			sem_wait(&roomToMoveTo->mutex); // lock the room while we modify it
			roomToMoveTo->ghost = ghost;
			sem_post(&roomToMoveTo->mutex);
			l_ghostMove(roomToMoveTo->name);
			
		}
		// 2.4 pick an evidence type from the ghosts list to leave
		else if (choice == 1) {
			// choose a type to leave
			EvidenceType evidence = getRandomEvidence(ghost->ghostClass);
			// lock the room before we modify it
			// in case a hunter is about to check the evidence
			sem_wait(&ghost->currRoom->mutex);
			// add evidence
			addEvidenceToList(evidence, &ghost->currRoom->evidences);
			// unlock list
			sem_post(&ghost->currRoom->mutex);
			l_ghostEvidence(evidence, ghost->currRoom->name);
		}
		// else do nothing
	}
	// remove self from room
	ghost->currRoom->ghost = NULL;
	// exit
	l_ghostExit(LOG_BORED);
	pthread_exit(NULL);
}

// note:
// enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
// enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
// poltergeist: 	EMF TEMP FINGER --SOUND--
// banshee:			EMF TEMP --FINGER-- SOUND
// bullies: 		EMF --TEMP-- FINGER SOUND
// phantom: 		--EMF-- TEMP FINGER SOUND
/**
 * get a random evidence type for a ghost
 * params:
 * 		in: GhostClass ghostclass: the type of ghost we have
 * returns: 
 * 		EvidenceType: a random type of evidence that the given ghost could leave
*/
EvidenceType getRandomEvidence(GhostClass ghostClass) {
	if (ghostClass > 3) return EV_UNKNOWN;
	// build an array of our possible types
	char possibleTypes[3];
	int i = 0;
	// deduce which types are possible
	if (ghostClass != PHANTOM) {
		possibleTypes[i++] = EMF; // everyone except phantom can leave EMF
	}
	if (ghostClass != BULLIES) {
		possibleTypes[i++] = TEMPERATURE; // etc for all other types
	}
	if (ghostClass != BANSHEE) {
		possibleTypes[i++] = FINGERPRINTS;
	}
	if (ghostClass != POLTERGEIST) {
		possibleTypes[i++] = SOUND;
	}
	// we now have a list similar to {EMF, FINGERPRINTS, SOUND}
	// pick a random element from our list of 3
	i = randInt(0, 3);
	return possibleTypes[i];
}