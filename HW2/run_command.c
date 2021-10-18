/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int status;

    /* Create a new child process.
     * Fill in code.
     */
    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
	     */
	    if(!is_background(myArgv)){			//如果沒有＆1就等待子行程結束
	    	if(waitpid(pid, &status, 0) == -1){
			perror("waitpid");
			exit(1);
	    	}
	    }

            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
	     */
	    if(WIFSIGNALED(status)){
		printf("%s terminated by signal %d.\n", myArgv[0], WTERMSIG(status));
	    }else if (WIFSTOPPED(status)){
		printf("%s stopped by signal %d.\n", myArgv[0], WSTOPSIG(status));
	    }

            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
	     */
	    execlp(myArgv[0], myArgv[0], myArgv[1], (char *) NULL);
            /* Handle error return from exec */
	    exit(errno);
    }
}
