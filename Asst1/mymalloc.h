#ifndef _MYMALLOC_H
#define _MYMALLOC_H


typedef struct _metadataNode{

	short int next;
	short int prv;
	short int size;
	short int buffer;

}metadataNode;


#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)


static char myblock[4096];

void* mymalloc(size_t size, char* filename, int line);
void myfree(void* ptr, char* filename, int line);


#endif
