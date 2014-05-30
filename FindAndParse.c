/*/*****************************************************
 * FILE NAME: FindAndParse.c
 *
 * Created on: Oct 10 2013
 * Author: Yihong Dai guangpu zhou
 * Professor : Barton P Miller
 * cs login: yihong  guangpu
 * PURPOSE: this file is used to parse the makefile if there is one
 * and pass the specification detail into buildGraph file and build graph based
 * on the specification
 *
 *****************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include"buildGraph.h"

char * checkIfitisTarget(char * );
int checkIfCommand (char  );

/* main read the argument in the command line and read the name of target
 *main also parse the makefile if there is one and passed the parsed string
 *into builGraph file to build the graph.
 *  argc:  number of arguments
 *  *argv[]:  the array of arguments
 */
int main (int argc, char* argv[] )
{
	// declare the size of dependency list to be 20
	const int dependencysize = 20 ;
	// used as delimiter to read the makefile
	char *s = "\n \t"; // used to check empty line
	// set runSpec to name of arg if there is one
	char * runSpec = NULL ;
	if ( argc == 2)
	{

		runSpec = argv[1];
	}
	DIR * dir ;
	char * mk1 =  "makefile";
	char* mk2 = "Makefile";
	// find the name of current dir
	char * Dirname =  get_current_dir_name();
	char * currentDIR = Dirname ;
	// find makeFile in the directory .
	char *mkName = (char *) malloc(strlen(mk1)+strlen(currentDIR)+2) ;
	if (mkName == NULL)
	{
		perror("error when doing malloc");
		exit(EXIT_FAILURE);
	}
	// flag checking if there is makefile in currentDIr
	int MkFlag = 0;
	struct dirent *entry;
	// open the current dir
	if ((dir = opendir(currentDIR)) == NULL)
	{
		// exit if problem in opening directory

		perror("opendir() error");
		exit(EXIT_FAILURE);
	}

	else {
		/* iterate through the dir to find the makefile*/
		int whileFlag = 0 ;
		while ((entry = readdir(dir)) != NULL && (whileFlag==0))
		{
			// if there is a file named makefile or Makefile, then we found the makefile
			if (strcmp(entry->d_name, mk1) == 0 || strcmp(entry->d_name, mk2) == 0  )

			{
				// set flag to be 1 if there is makefile
				MkFlag = 1;
				// copy the name to mkName
				strcpy(mkName,currentDIR);
				strcat(mkName, "/");
				strcat(mkName, entry->d_name);
				mkName[strlen(mkName)] = '\0';
				whileFlag = 1;
			}
		}
		// if we have not found the makefile
		if ( MkFlag == 0)
		{
			perror("makefile Not found");
			exit(0);
		}
		FILE *fp ;
		fp = fopen(mkName, "rb");
		// exit if error
		if( fp == NULL )
		{
			printf("Error while opening the file.!!! \n");
			exit(EXIT_FAILURE);
		}
		// used in the getline function
		size_t dum = 0 ;
		char * linePointer= NULL ;
		// NewSpec is initialized to 0 and changed to one if we are done reading specification
		int NewSpec = 0 ;
		char * tagetname ;
		// set initial size of dependencysize to be intitialSize and may be changed
		int intitialSize =  dependencysize *sizeof(char*);
		const int FixedSizeTarget = dependencysize *sizeof(char*);
		// set initial size of command list to be intitialSize and may be changed
		int intitialSizeCom =  dependencysize *sizeof(char*);
		const int FixedSizeCommand = dependencysize *sizeof(char*);
		// record the number of lines in command section
		int readNumLine = 0;
		int sizeCharPointer = sizeof(char**);
		// allocate space to dependencyList

		char ** dependencyList = (char **) malloc(intitialSize) ;
		if (dependencyList== NULL)
		{
			perror("Error while doing malloc \n");
			exit(EXIT_FAILURE);

		}
		// allocate space in the commandlist
		char ** commandLoist = (char **) malloc(intitialSizeCom) ;;
		if (commandLoist== NULL)
		{
			perror("Error while doing malloc \n");
			exit(EXIT_FAILURE);

		}
		// record the numer of dependency
		int depencyListNUm = 0 ;
		// record the number of words of command in total
		int NumargPerline = 0;

		// it is empty file flag
		int emptyFile = 0;
		// it is blank line file flga
		int BlankFlag = 0 ;
		// allocate space to head
		linkedList * head = malloc(sizeof(linkedList)) ;
		if (head== NULL)
		{
			perror("Error while doing malloc \n");
			exit(EXIT_FAILURE);

		}
		// set up the offset size to be 0 used to keep track of the linkedlist
		int offset = 0 ;
		/* read each line the makefile and pass each specification
		 * into buildgraph file and build the graph
		 */
		while(getline(&linePointer , &dum ,fp ) != -1 )
		{
			// it is not empty file
			emptyFile= 1;
           // set the first to be the first words in each line
			char * first = strtok(linePointer,s);

			if (first == NULL)
			{
				// it is a blank line
			}
			else
			{
				// then it is not blank
				BlankFlag = 1;
               // it there is space in file then it is wrong format
				if(linePointer[0] == ' ' )
				{
					perror("wrong formant for makefile");
					exit(EXIT_FAILURE);
				}

               // to see whether it is command line or not
				int ifcommand = checkIfCommand(linePointer[0]) ;
                // if it is target line
				if (ifcommand == 0 )
				{
                    // if we already read one specification in the past, then pass old one into buildGraph file
					if(NewSpec == 1 )
					{

						buildUpdateGraph ( tagetname, dependencyList  ,depencyListNUm,   commandLoist,NumargPerline , readNumLine,head, offset);
						/* offsetCorrection is used to keep track of the number of node in
						 * our linkedlist
						 */
						int offsetCorrection = 0 ;

						linkedList * temp = head;

						while(temp->next != NULL)
						{
							offsetCorrection ++ ;
							temp = temp->nextNode;
						}

						// update our offset
						offset = offsetCorrection ;


                      // set readNumLine to be 0 and used as count in next specification
						readNumLine =  0;
                        /*free up memory in dependecylist and
                         * commandline list
                         */
						int loop1 = 0 ;
						while (loop1<depencyListNUm)
						{
							free(dependencyList[loop1]);
							loop1++;
						}
						loop1= 0;
						while (loop1<NumargPerline)
						{
							free(commandLoist[loop1]);
							loop1++;
						}
						free(dependencyList);
						free(commandLoist);
						// reset initial size of dependency list and commandline list to FixedSizeTarget
						intitialSize = FixedSizeTarget;
						intitialSizeCom = FixedSizeCommand;
						/* allocate space again for dependencylist
						 * and commandline list
						 */
						dependencyList = (char **) malloc(intitialSize) ;
						if (dependencyList== NULL)
						{
							perror("Error while doing malloc \n");
							exit(EXIT_FAILURE);

						}
						commandLoist = (char **) malloc(intitialSizeCom) ;
						if (commandLoist== NULL)
						{
							perror("Error while doing malloc \n");
							exit(EXIT_FAILURE);

						}
						// set everyflag to be 0
						depencyListNUm = 0;
						NumargPerline = 0;
						NewSpec = 0;
					}
					// set tagetname to be first word in line
					tagetname =  checkIfitisTarget (first ) ;
					// set flag to be 1 to mean we have encounter a new specification
					NewSpec = 1;
                   /*while loop is used to read dependency and store depenency name in
                    * the dependecy list and reallocate them if they exceed the initial size
                    * of dependecy list
                    */
					while( first != NULL )
					{
						first = strtok(NULL, s);
						if(first != NULL)
						{
							dependencyList[depencyListNUm] = (char*)malloc(strlen(first)+1);
							if (dependencyList[depencyListNUm]== NULL)
							{
								perror("Error while doing malloc \n");
								exit(EXIT_FAILURE);

							}
							strcpy(dependencyList[depencyListNUm],first);

							depencyListNUm++;
							/*reallocate size if necessary
							 */
							if(depencyListNUm*sizeCharPointer>intitialSize)
							{
								dependencyList = (char**)realloc(dependencyList,intitialSize*2);
								if (dependencyList== NULL)
								{
									perror("Error while doing realloc \n");
									exit(EXIT_FAILURE);

								}
								intitialSize = intitialSize*2;

							}
						}
					}
				}

				else  // it is command line
				{
					// we store words in command line in commandlist and seperate each
					// command line by space character
					readNumLine ++ ;
					/*reallocate the space  for command line list if necessary
					 * and double the size
					 */
					if(NumargPerline*sizeCharPointer>intitialSizeCom)
					{
						commandLoist = (char**)realloc(commandLoist,intitialSizeCom*2);
						if (commandLoist== NULL)
						{
							perror("Error while doing realloc \n");
							exit(EXIT_FAILURE);

						}
						intitialSizeCom = intitialSizeCom*2;

					}
                   // allocate space for each index in commandlist to store the words
					(commandLoist[NumargPerline]) = (char*)malloc(strlen(first)+1);
					if (commandLoist[NumargPerline]== NULL)
					{
						perror("Error while doing malloc \n");
						exit(EXIT_FAILURE);

					}
					strcpy(commandLoist[NumargPerline],first);
					NumargPerline++;

					while( first != NULL )
					{

						first = strtok(NULL, s);
						if ( first!=NULL)
						{
							 // copy each word into commandlist
							commandLoist[NumargPerline] = (char*)malloc(strlen(first)+1);
							if (commandLoist[NumargPerline]== NULL)
							{
								perror("Error while doing malloc \n");
								exit(EXIT_FAILURE);

							}
							strcpy(commandLoist[NumargPerline],first);
							NumargPerline++;
							// reallocate space for commandlist if necessary
							if(NumargPerline*sizeCharPointer>intitialSizeCom)
							{
								commandLoist = (char**)realloc(commandLoist,intitialSizeCom*2);
								if (commandLoist== NULL)
								{
									perror("Error while doing realloc \n");
									exit(EXIT_FAILURE);

								}
								// double the size
								intitialSizeCom = intitialSizeCom*2;

							}
						}
					}
					// finished reading one comman dline
					// add space character into the last index of commandlist
					//reallocate space if necessary
					if(((NumargPerline+1)*sizeCharPointer)>intitialSizeCom)
					{
						commandLoist = (char**)realloc(commandLoist,intitialSizeCom*2);
						if (commandLoist== NULL)
						{
							perror("Error while doing realloc \n");
							exit(EXIT_FAILURE);

						}
						intitialSizeCom = intitialSizeCom*2;

					}

					commandLoist[NumargPerline] = (char*)malloc(strlen(" ") +1 );
					if (commandLoist[NumargPerline]== NULL)
					{
						perror("Error while doing malloc \n");
						exit(EXIT_FAILURE);

					}
					strcpy(commandLoist[NumargPerline]," ");
					NumargPerline++;
				}
			}
		}
		// if emptyFile flag is 0 meaning file is empty then exit
		if(emptyFile == 0)
		{
			perror( "emptyfile ");
			exit(EXIT_FAILURE);
		}
		// if BlankFlag flag is 0 meaning file is blank then exit

		if( BlankFlag == 0 )
		{
			perror( "blank line  ");
			exit(EXIT_FAILURE);
		}
		// pass the last specification into buildgraph file
		buildUpdateGraph ( tagetname, dependencyList  ,depencyListNUm,   commandLoist,NumargPerline , readNumLine,head, offset);

       /* set every flag into 0 and free up the memory in the dependency list and
        * commandlist
        */
		readNumLine =  0;
		int loop1 = 0 ;
		while (loop1<depencyListNUm)
		{
			free(dependencyList[loop1]);
			loop1++;
		}
		loop1= 0;
		while (loop1<NumargPerline)
		{
			free(commandLoist[loop1]);
			loop1++;
		}
		free(dependencyList);
		free(commandLoist);
		intitialSize = FixedSizeTarget;
		intitialSizeCom = FixedSizeCommand;


		handleAndCompile(runSpec,head);


		closedir(dir);
	}
	return 1 ;

}

/* checkIfCommand check if command is tab character
 * return 0 if it is not and return 1 if it is
 *  command:  character
 */
int checkIfCommand (char command )
{
	if (command != '\t')
	{
		return 0 ;
	}

	return 1 ;
}

/* checkIfitisTarget check if target is target word
 * give error and exit if it is not ending with :
 * or return truncated targetname wihout :
 * target: potential valid target name
 */
char * checkIfitisTarget(char * target)
{

	int strlength = strlen(target);
    // check if it is ending with :
	char temp = target[strlength-1];
	if (temp!= ':')
	{
		perror("wrong format in makefifle \n");
		exit(EXIT_FAILURE);
	}
   // allcoate space for target name
	char * targetname = (char * ) malloc( strlen(target));
	if (targetname== NULL)
	{
		perror("problem when doing malloc \n");
		exit(EXIT_FAILURE);
	}
	//copy the target into target name except :
	int  i = 0;
	while (i < (strlen(target)-1) )
	{
		targetname [i] = target[i];
		i++ ;
	}
	targetname [i] = '\0';

	return targetname ;
}

