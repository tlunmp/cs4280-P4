#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"

int i = 0;
int isStop = 0;


struct node* parser(){
	struct node* root; 
	root = program();	
	if(tokens[i].tokenType == EOT) {
	} else {
		sprintf(errorBuffer, "Parser Error in Line %d: Parsing Error stop", tokens[i].lineNum);
		strcpy(error[errorIndex], errorBuffer);
		errorIndex++;	
		isError = 2;
	}
	return root;
}

//void <vars> <block>

struct node* program(){
	struct node *p = newNode("program");
	if(strcmp(tokens[i].instance, "void") == 0){
		i++;
		p->left = vars();
		p->right = block();
		return p;
	} else {
		errorParser();
	}


	return p;
}

struct node* vars() {	
	//var Identifier : Integer <vars>
	struct node *t = newNode("vars");
	if(strcmp(tokens[i].instance, "var") == 0){
		i++;
		if(tokens[i].tokenType == IDENTIFIER) {		
			strcpy(t->tk[0].instance, tokens[i].instance);
			t->tk[0].tokenType = tokens[i].tokenType;
			t->tk[0].lineNum = tokens[i].lineNum;
			t->counts++;
			i++;
				
			if(strcmp(tokens[i].instance, ":") == 0){
				i++;
				
				if(tokens[i].tokenType == NUMBER) {			
					strcpy(t->tk[1].instance,tokens[i].instance);	 					
					t->tk[1].tokenType = tokens[i].tokenType;
					t->tk[1].lineNum = tokens[i].lineNum;
					t->counts++;
					i++;
					t->left = vars();
					return t;
				} else {
					errorParser();
				}
			} else {
			
				errorParser();
			}

		} else {
			
			errorParser();
		}
	//if Empty
	} else {
		return NULL;
	}

	return NULL;
}

//start <vars> <stats> stop
struct node* block() {

	struct node* block = newNode("block"); 
	if (strcmp(tokens[i].instance, "start") == 0) {
		i++;
		block->left = vars();
		block->eq = stats();
		if(strcmp(tokens[i].instance, "stop") == 0){	
			i++;
			return block;	
		} else {	
			
			errorParser();	
		}
	} else {
		errorParser();
	}
	
	return block;	
}


//<stat>  <mStat>
struct node* stats(){
	struct node* stats = newNode("stats");
	stats->left = stat();
	stats->eq = mStat();
	return stats;
}


// <in> | <out> | <block> | <if> | <loop> | <assign>
struct node* stat() {
	struct node* stat = newNode("stat");
	if(strcmp(tokens[i].instance,"scan") == 0){
		stat->left = in();
		return stat;
	} else if(strcmp(tokens[i].instance,"out") == 0){
		stat->left = out();
		return stat;
	} else if(strcmp(tokens[i].instance,"start") == 0){
		stat->right = block();
		return stat;
	} else if(strcmp(tokens[i].instance, "if") == 0) {
		stat->left = if1();
		return stat;
	} else if(strcmp(tokens[i].instance, "let") == 0) {
		stat->left = assign();
		return stat;
	} else if(strcmp(tokens[i].instance, "loop") == 0) {
		stat->left = loop();
		return stat;
	} else {

		errorParser();
	}


	return stat;

}

//empty |  <stat>  <mStat>
struct node* mStat(){
	struct node* mStat1 = newNode("mStat");
	if(strcmp(tokens[i].instance,"if") == 0 || strcmp(tokens[i].instance,"loop") == 0 ||strcmp(tokens[i].instance, "let") == 0 || strcmp(tokens[i].instance, "scan") ==0 || strcmp(tokens[i].instance, "start")==0 || strcmp(tokens[i].instance, "out") == 0){
		mStat1->left = stat();
		mStat1->eq = mStat();
		return mStat1;
	} else {
		return NULL;

	}
}

// scan  Identifier  . 
struct node* in(){
		struct node* in = newNode("in");
	if(strcmp(tokens[i].instance,"scan") == 0){
		i++;
		if(tokens[i].tokenType == IDENTIFIER){
			strcpy(in->tk[0].instance, tokens[i].instance);
			in->tk[0].tokenType = tokens[i].tokenType;
			in->tk[0].lineNum = tokens[i].lineNum;
			in->counts++;
			i++;
			if(strcmp(tokens[i].instance, ".")==0){
				i++;



				return in;
			} else {
		
				errorParser();
			}

		} else {	
			errorParser();
		}
	} else {
		errorParser();
	}
	return in;
}

//out [ <expr>  ] .
struct node* out(){
		struct node* out = newNode("out");
	if(strcmp(tokens[i].instance,"out") == 0){
		i++;
		if(strcmp(tokens[i].instance,"[") == 0){
			i++;
			out->left = expr();
			if(strcmp(tokens[i].instance,"]") == 0){
				i++;
				if(strcmp(tokens[i].instance,".") == 0){
					i++;
					return out;
				} else {
					errorParser();
				
				}			
			} else {
				errorParser();
			}	
		
		} else {
			errorParser();
		}

	} else {
		errorParser();
	}

	return out;
}

// if(<expr><RO><expr>) <stat>
struct node* if1(){
	struct node* if2 = newNode("if");
	if(strcmp(tokens[i].instance, "if")==0){
		i++;
		if(strcmp(tokens[i].instance, "(")==0){
			i++;
			if2->left = expr();
			if2->eq = ro();
			if2->eq2 = expr();
			if(strcmp(tokens[i].instance, ")")==0){
				i++;
				if2->right = stat();		
				return if2;
			} else {
				errorParser();
			}
		
		} else {
			errorParser();
		}
	} else {
		errorParser();
	}


	return if2;
}

// >|>=|<|<=|=|==
struct node* ro(){
	struct node* ro1 = newNode("RO");
	
	if(strcmp(tokens[i].instance, "<" ) == 0){
		strcpy(ro1->tk[0].instance,tokens[i].instance);	
		ro1->tk[0].tokenType = tokens[i].tokenType;
        	ro1->tk[0].lineNum = tokens[i].lineNum;
		ro1->counts++;
		i++;
		if(strcmp(tokens[i].instance, "=") == 0){
			strcpy(ro1->tk[1].instance,tokens[i].instance);	
			ro1->tk[1].tokenType = tokens[i].tokenType;
            		ro1->tk[1].lineNum = tokens[i].lineNum;
			ro1->counts++;
			i++;
			return ro1;
		} else {
			return ro1;
		}

	} else if(strcmp(tokens[i].instance, ">" ) == 0){
		strcpy(ro1->tk[0].instance,tokens[i].instance);	
		ro1->tk[0].tokenType = tokens[i].tokenType;
        	ro1->tk[0].lineNum = tokens[i].lineNum;
        	ro1->counts++;
		i++;
		if(strcmp(tokens[i].instance, "=") == 0){
			strcpy(ro1->tk[1].instance,tokens[i].instance);	
			ro1->tk[1].tokenType = tokens[i].tokenType;
            		ro1->tk[1].lineNum = tokens[i].lineNum;
            		ro1->counts++;
			i++;
			return ro1;
		} else {
			return ro1;
		}
		return ro1;
	
	} else if(strcmp(tokens[i].instance, "=" ) == 0){
		strcpy(ro1->tk[0].instance,tokens[i].instance);
		ro1->tk[0].tokenType = tokens[i].tokenType;
 	       	ro1->tk[0].lineNum = tokens[i].lineNum;
        	ro1->counts++;
		i++;
		if(strcmp(tokens[i].instance, "=") == 0){
			strcpy(ro1->tk[1].instance,tokens[i].instance);	
			ro1->tk[1].tokenType = tokens[i].tokenType;
           		ro1->tk[1].lineNum = tokens[i].lineNum;
			ro1->counts++;
			i++;
			return ro1;
		} else {
			return ro1;
		}
		return ro1;
	} else {
		errorParser();
	}
	return ro1;
}

// loop( <expr> <RO> <expr> ) <stat>
struct node* loop(){
	struct node* loop1 = newNode("loop");
	if(strcmp(tokens[i].instance, "loop") == 0){
		i++;
		if(strcmp(tokens[i].instance, "(") == 0) {
			i++;
			loop1->left = expr();
			loop1->eq = ro();
			loop1->eq2 = expr();
			if(strcmp(tokens[i].instance, ")") == 0) {
				i++;
				loop1->right = stat();	
				return loop1;
		
			} else {
				errorParser();
			}
		} else {
			errorParser();
		}
	} else {	
		errorParser();
	}
		
	return loop1;
}

// let Identifier = <exp> .
struct node* assign(){
	struct node* assign = newNode("assign");
	if(strcmp(tokens[i].instance, "let")==0){
		i++;
		if (tokens[i].tokenType == IDENTIFIER) {
			strcpy(assign->tk[0].instance,tokens[i].instance);	
			assign->tk[0].tokenType = tokens[i].tokenType;
            		assign->tk[0].lineNum = tokens[i].lineNum;
            		assign->counts++;
			i++;
			
			if(strcmp(tokens[i].instance, "=")== 0){
				strcpy(assign->tk[1].instance,tokens[i].instance);	
				assign->tk[1].tokenType = tokens[i].tokenType;
               			assign->tk[1].lineNum = tokens[i].lineNum;
                		assign->counts++;
				i++;
				assign->left = expr();
				if(strcmp(tokens[i].instance, ".")== 0){
					i++;
					return assign;
				} else {
					errorParser();

				}
			} else {
				errorParser();
			}
		} else {
			errorParser();	
		}

	} else {
		errorParser();
	}
	return assign;
}

//<A> / <expr> | <A> * <expr> | <A>
struct node* expr(){
	struct node* exp = newNode("expr");
	exp->left = a();
	if(strcmp(tokens[i].instance, "/")==0|| strcmp(tokens[i].instance,"*")==0){
		strcpy(exp->tk[0].instance,tokens[i].instance);	 					
		exp->tk[0].tokenType = tokens[i].tokenType;
        	exp->tk[0].lineNum = tokens[i].lineNum;
        	exp->counts++;
		i++;
		exp->eq = expr();
		return exp;
	} else {
		return exp;
	}
}


//<M> + <A> | <M> - <A> | <M>
struct node* a(){
	struct node* a1 = newNode("A");
	a1->left = m();
	if(strcmp(tokens[i].instance, "+") ==0 || strcmp(tokens[i].instance,"-")==0){
		strcpy(a1->tk[0].instance,tokens[i].instance);	 					
		a1->tk[0].tokenType = tokens[i].tokenType;
        	a1->tk[0].lineNum = tokens[i].lineNum;
        	a1->counts++;
		i++;
		a1->eq = a();
		return a1;
	} else {
		return a1;
	}
	return a1;
}

//-<M> | <R>
struct node* m(){
	struct node* m1 = newNode("M");
	if (strcmp(tokens[i].instance, "-") == 0)
	{
		strcpy(m1->tk[0].instance,tokens[i].instance);	 					
		m1->tk[0].tokenType = tokens[i].tokenType;
        	m1->tk[0].lineNum = tokens[i].lineNum;
        	m1->counts++;
		i++;
		m1->left = m();
		return m1;
		
	} else if(strcmp(tokens[i].instance, "(") == 0 || tokens[i].tokenType == IDENTIFIER || tokens[i].tokenType == NUMBER){
		m1->left = r();	
		return m1;
	}


	return m1;
}

//(exp) | identifier | integer
struct node* r(){
	struct node* r = newNode("R");
	if(strcmp(tokens[i].instance, "(") ==0){	
		i++;
		r->left = expr();
		if(strcmp(tokens[i].instance,")") == 0){
			i++;
			return r;
		} else {		
			errorParser();
		}
	} else if(tokens[i].tokenType == IDENTIFIER){
		strcpy(r->tk[0].instance,tokens[i].instance);	 					
		r->tk[0].tokenType = tokens[i].tokenType;
        	r->tk[0].lineNum = tokens[i].lineNum;
        	r->counts++;
		i++;
		return r; 

	} else if(tokens[i].tokenType == NUMBER){
		strcpy(r->tk[0].instance,tokens[i].instance);	 					
		r->tk[0].tokenType = tokens[i].tokenType;
        	r->tk[0].lineNum = tokens[i].lineNum;
       		r->counts++;
		i++;
		return r;
	} else {
		errorParser();	
	
	}
	return r;
}

//check if theres an error stop the sciript
void errorParser(){
		if(isError == 2) {
			if(strcmp(duplicate, tokens[i].instance) == 0 ){
			
			} else {
				sprintf(errorBuffer, "Parser Error in Line %d: token %s is invalid", tokens[i].lineNum, tokens[i].instance);
				strcpy(error[errorIndex], errorBuffer);
				errorIndex++;	
				isError = 2;
			}
			
		} else {
			sprintf(errorBuffer, "Parser Error in Line %d: token %s is invalid", tokens[i].lineNum, tokens[i].instance);
			strcpy(error[errorIndex], errorBuffer);
			errorIndex++;	
			isError = 2;
			strcpy(duplicate,tokens[i].instance);
		}
}


struct node *newNode(char *label) {
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	temp->counts=0;
	strcpy(temp->label,label);	
	strcpy(temp->tk->instance, "NULL");
	temp->counts++;
	temp->eq = temp->left = temp->eq2 = temp->right = NULL;
	return temp;
}

