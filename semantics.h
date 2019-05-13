#ifndef TESTTREE_H
#define TESTTREE_H

#define MAXSIZE 1000
char *stack[MAXSIZE];
int globalLocalSize[MAXSIZE]; 
int localStackIndex;
int stackIndex;
extern int isIf;

void semantics(struct node *root,int level, FILE *f);
void pop();
void push(char *item);
void show();

int find(char *item, int tempSize,int tempTop);
int verify(char *item);
#endif
