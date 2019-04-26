#include<stdio.h>
#include<stdlib.h>
#include"mymalloc.h"

void workloadA();
void workloadB();
void workloadC();

int main(int argc, char** argv){


	workloadA();

	workloadB();

//	workloadC();

	printf("we did it!\n");
	return 0;
}


void workloadA(){

	int i = 0;
	void * test;
	for(i = 0; i < 150; ++i){

		test = malloc(9);
		free(test);

	}


	return;
}

void workloadB(){

	int i = 0;
	int j = 0;

	void * test[150];
	for(i = 0; i < 150; ++i){
		test[i] = malloc(1);
		if((i + 1) % 50 == 0){
			while(j < i){
				free(test[j]);
				++j;
			}
		}
		
	}

	return;
}

void workloadC(){

	int mallocCounter = 0;	
	int freeCounter = 0;

	int i = 0;	
}
