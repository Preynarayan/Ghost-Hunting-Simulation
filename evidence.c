#include "defs.h"

/*
    Initializes an evidence list
    params:
        in/out:     EvidenceListType*    The evidence list to initialize
*/
void initEvidenceList(EvidenceListType* list){
    list->head = list->tail = NULL;
	sem_init(&list->mutex, 0, 1);
}

/*
    Initializes an evidence node
    params:
        in/out:     EvidenceNodeType*    The evidence node to initialize
*/
void initEvidenceNode(EvidenceNodeType* node) {
    node->type = 5;
    node->next = NULL;
}

/*
	Create a new evidence, and add it to the end of the linkedlist.
	Dynamically allocates nodes, must be cleaned up later.
	Duplicates will not be added.
	Params:
		in:		EvidenceType		The evidence to create a node around
		in/out:	EvidenceListType* 	The list to insert the node into
*/
void addEvidenceToList(EvidenceType evidenceType, EvidenceListType* list) {
	// go to the end of the list
	EvidenceNodeType* newNode = list->head;
    while (newNode != NULL) {
        if (newNode->type == evidenceType) {
            // Evidence type already exists, so return without adding it
			// this makes it really, more of a set
            return;
        }
        newNode = newNode->next;
    }
	// create a new node and put it at the end
	newNode = (EvidenceNodeType*)malloc(sizeof(EvidenceNodeType));
	initEvidenceNode(newNode); // initialize data
	newNode->type = evidenceType;
	// insert the node
	if (list->head == NULL) list->head = list->tail = newNode; // edge case: first node
	else {
		list->tail->next = newNode; // add to the end
		list->tail = newNode; // update the tail
	}
}

/**
 * Free a dynamically allocated evidence linked list by traversing it
 * params:
 *     in:     EvidenceListType*    The evidence list to be freed
*/
void cleanupEvidenceList(EvidenceListType* list) {
	EvidenceNodeType* currNode = list->head;
	while (currNode != NULL) {
		EvidenceNodeType* temp = currNode;
		currNode = currNode->next;
		free(temp);
	}
}