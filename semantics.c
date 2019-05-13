#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "codegen.h"

static int top = -1;
static int indexSize = 0;
int localStackIndex = 0;
int isIf = 0;

int isEOF = 0;
void semantics(struct node *root,int level, FILE *f) { 

  int isVar = 0;
  int isScan = 0;
  if (root != NULL) {
	
	if(strcmp(root->label,"vars")==0) {
		isVar = 1;
	}

	 if(strcmp(root->label,"block")==0) {
		isVar = 1;
		indexSize++;
	}
	
	if(strcmp(root->label,"R") == 0 || strcmp(root->label,"in") == 0 || strcmp(root->label,"assign") == 0) {
		isScan = 1;
	}

	if(strcmp(root->label,"if") == 0 || strcmp(root->label, "in") == 0 || strcmp(root->label, "loop") == 0 || strcmp(root->label,"assign") == 0 || strcmp(root->label,"out") == 0 ){
		
	}
	
        	int i;

        for(i=0;i<root->counts-1;i++) {	
		if(isVar == 1 && root->tk[i].tokenType == IDENTIFIER){
      			
			if(globalLocalSize[indexSize] == 0){
				push(root->tk[i].instance);
				fprintf(f,"PUSH\n");
				globalLocalSize[indexSize]++;
			} else {
				
				int verifyResult = verify(root->tk[i].instance);
				if (verifyResult == -1){
					push(root->tk[i].instance);
					fprintf(f,"PUSH\n");
					globalLocalSize[indexSize]++; 
				} else if(verifyResult == 1){
					if(indexSize == 0){
						sprintf(errorBuffer, "Variable Error in Line %d: Duplicate on Global Variable %s\n", root->tk[i].lineNum, root->tk[i].instance);
						strcpy(error[errorIndex], errorBuffer);
						errorIndex++;	
						isError = 3;
					} else {
						sprintf(errorBuffer, "Variable Error in Line %d: Duplicate on Local Variable %s\n", root->tk[i].lineNum, root->tk[i].instance);
						strcpy(error[errorIndex], errorBuffer);
						errorIndex++;	
						isError = 3;
					}
				}
			}
			
		} else if(isVar == 1 && root->tk[i].tokenType == NUMBER) {
				//fprintf(f,"LOAD %s\n", root->tk[i].instance);	
				//fprintf(f,"STACKW 0\n");
				//fprintf(f,"STACKR 0\n");

		}
			if(isScan == 1 && root->tk[i].tokenType == IDENTIFIER ){
			int tempTop = top;
			int tempSize = indexSize;
			int scanResult = find(root->tk[i].instance,tempSize,tempTop);
		
			if (scanResult >= 0){	
				localStackIndex = tempTop-scanResult;
				isScan = 0;		
			} else {
			
			sprintf(errorBuffer, "Variable Error in Line %d: Variable %s is not declared\n", root->tk[i].lineNum, root->tk[i].instance);
						strcpy(error[errorIndex], errorBuffer);
						errorIndex++;	
						isError = 3;
			}
		
		}

	}
		
	if (isIf ==2) {
		isIf=0;

	}



	semantics(root->left,level+1, f);	

	if(strcmp(root->label,"nstat")==0){
		isIf = 0;
	}
	
	if(strcmp(root->label,"assign")==0 && isIf == 0){
		codegen(root,f);
		isIf = 0;
	}


	if(strcmp(root->label,"in")==0 && isIf == 0){
		codegen(root,f);
		isIf = 0;
	}
		if(strcmp(root->label,"out")==0 && isIf == 0){
		codegen(root,f);
		isIf = 0;
	}
	

	if(strcmp(root->label,"if")==0 && isIf == 0){
		codegen(root,f);
		isIf = 1;
	}

	if(strcmp(root->label,"loop")==0 && isIf == 0){
		codegen(root,f);
		isIf = 1;
	}




	semantics(root->eq, level+1, f);	
	
	semantics(root->eq2, level+1, f);
       		if(strcmp(root->label, "block")==0){
	
		int tempSize = indexSize;
		int i;

		for (i = 0; i < globalLocalSize[tempSize]; i++) {
			pop();	
			fprintf(f,"POP\n");
		}
		indexSize--;
		
		if(localStackIndex > 0 ){
			localStackIndex--;
		}
	}

	semantics(root->right,level+1, f);




	if(strcmp(root->label,"program")==0){
		fprintf(f,"STOP\n");
		codegen(root,f);
	}
	

/*
	if(strcmp(root->label, "block")==0){
	
		int tempSize = indexSize;
		int i;

		for (i = 0; i < globalLocalSize[tempSize]; i++) {
			pop();	
			fprintf(f,"POP\n");
		}
		indexSize--;

	}
*/
   }
}


//verify the character if theres double
int verify(char *item){
	int i;	
	int tempSize = indexSize;
	int tempTop = top;

	//globalLocalSize[tempSize] showing the count 
	for (i = 0; i < globalLocalSize[tempSize]; i++) {
		if(strcmp(stack[tempTop], item) == 0) {	
			return 1;
		}
		tempTop--;
	} 
	return -1;
}

//check if variable is declared
int find(char *item, int tempSize, int tempTop){
	int i;	
	int isMatch = 0;

	for (i = 0; i < globalLocalSize[tempSize]; i++) {
		if(strcmp(stack[tempTop], item) == 0) {	
			isMatch = 1;
			return tempTop;
		}
		tempTop--;
	} 

	//if its is not match search the global
	if(tempSize > 0) {
		if(isMatch == 0) {
			--tempSize;
			int result = find(item, tempSize, tempTop);	
			if(result >= 0){
				return tempTop;
			} else {
				return -1;
			}
		}
	}

	return -1;
}

//pushing the identifier to the stack
void push(char *item) {
	if(top > 100) {
		
		sprintf(errorBuffer, "Insertint Identifier Failed: Too many Identifier \n"), 
		strcpy(error[errorIndex], errorBuffer);
		errorIndex++;	
		isError = 3;
	
	} else {	
		top++;
		stack[top] = item;
	}
}

//removing identifier fromt he stack
void pop() {
	if (top == -1) {		
		sprintf(errorBuffer, "Removing Identifier Failed: Stack is Empty \n"), 
		strcpy(error[errorIndex], errorBuffer);
		errorIndex++;	
		isError = 3;
	} else {
		char *item;
		item = stack[top];
		top--;
		
	}
}

void show() {
	printf("Static Semantic is Correct \n");

}

