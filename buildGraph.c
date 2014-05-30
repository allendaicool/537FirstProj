

/*****************************************************
 * FILE NAME: buildGraph.c
 *
 * Created on: Oct 14 2013
 * Author: Yihong Dai guangpu zhou
 * Professor : Barton P Miller
 * cs login: yihong guangpu
 * PURPOSE: this file contains the structure of the graph
 * , methods to change the graph, methods to post-traverse the graph,
 * methods to do cycle detection on the graph and methods to compile the
 * nodes of the graph. It can also create a linked list containing
 * all nodes of the graph.
 *
 *****************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "ProcessCreation.h"
#include <sys/types.h>
#include <sys/stat.h>


const int  UNVISITED = 0 ;
const int  IN_PROGRESS = 1;
const int  DONE = 2 ;



/* vertexT is the structure that reporesents vertexes in the graph */
typedef struct vertexT {
	/* name of the node */
	char* name;
	int status ;
	/* the leftmost edge of vertex */
	struct edgeT *edges ;
	/* array of commandlines */
	char ** commandline ;
	int  NumOfCommandLine;
} vertexT;




/* linkedList contains all vertexes in the graph */
typedef struct linkedList {
	/* current vertex in linkedList */
	vertexT * next ;
	/* next node in linkedList */
	struct linkedList * nextNode ;
}linkedList;

/* edgeT represents edges of a vertex */
typedef struct edgeT {
	/* the vertex being connected to*/
	struct vertexT *connectsTo;
	/* the vertex's next edge */
	struct edgeT *next;
} edgeT;



int recurCheck (vertexT * );
vertexT* lookupGivePos (char *  , int  , linkedList *  );
int PostOrderTraverseHelp ( vertexT* );
void addInLinkedList (vertexT *  , linkedList*  , int , int  );
vertexT *  createNode (char * , int , linkedList *  , int , int  );
int handleAndCompile (char * ,linkedList * );
int compileInGcc (vertexT*  );
int buildUpdateGraph ( char * , char **  ,int , char ** ,int , int  , linkedList *  , int );


/* This function finds the vertex to be compiled and does
 * cycle detection as well as post-order traversal for
 * the linkedlist head's vertex value.
 * return 1 in end
 * Parameters:
 *  runspec: name of the file to be searched for
 *  head: head of the linked list
 */
int handleAndCompile (char * runspec,linkedList * head)
{
	/* initialize vertex pointer */
	vertexT* temp ;

	if ( runspec != NULL)
	{
		/* checks whether there is a file with the given name */
		if ( (temp =  lookupGivePos(runspec,0,head)) == NULL )
		{
			/* exit if no file has the given name */
			printf(" no such node!!! \n");
			exit(EXIT_FAILURE);
		}

	}
	else
	{
		/* temp holds linkedlist head */
		temp = head->next;

	}
	/* checks whether there is any cycle */
	if (recurCheck (temp) == 1)
	{
		/* exit if cycle is detected */
		printf(" cycle detected !!! \n");
		exit(EXIT_FAILURE);
	}
	else
	{
		/* do post-order traversal for the linkedlist head's node */
		PostOrderTraverseHelp(temp);
	}

	return 1;
}




/* This function creates a graph based on target name and dependencies.
 * It also provides command lines for graph's nodes and puts newly created graph's nodes
 * into linked list.
 *
 * Parameters:
 *  name: target name
 *  dependency: list of dependencies
 *  Numcommand: number of dependencies
 *  head: head of linked list which holds all nodes
 *  offset: offset is uesed to keep track of the number of nodes in the graph
 *
 */
int buildUpdateGraph ( char * name, char ** dependency ,int arg, char ** command,int Numcommand, int NumcommandLine , linkedList * head , int offset)
{
	// offsetDep is set  know how many dependency is added in to graph
	int offsetDep = 0;
	/* temp is the target file's vertex*/
	vertexT * temp ;
	/* check whether any node with the name is already in the graph */
	if ((temp =lookupGivePos (name, NumcommandLine,head )) == NULL)
	{
		/* create new node and assign it to temp */
		temp = 	createNode(name,NumcommandLine,head,offset,0 );
		offsetDep ++ ;
	}

	/* use j to track position in the commandline */
	int j = 0 ;


	if(Numcommand > 0 )
	{
		/* allocate space for command line */
		temp->commandline = (char **)malloc(Numcommand*sizeof(char*));
		if ( temp->commandline == NULL)
		{
			/* exit if failed to allocate space for command line */
			perror("Error while doing malloc \n");
			exit(EXIT_FAILURE);
		}
		/* copy the command to commandline */
		while ( j < Numcommand)
		{
			/* allocate space for each string in the command line */
			(temp->commandline)[j] = (char*)malloc(strlen(command[j])+1);
			if ( (temp->commandline)[j] == NULL)
			{
				/* exit if failed to allocate space for the command line */
				perror("Error while doing malloc \n");
				exit(EXIT_FAILURE);
			}
			/* copy all elements of command to created vertex's commandline */
			strcpy(temp->commandline[j] ,command[j]);
			j++ ;
		}
	}

	if(arg> 0)
	{
		/* vertex for dependencies */
		vertexT * dep ;
		/* tracks the position in the dependency list */
		int i = 0;
		/* allocate space for created vertex's edges */
		temp->edges = (edgeT *)malloc(sizeof(edgeT));
		if ( temp->edges == NULL)
		{
			/* exit if failed to allocate spacefor edges  */
			perror("Error while doing malloc \n");
			exit(EXIT_FAILURE);
		}
		/* firstly tempEdge contains leftmost edge and then moves through all edges*/
		edgeT * tempEdge = temp->edges ;

		while (i< arg)
		{
			/* check whether the dependency's node is already in the graph */
			if( (dep=lookupGivePos(dependency[i],0,head)) == NULL)
			{
				/* create node for dependency */
				dep = createNode(dependency[i],0,head,offset,offsetDep);
				offsetDep ++ ;

			}
			/* let the edge connect to the dependency node */
			tempEdge->connectsTo =  dep;
			/* check whether more edges are needed for dependencies */
			if(i<(arg-1))
			{
				/* allocate space for the next dependency's node's edge*/
				edgeT * temp2 = (edgeT *)malloc(sizeof(edgeT));
				if ( temp2 == NULL)
				{
					/* exit if failed to allocate space for the next edge */
					perror("Error while doing malloc \n");
					exit(EXIT_FAILURE);
				}
				/* the next edge following the current edge */
				tempEdge->next = temp2 ;
				/* move to the next edge following the current edge */
				tempEdge = tempEdge->next;
			}
			/* update counter i*/
			i++;
		}
	}

	return 1;
}

/* This function adds new vertex as nodes to linked list.
 *
 * Parameters:
 *  T: the vertex to be added to linked list
 *  head: the head of the linked list
 *  offset:  how many nodes in the graph before calling buildupdateGraph
 *  offsetinDep:how many dependency has been added in the graph
 */
void addInLinkedList (vertexT * T , linkedList* head , int offset, int offsetinDep )
{
	/* allocate space for new linked list's node */
	linkedList  * newNodeInList = ( linkedList  *) malloc(sizeof(linkedList));
	if(newNodeInList == NULL)
	{
		/* exit if failed to allocate space for new node in linked list */
		perror("Error while doing malloc \n");
		exit(EXIT_FAILURE);
	}
	/* temp contains the head of linked list */
	linkedList* temp = head ;
	int count = 0 ;
	/* move to the last node in the linked list */
	while ( count < offset +offsetinDep)
	{
		temp = temp->nextNode ;
		count++;
	}
	/* last node in linked list has T as its value */
	temp->next = T ;
	/* let an empty node be linked list's last node */
	temp->nextNode = newNodeInList;

}

/* This function creates a new node for the graph based on
 * the given names, number of command lines, graph's linked list's head and
 * other information. Return the node if it is created successfully.
 *  exit if error
 * Parameters:
 *  name: name of the node to be created
 *  numOfCommandLIne: number of command lines
 *  head: head of the linked list
 *  offset:  how many nodes in the graph before calling buildupdateGraph
 *  offsetinDep:how many dependency has been added in the graph
 */
vertexT *  createNode (char * name, int numOfCommandLIne, linkedList * head , int offset, int offsetDep )
{
	/* allocate space for vertex to be created */
	vertexT * Node = (vertexT *) malloc(sizeof(vertexT));
	if(Node == NULL)
	{
		/* exit if no space has been allocated to Node */
		perror("Error while doing malloc \n");
		exit(EXIT_FAILURE);

	}
	/* allocate space for the node' name*/
	Node->name = (char *) malloc(strlen(name) +1 );
	if( Node->name == NULL)
	{
		/* exit if failed to allocate space for Node's value name */
		perror("Error while doing malloc \n");
		exit(EXIT_FAILURE);
	}
	/* copy the parameter name to Node's name */
	strcpy(Node->name,name);


	/* initilize the node to be unvisited */
	Node->status = UNVISITED ;
	/* initialize the node to contain zero edges  */
	Node->edges = NULL ;
	/* initialize the node to contain no command lines */
	Node->commandline = NULL;
	/* initilize the node's number of command lines to parameter numOfCommandLIne */
	Node->NumOfCommandLine = numOfCommandLIne ;
	/* add vertex created to the linked list */
	addInLinkedList (Node,   head , offset,offsetDep );

	return Node ;

}


/* This function searches the whole linked list to find the node with the TargetName
 * and then return a pointer to the node. It also assigns numOfCommandLine to the
 * node's vertex's NumOfCommandLine. if not found return null
 *
 * Parameters:
 *  TargetName: the name of the node to be searched for
 *  numOfCommandLine: the value to be assigned to the found node's NumOfCommandLine
 *  head: the head of the linked list
 */vertexT* lookupGivePos (char * TargetName , int numOfCommandLine , linkedList * head )
 {
	 /* the head vertex in the linked list */
	 linkedList * temp = head ;
	 /* check all vertexes until the vertex name is same as TargetName is found */
	 while (temp->next !=NULL)
	 {
		 /* return the node in the linked list if it is the file to be found */
		 if ( strcmp(temp->next->name,TargetName ) == 0)
		 {
			 if (numOfCommandLine > 0)
			 {
				 /* assign parameter numOfCommandLine to the NumOfCommandLine of the vertex just found */
				 temp->next->NumOfCommandLine = numOfCommandLine ;
			 }
			 /* return the vertex whose name is same as TargetName*/
			 return temp->next ;
		 }
		 /* move to the next node in the linked list */
		 temp = temp->nextNode ;
	 }

	 return NULL ;

 }



 /* This function does cycle detection for the graph
  * and return 1 if any cycle is found. Otherwise return 0.
  *
  * Parameters:
  *  T: the vertex to start the cycle detection
  */
 int recurCheck (vertexT * T   )
 {
	 /* change current vertex's status to IN_PROGRESS */
	 T->status = IN_PROGRESS;
	 /* temp2 holds current vertex's edge */
	 edgeT * temp2 =  T->edges ;

	 /* do recursive cycle detection for current vertex's chidren */
	 while(temp2 != NULL)
	 {

		 /* return 1 if any children of current vertex had been processed */
		 if ( temp2->connectsTo->status == IN_PROGRESS){
			 return 1 ;
		 }


		 else
		 {
			 /* do cycle detection starting from each of current vertex's children */
			 if (recurCheck(temp2->connectsTo) == 1)
			 {
				 /* return 1 if any cycle is found */
				 return 1 ;
			 }
		 }
		 /* move to the next edge of current vertex*/
		 temp2 = temp2->next;
	 }
	 /* set current vertex's status to DONE after all its children are processed */
	 T->status = DONE ;
	 /* return 0 if no cycle is found starting from this vertex and its children */
	 return 0 ;
 }



 /* This function does the post-order traversal starting from the given
  * vertex.
  *
  * Parameters:
  *  T: the vertex that the post-order traversal starts from
  */
 int PostOrderTraverseHelp ( vertexT* T )
 {
	 /* checks whether T has no children */
	 if(T->edges == NULL)
	 {
		 /* checks whether T has any command line */
		 if(T->commandline != NULL)
		 {
			 /* compile vertext T if it has command line */
			 compileInGcc (T );
		 }
		 /* return 1 if current vertex has no children */
		 return 1;
	 }
	 /* move to the next edge of the current vertex */
	 edgeT * temp = T->edges ;
	 /* do post-order traversal for all children of vertex T */
	 while(temp != NULL )
	 {

		 PostOrderTraverseHelp (temp->connectsTo);
		 /* move to the next edge */
		 temp = temp->next ;
	 }

	 /* compile the current vertex after all its children are processed */
	 compileInGcc (T );
	 return 1;
 }


 /* this function would compile the command in each node
  * that has command lines in it
  * input is the node
  * if output file exist and uptodate,
  * it doesn't compile command and return -1;otherwise return 1;
  *exit if error
  * Parameters:
  * vertexT* T : the node in the Graph
  */
 int compileInGcc(vertexT* T )
 {
	 // flag to run the command
	 int flag = 0 ;

	 edgeT* temp = T->edges;
	 // used to terminate while loop if necessary
	 int flagwhile = 0;
	 // if no dependecy, then run the command.
	 if ( temp == NULL  )
	 {

		 flag = 1;

	 }
	 else{
		 /* while loop used to check if there is a file out of date or doesnot exist
		  *  then run the command
		  */
		 while ((temp != NULL) && (flagwhile==0))
		 {
			 if ( checkIfitexist (T) == 0|| (giveModTime(temp->connectsTo->name) > giveModTime(T->name)))
			 {

				 flag = 1 ;
				 flagwhile = 1;
			 }
			 // iterate on all children on the vertex
			 temp = temp->next;
		 }
	 }
	 // if flag is one ,then run command
	 if( flag == 1)
	 {
		 int numOfCommandLine = T ->NumOfCommandLine ;
		 // i is used to keep track of the number of command line
		 int i = 0 ;
		 // j is used to keep track of the index of last word in new command line
		 int j =  0 ;
		 // quit is used to quit the inner while loop
		 int quit = 0 ;
		 // jPrev is used to keep track of the index of last word in old command line
		 int jPrev = 0 ;
		 // iterate over command lines
		 while ( i<numOfCommandLine )
		 {
			 // set jprev to index of  ending word in last command line
			 jPrev = j ;
			 quit = 0 ;
			 // update the number of command lines
			 i++;
			 while (quit == 0 )
			 {
				 /*if we encounter a space which I manually add in to separate each command line
				  * then we copy the whole command line except space
				  * and pass it to the runprocess method in the processCreation file
				  */
				 if ( strcmp((T->commandline)[j], " ") == 0)
				 {
					 char ** run = (char**) malloc((j -jPrev+1) *sizeof(char*));
					 if ( run == NULL)
					 {
						 perror("Error while doing malloc \n");
						 exit(EXIT_FAILURE);
					 }

					 // k is used to keep track of copying each string into new string list
					 int k  = jPrev ;
					 // l is initialized to j and together with k to copy the strings
					 int l =  j ;
					 while (k < j )
					 {
						 run[j-l] = malloc(strlen(T->commandline[k])+1);
						 if ( run[j-l] == NULL)
						 {
							 perror("Error while doing malloc \n");
							 exit(EXIT_FAILURE);
						 }
						 run[j-l] = strcpy(run[j-l], T->commandline[k]);
						 k++;
						 l--;
					 }

					 // index used to free up the memory
					 int arraySize = j-jPrev ;
					 int p = 0;
					 // set last word in the string list to be NULL
					 run[j-jPrev] = NULL;

					 // call the function in processCreation.c
					 RunChildProcess ( run );
					 // free memory
					 // used to free up memory
					 int freeV =  0 ;
					 while (freeV<arraySize)
					 {
						 free(run[freeV]);
						 freeV++;
					 }
					 free(run);
					 quit = 1;
				 }
				 j++ ;
			 }
		 }
		 return 1 ;
	 }

	 return -1;
 }

 /* this function returns the modification time of given file
  * exit if there is error in opening file or doing mallo
  * Parameters:
  *  target: the vertex to be found
  */
 int giveModTime( char *name)
 {
     // copy filename
	 char *filenNamePath = (char*) malloc(strlen(name)+1);

	 if(filenNamePath == NULL)
	 {
		 perror("Error while doing malloc \n");
		 exit(EXIT_FAILURE);
	 }
	 strcpy(filenNamePath,name);
	 struct stat buf ;
	 // give modification time
	 int a = stat(filenNamePath,&buf);
	 if (a == -1)
	 {
		 perror("Error while doing stat \n");
		 exit(EXIT_FAILURE);
	 }

	 int  b = (buf).st_mtime ;
	 free(filenNamePath);
	 return b ;

 }

 /* this function returns 1 the given target vertex
  * exists in the current directory. Otherwise return 0/
  *
  * Parameters:
  *  target: the vertex to be found
  */
 int checkIfitexist(   vertexT* target    )
 {


	 /* initialize directory pointer */
	 DIR * dir ;
	 /* assign the current directory's name to Dirname string */
	 char * Dirname = get_current_dir_name();
	 /* targetname holds the target vertex's name */
	 char * tagetname = target->name;
	 /* initilize directory entry */
	 struct dirent *entry;
	 /* open the current directory */
	 if ((dir = opendir(Dirname)) == NULL)
	 {
		 /* exit if problem in opening directory*/
		 perror("opendir() error");
		 exit(EXIT_FAILURE);
	 }
	 /* read the content of the current directory */
	 while ((entry = readdir(dir)) != NULL)

	 {
		 /* checks whether the current entry's name is the same as target vertex's name */
		 if (strcmp(entry->d_name, tagetname) == 0  )

		 {
			 return  1;
		 }
	 }

	 return 0 ;

 }



