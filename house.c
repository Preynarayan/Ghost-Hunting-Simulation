#include "defs.h"

/*
    Initializes the given house.
        out: house, the house to be initialized
*/
void initHouse(HouseType* house) {
    // create a new array to store the hunters
	initHunterArray(&house->hunters);
    // init the list of rooms
    initRoomList(&house->rooms);
    // init the list of evidence
    initEvidenceList(&house->sharedEvidences);
} 

/*
    Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
        out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    RoomType* van                = createRoom("Van");
    RoomType* hallway            = createRoom("Hallway");
    RoomType* master_bedroom     = createRoom("Master Bedroom");
    RoomType* boys_bedroom       = createRoom("Boy's Bedroom");
    RoomType* bathroom           = createRoom("Bathroom");
    RoomType* basement           = createRoom("Basement");
    RoomType* basement_hallway   = createRoom("Basement Hallway");
    RoomType* right_storage_room = createRoom("Right Storage Room");
    RoomType* left_storage_room  = createRoom("Left Storage Room");
    RoomType* kitchen            = createRoom("Kitchen");
    RoomType* living_room        = createRoom("Living Room");
    RoomType* garage             = createRoom("Garage");
    RoomType* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoomToList(van, &house->rooms);
    addRoomToList(hallway, &house->rooms);
    addRoomToList(master_bedroom, &house->rooms);
    addRoomToList(boys_bedroom, &house->rooms);
    addRoomToList(bathroom, &house->rooms);
    addRoomToList(basement, &house->rooms);
    addRoomToList(basement_hallway, &house->rooms);
    addRoomToList(right_storage_room, &house->rooms);
    addRoomToList(left_storage_room, &house->rooms);
    addRoomToList(kitchen, &house->rooms);
    addRoomToList(living_room, &house->rooms);
    addRoomToList(garage, &house->rooms);
    addRoomToList(utility_room, &house->rooms);
}

/*
	Free everything everywhere in the entire house
	Params:
		in:		HouseType* house		The house to be freed
*/
void cleanupHouse(HouseType* house) {
	// first, free every room
	RoomListType* roomList = &house->rooms;
	RoomNodeType* currNode = roomList->head;
	while (currNode != NULL) {
		cleanupRoom(currNode->data);
		currNode = currNode->next;
	}
	// when every room is free, we can free the master list of rooms
	cleanupRoomList(roomList);
	cleanupHunterArray(&house->hunters);
	cleanupEvidenceList(&house->sharedEvidences);
	free(house); // finally, free the house itself
}