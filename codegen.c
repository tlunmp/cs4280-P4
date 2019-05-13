#include <stdio.h>
#include <string.h>
#include "node.h"
#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "codegen.h"

static LabelCntr=0;                                     /* for unique labels */
static VarCntr=0;                                    /* for unique variables */
typedef enum {VAR, LABEL} nameType;
static char Name[20];                        /* for creation of unique names */
static char *newName(nameType what);
static int counter = 0;

static char *newName(nameType what)
{ if (what==VAR)
    sprintf(Name,"V%d",VarCntr++);    /* generate a new label as V0, V1, etc */
  else
    sprintf(Name,"L%d",LabelCntr++);            /* new lables as L0, L1, etc */
  return(Name);
}

void codegen(struct node *root, FILE *f){

char label[20], label2[20], argR[20];
	
	if(root == NULL)
	return;
		if(strcmp(root->label,"expr") == 0) {		
				if(strcmp(root->tk[0].instance, "NULL") == 0 ) {
						codegen(root->left, f);
				} else {
					codegen(root->eq,f);
					strcpy(argR,newName(VAR));
					fprintf(f,"STORE %s\n",argR);
					codegen(root->left,f);
					if (strcmp(root->tk[0].instance,"/") == 0){ 
			 	 		fprintf(f,"DIV %s\n",argR);
					} else if (strcmp(root->tk[0].instance,"*")==0){
			 	 		fprintf(f,"MULT %s\n",argR);
					}
				}
		} else if(strcmp(root->label,"A") == 0) {
			
				if(strcmp(root->tk[0].instance, "NULL") == 0 ) {
					codegen(root->left, f);
                        	} else {
					codegen(root->eq,f);
					strcpy(argR,newName(VAR));
					fprintf(f,"STORE %s\n",argR);
					codegen(root->left,f);
					if (strcmp(root->tk[0].instance,"+") == 0){ 
			 	 		fprintf(f,"ADD %s\n",argR);
					} else if (strcmp(root->tk[0].instance,"-")==0){
			 	 		fprintf(f,"SUB %s\n",argR);
					}
				
				}

		} else if(strcmp(root->label, "M") == 0) {
				
			if(strcmp(root->left->label, "R") ==0 ) {					
				codegen(root->left, f);	
			} else if(strcmp(root->left->label, "M") ==0 ) {					
				codegen(root->left, f);
				fprintf(f,"MULT -1\n");
			}

		} else if(strcmp(root->label, "R") == 0) {
			if(root->left != NULL) {
				codegen(root->left, f);
			} else if(root->tk[0].tokenType == IDENTIFIER) {
				//fprintf(f,"LOAD %s\n",root->tk[0].instance);
				fprintf(f,"STACKR %d\n",localStackIndex);
			} else if(root->tk[0].tokenType == NUMBER) {
				//fprintf(f,"LOAD %s\n",root->tk[0].instance);
				fprintf(f,"LOAD %s\n", root->tk[0].instance);
			}
		} else if(strcmp(root->label, "out") == 0) {
			codegen(root->left, f);
			strcpy(argR,newName(VAR));
			fprintf(f,"STORE %s\n",argR);
			fprintf(f,"WRITE %s\n",argR);
		} else if(strcmp(root->label,"in") == 0) {
			//fprintf(f,"READ %s\n",root->tk[0].instance);
			strcpy(argR,newName(VAR));
			fprintf(f,"READ %s\n",argR);
			fprintf(f,"LOAD %s\n",argR);
			fprintf(f,"STACKW %d\n",localStackIndex);
		} else if(strcmp(root->label,"assign") == 0) {	
			codegen(root->left, f);
			//fprintf(f,"STORE %s\n",root->tk[0].instance);
			
			fprintf(f,"STACKW %d\n",localStackIndex);	
		} else if(strcmp(root->label,"if") == 0) {
			codegen(root->eq2, f);
			strcpy(argR,newName(VAR));
			fprintf(f,"STORE %s\n",argR);
			codegen(root->left, f);
			fprintf(f,"SUB %s\n",argR);
			strcpy(argR,newName(LABEL));
			if(root->eq->tk[0].tokenType == EQUAL) {	
				if(root->eq->tk[1].tokenType == EQUAL) {
					fprintf(f,"BRZERO %s\n",argR);
				} else {
					fprintf(f,"BRPOS %s\n",argR);
					fprintf(f,"BRNEG %s\n",argR);
				}
			}else if(root->eq->tk[0].tokenType == LESS_THAN) {
				if(root->eq->tk[1].tokenType == EQUAL) {	
					fprintf(f,"BRPOS %s\n",argR);
				} else {
					fprintf(f,"BRZPOS %s\n",argR);
				}
			} else if(root->eq->tk[0].tokenType == GREATER_THAN) {
				if(root->eq->tk[1].tokenType == EQUAL) {
					fprintf(f,"BRNEG %s\n",argR);
				} else {
					fprintf(f,"BRZNEG %s\n",argR);
				}
			}
			
			codegen(root->right, f);
			
			fprintf(f,"%s: NOOP\n",argR);
		} else if (strcmp(root->label,"mstat")==0) {
				codegen(root->left, f);
				codegen(root->eq, f);
		} else if (strcmp(root->label,"stats")==0) {
			isIf = 0;
				codegen(root->left, f);
				codegen(root->eq, f);
		} else if (strcmp(root->label,"stat")==0) {
			
			if(root->left != NULL) {
				codegen(root->left, f);
			}else if(root->eq != NULL) {

				codegen(root->eq, f);
			}else if(root->eq2 != NULL) {

				codegen(root->eq2, f);
			}else if(root->right != NULL) {

				codegen(root->right, f);
			}
			isIf = 2;
		} else if(strcmp(root->label, "program") == 0) {
				
				if(counter == 0) {
					int i;
					for (i=0;i<VarCntr; i++)      
    					fprintf(f,"V%d 0\n",i);
					counter = 1;
				}
				
		} else if(strcmp(root->label, "loop") == 0) {
			fprintf(f,"\n");
			strcpy(label,newName(LABEL));
			fprintf(f,"%s: ",label);
			codegen(root->eq2, f);
			strcpy(argR,newName(VAR));
			fprintf(f,"STORE %s\n",argR);
			codegen(root->left, f);
			fprintf(f,"SUB %s\n",argR);
			strcpy(argR,newName(LABEL));
			if(root->eq->tk[0].tokenType == EQUAL) {	
				if(root->eq->tk[1].tokenType == EQUAL) {
					fprintf(f,"BRZERO %s\n",argR);
				} else {
					fprintf(f,"BRPOS %s\n",argR);
					fprintf(f,"BRNEG %s\n",argR);
				}
			}else if(root->eq->tk[0].tokenType == LESS_THAN) {
				if(root->eq->tk[1].tokenType == EQUAL) {	
					fprintf(f,"BRPOS %s\n",argR);
				} else {
					fprintf(f,"BRZPOS %s\n",argR);
				}
			} else if(root->eq->tk[0].tokenType == GREATER_THAN) {
				if(root->eq->tk[1].tokenType == EQUAL) {
					fprintf(f,"BRNEG %s\n",argR);
				} else {
					fprintf(f,"BRZNEG %s\n",argR);
				}
			}
			
			codegen(root->right, f);
			fprintf(f,"BR %s\n",label);
			fprintf(f,"%s: NOOP\n",argR);
		} else {

			codegen(root->left, f);
			codegen(root->eq, f);
			codegen(root->eq2, f);
			codegen(root->right, f);
	


		}

		
}
