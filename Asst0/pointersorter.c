#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<string.h>

typedef struct _node{
	
	char * str;
	struct _node* next;

}node;

int processInput(int, char**);
int nodeInsert(char*, node**);
void printLinkedList(node*);
void cleanup(node*);
int isSameCase(char, char);
int insertOrContinue(char, char);

int main(int argc, char** argv){

	processInput(argc, argv);

	return 0;
}



int processInput(int argc, char** argv){

	int currentArgument = 1; //used to iterate through each argument passed through command line separated by spaces. starts at 1 because arg 0 is the name of the program
	int currentCharacter = 0; //used to iterate through each individual argument by character
	int startingIndex = 0; //first index of substring to be added to LL node
	int endingIndex = -1; //last index of substring added to LL node
	int size = -1; //size of substring to be added to LL node; found by taking endingIndex - startingIndex
	int ctr = 0;
	char* parsedString = "error: could not parse string";
	node * head = NULL;	

	int insertionFlag = -1;

	if(argc != 2){
	
		printf("Error: %d inputs. Please enter just one string.\n", argc-1);
		return 1; 
	}

	//increments through each argument passed through the command line
	while(currentArgument < argc){ 

		startingIndex = endingIndex + 1;
		//iterates through each argument by character, until it hits a separator or the end
		while(isalpha(argv[currentArgument][currentCharacter])){	
			++endingIndex;
			++currentCharacter;
		}
		++endingIndex;

		//iterate through argv[currentArgument] size times, adding each char there to the array		
		if(isalpha(argv[currentArgument][currentCharacter-1])){

			size = endingIndex - startingIndex;
			if(size < 0){
				printf("fatal error; string of negative length\n");
				return 2;
			}
			parsedString = (char*)malloc(size*sizeof(char));

			ctr = currentCharacter-size;
			int i = 0;
			
			while(ctr < currentCharacter){
				parsedString[i] = argv[currentArgument][ctr];
				++ctr;
				++i;
			}
			
			insertionFlag = nodeInsert(parsedString, &head);
			if(insertionFlag != 1){
				printf("fatal error; insertion failed\n");
				return 2;
			}
		}
		if(isspace(argv[currentArgument][currentCharacter]) || isdigit(argv[currentArgument][currentCharacter]) || ispunct(argv[currentArgument][currentCharacter])){
			//the current character is a delimiter
			++currentCharacter;
		}else{
			//the end of the argument has been reached
			++currentArgument;
			currentCharacter = 0;
		}
	}
	if(head != NULL){
		printLinkedList(head);
		cleanup(head);
		free(parsedString);
	}

	return 0;
}

int nodeInsert(char * parsedString, node** head){

	node * newNode = (node*)malloc(sizeof(node));
	newNode->str = parsedString;
	newNode->next = NULL;

	node * ptr = (*head);		
	node * prv = NULL;

	if((*head) == NULL || strcmp((*head)->str, newNode->str) == 0){
		newNode->next =(*head);
		(*head) = newNode;
		return 1;
	}

	int length = strlen(newNode->str);
	int i = 0;
//special case to handle insertion before the head
	for(i = 0; i <= length; i++){	
		if(!isprint((*head)->str[i])){
			newNode->next =(*head);
			(*head) = newNode;
			return 1;
		}else if(!isprint(newNode->str[i])){
			break;
		}else if(newNode->str[i] < (*head)->str[i]){//lesser and same case = go to next node; lesser and diff case can insert
			if(isSameCase(newNode->str[i], ptr->str[i]) == 1){
				break;
			}else{
				newNode->next =(*head);
				(*head) = newNode;
				return 1;
			}
		}else if(newNode->str[i] > (*head)->str[i]){//greater and same case = insert. greater and diff case, go to next node
			if(isSameCase(newNode->str[i], ptr->str[i]) == 1){
				newNode->next =(*head);
				(*head) = newNode;
				return 1;
			}else{
				break;
			}
		}
	}

	while(ptr != NULL){
		if(strcmp(ptr->str, newNode->str) == 0){
			prv->next = newNode;
			newNode->next = ptr;
			return 1;
		}
		
		i = 0;
		while(i <= length){
			int flag = insertOrContinue(newNode->str[i], ptr->str[i]);
			if(flag == 0){
				prv = ptr;
				ptr = ptr->next;
				i = 0;
			}else if(flag == 1){
				prv->next = newNode;
				newNode->next = ptr;
				return 1;
			}else{
				++i;
			}
			if(ptr == NULL){
				break;
			}
		}
	}

	prv->next = newNode;

	return 1;
} 


int insertOrContinue(char newChar, char ptrChar){

	//checks to see whether to compare the next character, begin comparison to the next word, or to insert the node


	//the 'isprint' statements check for '\0'; iscntrl() and == '\0' weren't working, but this did
	if(!isprint(newChar)){
		return 0;
	}else if(!isprint(ptrChar)){
		return 1;
	}else if(newChar < ptrChar){//lesser and same case = go to next node; lesser and diff case can insert
		if(isSameCase(newChar, ptrChar) == 1){
			return 0;
		}else{
			return 1;
		}
	}else if(newChar > ptrChar){//greater and same case = insert. greater and diff case, go to next node
		if(isSameCase(newChar, ptrChar) == 1){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 2;
	}


}


int isSameCase(char a, char b){

	if(a == toupper(a) && b == toupper(b)){
		return 1;
	}else if(a == tolower(a) && b == tolower(b)){
		return 1;
	}else{
		return 0;
	}

}

void printLinkedList(node* head){

	node* ptr = head;
	while(ptr != NULL){
		printf("%s\n", ptr->str);
		ptr = ptr->next;
	}

}

void cleanup(node* head){

	node* ptr = head;
	node* prv = NULL;

	while(ptr != NULL){
		prv = ptr;
		ptr = ptr->next;
		free(prv);
	}
}
