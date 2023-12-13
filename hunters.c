#include "defs.h"

/*
	Initialize a hunter, with initial values of 0 for fear and boredom, 
	and a given device
	Params:
		in:	 char*               name[MAX_STR]: The name of the hunter
		out: HunterType* 		 hunter: The hunter to be initialized
		in:	 int                 evidenceType: The device the hunter will use
*/
void initHunter(char* name, HunterType* hunter, int evidenceType, EvidenceListType* sharedEvidences) {
	hunter->evidenceDevice = evidenceType;
	hunter->currRoom = NULL;
	hunter->sharedEvidences = NULL;
	hunter->fear = 0;
	hunter->boredom = 0;
	hunter->sharedEvidences = sharedEvidences;
	l_hunterInit(name, evidenceType);
}

/*
	Initialize a hunter array on the heap
	Params:
		out:	 list:  an arraylist of hunters
*/
void initHunterArray(HunterArrayType* hunters) {
	hunters->elements = (HunterType*)calloc(NUM_HUNTERS, sizeof(HunterType));
	hunters->size = 0;
}

/*
	Cleanup a hunter array
	Params:
		in:	 list:  an arraylist of hunters
*/
void cleanupHunterArray(HunterArrayType* hunters) {
	free(hunters->elements);
}

/*
	place hunters in the first room (the van)
	Params:
		in:	 house:  the house with the hunters
*/
void placeHunters(HouseType* house){
	for(int i = 0; i < NUM_HUNTERS; i++){
		// update hunters current room
		house->hunters.elements[i].currRoom = house->rooms.head->data;
		// add hunter to the first rooms list
		moveHunter(&house->hunters.elements[i], house->rooms.head->data);
	}
}

/**
 * Main function for the hunter behaviour, loops forever while the hunter
 * explores the house and finds evidence, then exits when bored or afraid.
 * This function should run in its own thread, called from main.
 * Each hunter runs on its own thread.
 * Params:
 *     in/out:	 void* 	hunterPointer: a void pointer to a hunter, ready to go
 * Returns:
 *     NULL, when finished
*/
void* hunterThreadFunc(void *hunterPointer){
	HunterType* hunter = (HunterType*)hunterPointer;
	// loop this thread forever, until pthread_exit is called
	while (1) {
		// delay for a small period to demonstrate threads interfering
		usleep(HUNTER_WAIT);
		// gain fear while in a room with a ghost, or gain boredom otherwise
		if (hunter->currRoom->ghost != NULL) { // FIXME: jump depends on uninitialized value
			hunter->fear++;
			hunter->boredom = 0;
		}
		else {
			hunter->boredom++;
		}
		// 3.3 exit thread if afriad
		if(hunter->fear == FEAR_MAX){
			removeHunterFromRoom(hunter);
			l_hunterExit(hunter->name, LOG_FEAR);
			pthread_exit(NULL);
		} else if (hunter->boredom == BOREDOM_MAX) {
			// 3.4 exit thread if bored
			removeHunterFromRoom(hunter);
			l_hunterExit(hunter->name, LOG_BORED);
			pthread_exit(NULL);
		}
		//3.2 Randomly choose to either collect evidence, move, or review evidence.
		int choice = randInt(0,3);
		if (choice == 0){
			// 3.2.1 search for evidence in the room
			checkEvidence(hunter);
		} else if (choice == 1) {
			// 3.2.2 Move to another room
			// pick a random room
			RoomNodeType* currRoomNode = NULL;
			currRoomNode = hunter->currRoom->linkedRooms.head;
			int roomIndex = randInt(0, hunter->currRoom->linkedRooms.size);
			// traverse to that random room
			for (int i = 0; i < roomIndex; i++) {
				currRoomNode = currRoomNode->next;
			}
			// move into that room
			moveHunter(hunter, currRoomNode->data);
			l_hunterMove(hunter->name, hunter->currRoom->name);
		} else if (choice == 2) {
			//3.2.3 Review evidence
			reviewEvidence(hunter);
		}
	}
}

/*
	Remove an evidence from a list of evidences
	Params:
		in:	 node:  the node to be removed
		in:	 list:  the list to remove the node from
*/
void removeEvidenceFromList(EvidenceNodeType* node, EvidenceListType* list){
	EvidenceNodeType* currNode = list->head;
	EvidenceNodeType* prevNode = NULL;
	// find the node
	while (currNode != NULL){
		if(currNode == node){
			// we found the node to remove
			if (prevNode == NULL) {
				// remove the head
				list->head = currNode->next;
			} else if (currNode->next == NULL){
				// remove the tail
				prevNode->next = NULL;
				list->tail = prevNode;
			}
			else {
				// remove the node, point around it
				prevNode->next = currNode->next;
			}
			// free the node
			free(currNode);
			return;
		} 
		prevNode = currNode;
		currNode = currNode->next;
	}
}

/**
 * Move a hunter from one room to another
 * Params:
 *     in/out:	 hunter:  the hunter to move
 *     in/out:	 des:  the room to move the hunter to
*/
void moveHunter(HunterType* hunter, RoomType* des){
	removeHunterFromRoom(hunter); // remove the hunter from their room BEFORE moving
	sem_wait(&des->mutex); // lock the destination room
	hunter->currRoom = des; // move to next room and update it
	des->hunters[des->numHunters] = hunter;
	des->numHunters++;
	sem_post(&des->mutex); // unlock dest room
}

/**
 * Remove a hunter from their current room
 * Params:
 *     in/out:	 hunter:  the hunter who wants to leave
*/
void removeHunterFromRoom(HunterType* hunter) {
	RoomType *currRoom = hunter->currRoom;
	if (currRoom == NULL) return;
	sem_wait(&currRoom->mutex); // lock room before modifying
	for (int i = 0; i < currRoom->numHunters; i++) {
		if (currRoom->hunters[i] == hunter) { // compare pointers, not names
			// shift elements to the left to fill the gap
			for (int j = i; j < currRoom->numHunters-1; j++){
				currRoom->hunters[j] = currRoom->hunters[j+1];
			}
			// clear last element
			currRoom->hunters[currRoom->numHunters-1] = NULL;
			// decrement list size
			currRoom->numHunters--;
			break;
		}
	}
	sem_post(&currRoom->mutex); // unlock for other threads
}

/**
 * Search the current room for any evidence this hunter can collect.
 * If one is found, add it to the hunter's shared list of evidence, and remove it from the room.
 * Params:
 *     in/out:	 hunter:  the hunter who wants to collect evidence
*/
void checkEvidence(HunterType* hunter){
	// lock the room so that the list cant be modified while checking it
	sem_wait(&hunter->currRoom->mutex);
	// traverse the list for an evidence that this hunter can collect
	EvidenceNodeType* currEvidenceNode = hunter->currRoom->evidences.head;
	while (currEvidenceNode != NULL) {
		if (currEvidenceNode->type == hunter->evidenceDevice) {
			// when we find a matching evidence type, add it to the shared evidence list
			sem_wait(&hunter->sharedEvidences->mutex);
			addEvidenceToList(currEvidenceNode->type, hunter->sharedEvidences);
			sem_post(&hunter->sharedEvidences->mutex);
			// free the node
			removeEvidenceFromList(currEvidenceNode, &hunter->currRoom->evidences);
			l_hunterCollect(hunter->name, hunter->evidenceDevice, hunter->currRoom->name);	
			break; // we found one, done!	
		} 
		currEvidenceNode = currEvidenceNode->next;
	}
	sem_post(&hunter->currRoom->mutex); // unlock the room whether or not we find it
}

/**
 * Check if the hunters have collected enough evidence.
 * If a hunter realizes they have enough, they exit the house.
 * Params:
 *     in/out:	 hunter:  the hunter who wants to check
*/
void reviewEvidence(HunterType* hunter){
	// sum the evidence
	int evidenceCount = 0;
	sem_wait(&hunter->sharedEvidences->mutex); // lock list while we read it
	// traverse the list
	EvidenceNodeType* currEvidenceNode = hunter->sharedEvidences->head;
	while (currEvidenceNode != NULL){
		evidenceCount++;
		currEvidenceNode = currEvidenceNode->next;
	}
	sem_post(&hunter->sharedEvidences->mutex);

	// break if there is not enough in the set
	if (evidenceCount < 3) {
		l_hunterReview(hunter->name, LOG_INSUFFICIENT);
		return;
	}
	// hunter realizes there is enough evidence
	// remove hunter from the room
	removeHunterFromRoom(hunter);
	// log exit and stop this thread
	l_hunterReview(hunter->name, LOG_SUFFICIENT);
	l_hunterExit(hunter->name, LOG_EVIDENCE);
	pthread_exit(NULL);
}