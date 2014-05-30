/*
 * buildGraph.h
 *
 *  Created on: Oct 11, 2013
 *      Author: yihong
 */

#ifndef BUILDGRAPH_H_
#define BUILDGRAPH_H_

typedef struct vertexT {
	char* name;
	int Istop ;
	int status ;
	struct edgeT *edges ;
	char ** commandline ;
} vertexT;





typedef struct linkedList {
	vertexT * next ;
	struct linkedList * nextNode ;
}linkedList;

typedef struct edgeT {
	struct vertexT *connectsTo;
	struct edgeT *next;
} edgeT;

int setAschild (vertexT* );
int recurCheck (vertexT * );
vertexT* lookupGivePos (char *  , int  , linkedList *  );
int PostOrderTraverseHelp ( vertexT* );
void addInLinkedList (vertexT *  , linkedList*  , int , int  );
vertexT *  createNode (char * , int , linkedList *  , int , int  );
int handleAndCompile (char * ,linkedList * );
int compileInGcc (vertexT*  );
int buildUpdateGraph ( char * , char **  ,int , char ** ,int , int  , linkedList *  , int );


#endif /* BUILDGRAPH_H_ */
