#include "defs.h"

/* 
    Initializes a room
    Params:   
		in:  char* 				name: Name of the room
		out: RoomType* 			room: the destination room to initialize
*/
void initRoom(char* name, RoomType* room) {
    strcpy(room->name, name); // set the name
    initRoomList(&room->linkedRooms); // init the room list
    initEvidenceList(&room->evidences); // init the evidence list
	// init the hunter array
	for (int i = 0; i < NUM_HUNTERS; i++) {
		room->hunters[i] = NULL;
	}
	room->ghost = NULL;
	room->numHunters = 0;
	sem_init(&room->mutex, 0, 1); // must init mutex too
}

/*
    Dynamically allocates a new room and initializes
	params:
        in:         char* name  - the name of the room
    returns:    RoomType*   - a pointer to the room (on the heap)
*/
RoomType* createRoom(char* name) {
    RoomType* room = malloc(sizeof(RoomType));
    initRoom(name, room); // room starts with no ghost
    return room;
}

/*
	Initialize a room linked list
	Params:
		out: RoomListType* list: The list to be initialized
*/
void initRoomList(RoomListType* list) {
	list->head = list->tail = NULL;
	list->size = 0;
}

/*
	Initialize a node for a room
	Params:
		out: RoomNodeType* node: The node to be initialized
*/
void initRoomNode(RoomNodeType* node) {
	node->data = NULL; 
	node->next = NULL;
}

/*
	Create a new node for a room, and add it to the end of the linkedlist.
	Dynamically allocates nodes, must be cleaned up later
	Params:
		in:		RoomType*		The room to create a node around
		in/out:	RoomListType* 	The list to insert the node into
*/
void addRoomToList(RoomType* room, RoomListType* list) {
	// create a new node
	RoomNodeType* newNode = (RoomNodeType*)malloc(sizeof(RoomNodeType));
	initRoomNode(newNode); // initialize the node
	newNode->data = room; // put the room in the node
	// insert the node
	if (list->head == NULL) list->head = list->tail = newNode; // edge case: first node
	else {
		list->tail->next = newNode; // add after tail
		list->tail = newNode; // update tail pointer
	}
	list->size++;
}

/**
 * Connect two rooms together, by adding them to each other's linked list
 * Params:
 *     in:     RoomType* room1
 *     in:     RoomType* room2
*/
void connectRooms(RoomType* room1, RoomType* room2) {
    addRoomToList(room2, &room1->linkedRooms);
    addRoomToList(room1, &room2->linkedRooms);
}

/*
	Free a dynamically allocated room
	Params:
		in:		RoomType* room		The room to be freed
*/
void cleanupRoom(RoomType* room) {
	// free the contents of the room
	cleanupRoomList(&room->linkedRooms); // free the list, but not the rooms themselves
	cleanupEvidenceList(&room->evidences); // free all the evidence
	// free the room
	// note that this will leave other lists pointing to garbage data (this)
	// all rooms should be freed simultaneously to prevent issues
	free(room);
}

/*
	Free every node of a linked list of rooms
	Note: the data in each node is NOT freed, use cleanup house at the end
	Params:
		in:		RoomListType* list		The list to be freed
*/
void cleanupRoomList(RoomListType* list) {
	// traverse the list
	while (list->head != NULL) {
		// pop and free each node along the way
		RoomNodeType* temp = list->head;
		list->head = list->head->next;
		free(temp);
	}
}