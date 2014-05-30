/*/*****************************************************
 * FILE NAME: ProcessCreation.c
 *
 * Created on: Oct 10 2013
 * Author: Yihong Dai guangpu zhou
 * Professor : Barton P Miller
 * cs login: yihong  guangpu
 * PURPOSE: this file is used to run the command line and use fork and execvp to
 * run the command in the shell
 *
 *****************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/* RunChildProcess run command line in the shell
 * give error and exit if it is invalid command in the shell
 * or return status
 * command: commandline
 */
int  RunChildProcess ( char ** command )
{
	// the first string in the command
	char * filename = command[0];
	struct rusage * space = NULL ;
	int status = 0;
	pid_t pid;
   // fork a child process to run the execvp
	pid = fork ();
	if (pid == 0)
	{
		/* This is the child process.  Execute the shell command. */
		int val = execvp (filename, command);


		exit (EXIT_FAILURE);
	}
	else if (pid < 0)
		/* The fork failed.  Report failure.  */
	{
		status = -1;
	}
	else{
		/* This is the parent process.  Wait for the child to complete.  */
		if (wait3 (&status, 0 , space) != pid)
		{
			perror("fork error in the wait3\n");
			exit (EXIT_FAILURE);
		}

		return  status;
	}
	return status ;
}
