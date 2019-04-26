#include <stdlib.h>
#include<string.h>
#include<stdio.h>
#include "mymalloc.h"


void* mymalloc(size_t size, char* filename, int line){


	if(size <= 0){
		//can't request no/negative space
		fprintf(stderr, "Error: in file %s on line %d: Cannot malloc for 0 or negative space\n", filename, line);
		return NULL;
	}

	//we implemented padding to make navigating our lists easier, while also reducing external fragmentation (at the cost of internal fragmentation)
	while(size % 8 != 0){
		++size;
	}


	metadataNode node;

	if(myblock[0] != 123){ //we use the first byte of the array for our magic number, used to check initialization
		if((size + sizeof(node) + sizeof(node) + 1) > 4096){
			//the first insertion must leave enough space for the user's space and two metadata nodes, including the header
			fprintf(stderr, "Error: in file %s on line %d: Too much space requested\n", filename, line);
			return NULL;
		}
		//clears random memory, which makes our life easier for free
		int i = 0;
		for(i = 0; i <= 4096; ++i){
			myblock[i] = 0;
		}
		//header node
		node.size = 4096 - sizeof(node) - 1;
		node.next = -1;
		node.prv = -1;
		node.buffer = sizeof(node) + 1;
		memcpy(&myblock[1], &node, sizeof(node));

		//the metadata to keep track of what the user inserted
		metadataNode node1;
		node1.size = size;
		node1.next = -1;
		node1.prv = -1;	
		node1.buffer = sizeof(node) + sizeof(node1) + 1;

		node.size = node.size - size - sizeof(node1);
		node.buffer += size + sizeof(node1);

		//inserts metadata into array
		memcpy(&myblock[sizeof(node1) + 1], &node1, sizeof(node1));
		memcpy(&myblock[1], &node, sizeof(node));

		//sets magic number to show initialization has been completed
		myblock[0] = 123;

		return (void*)&myblock[node1.buffer];

	}else{
		metadataNode * ptr = (metadataNode*)&myblock[1];
	
		//searches for where there is enough space for what the user requests + the metadata needed to keep track of it (first fit)
		while(ptr->size < (size + sizeof(metadataNode))){
			if(ptr->size < (size + sizeof(metadataNode)) && ptr->next == -1){
				fprintf(stderr, "Error: in file %s on line %d: Too much space requested\n", filename, line);
				return NULL;
			}
			ptr = (metadataNode*)&myblock[ptr->next];
		}

	
		node.next = -1;
		node.prv = -1;
		node.size = size;
		node.buffer = ptr->buffer + sizeof(node);

	
		memcpy(&myblock[ptr->buffer], &node, sizeof(node));
	

		ptr->size = ptr->size - size - sizeof(node);
		ptr->buffer += size + sizeof(node);
	

		return (void*)&myblock[node.buffer];	

	}

	
}

void myfree(void* ptr, char* filename, int line){

	//freeing a null pointer/nonpointer casted as a pointer
	if(ptr == NULL || ptr == 0){
		fprintf(stderr, "Error: in file %s on line %d: Cannot free null pointer\n", filename, line);
		return;
	}


	metadataNode * tempPtr = (metadataNode*)(ptr - sizeof(metadataNode));

	//user tries to free memory outside of our array
	if(tempPtr > (metadataNode*)&myblock[4096] || tempPtr < (metadataNode*)&myblock[0]){
		fprintf(stderr, "Error: in file %s on line %d: Invalid pointer\n", filename, line);
		return;

	}

	//user tried to free something that is not a metadata node, or tried to free the same thing multiple times
	if(tempPtr->next != -1 || tempPtr->prv != -1){
		fprintf(stderr, "Error: in file %s on line %d: This memory cannot be freed\n", filename, line);
		return;
	}

	metadataNode * traversalPointer = (metadataNode*)&myblock[1];
	metadataNode* previous;

	int flag = 0;

	//finds where in the free list to insert; sets flag to denote the type of insertion
	while(traversalPointer->next != -1 && traversalPointer->buffer < tempPtr->buffer){

		previous = traversalPointer;
		traversalPointer = (metadataNode*)&myblock[traversalPointer->next];

		if(traversalPointer->next == -1){
			flag = 1;
		}else if(traversalPointer->buffer > tempPtr->buffer){
			flag = 2;
		}
	}

	int temp = 0;

	if(flag == 0){ //insert directly after header
		previous = traversalPointer;
		temp = 0;	
		temp = previous->size;
		previous->size = tempPtr->size;
		tempPtr->size = temp;

		temp = previous->buffer;	
		previous->buffer = tempPtr->buffer;
		tempPtr->buffer = temp;

		if(traversalPointer->next != -1){
			tempPtr->prv = previous->buffer - sizeof(tempPtr);
			tempPtr->next = previous->next;				
			previous->next = previous->buffer - sizeof(metadataNode);
			traversalPointer->prv = -1;
		}else{
			tempPtr->prv = 9;
			previous->next = previous->buffer - sizeof(metadataNode);

		}
	}else if(flag == 1){//insert at the end of the list

		previous = traversalPointer;
		temp = 0;	
		temp = previous->size;
		previous->size = tempPtr->size;
		tempPtr->size = temp;

		temp = previous->buffer;	
		previous->buffer = tempPtr->buffer;
		tempPtr->buffer = temp;

		previous->next = previous->buffer - sizeof(metadataNode);
		tempPtr->prv = previous->next;

	}else if(flag == 2){//insert in the middle of two nodes
		temp = 0;	
		temp = previous->size;
		previous->size = tempPtr->size;
		tempPtr->size = temp;

		temp = previous->buffer;	
		previous->buffer = tempPtr->buffer;
		tempPtr->buffer = temp;

		tempPtr->prv = previous->buffer - sizeof(tempPtr);
		tempPtr->next = previous->next;			

		previous->next = previous->buffer - sizeof(metadataNode);
		traversalPointer->prv = tempPtr->buffer - sizeof(metadataNode);
	}
			metadataNode * gptr = (metadataNode*)&myblock[1];
			metadataNode * tptr = NULL;

			while(gptr->next != -1){

				printf("free space left: %d starting at: %d; gptr->next: %d; gptr->prv: %d\n", gptr->size, gptr->buffer, gptr->next, gptr->prv);

				if(tptr != NULL){
					gptr->buffer = tptr->buffer + tptr->size + sizeof(metadataNode);
				}

				tptr = gptr;
				gptr = (metadataNode*)&myblock[gptr->next];

			}
			printf("free space left: %d starting at: %d; gptr->next: %d; gptr->prv: %d\n", gptr->size, gptr->buffer, gptr->next, gptr->prv);	

			printf("current: .prv %d .next %d .size %d .buffer %d\n", previous->prv, previous->next, previous->size, previous->buffer);

			if(previous->next - previous->prv - previous->size - sizeof(metadataNode) == 0 && previous->prv != -1){

				metadataNode * pptr = (metadataNode*)&myblock[myblock[previous->prv]];
				printf("next, somehow: .prv %d .next %d .size %d .buffer %d\n", pptr->prv, pptr->next, pptr->size, pptr->buffer);
				metadataNode * qptr = (metadataNode*)&myblock[previous->prv - previous->size - sizeof(metadataNode)];

				if(qptr->next == 0){
					qptr =  (metadataNode*)&myblock[1];
				}

				printf("previous :.prv %d .next %d .size %d .buffer %d\n", qptr->prv, qptr->next, qptr->size, qptr->buffer);


/*				qptr->size += previous->size + sizeof(metadataNode);
				pptr->buffer = qptr->buffer + qptr->size + sizeof(metadataNode);
				qptr->next = pptr->prv;
*/


				previous->size += pptr->size + sizeof(metadataNode);
				previous->next = pptr->next;


/*			
				temp = 0;	
				temp = previous->size;
				previous->size = pptr->size;
				pptr->size = temp;
		
				temp = previous->buffer;	
				previous->buffer = pptr->buffer;
				pptr->buffer = temp;

				temp = previous->next;	
				previous->next = pptr->next;
				pptr->next = temp;

				temp = previous->prv;	
				previous->prv = pptr->prv;
				pptr->prv = temp;
*/
	

				pptr->size = 0;
	
			}
	




		//if the one we're inserting 
/*
		if((previous->next - previous->prv) % 8 == 0 ){
			//coalesces free memory
			previous->size += tempPtr->size;
			previous->next = tempPtr->next;
			previous->prv = tempPtr->prv;

	}*/




	return;
}
