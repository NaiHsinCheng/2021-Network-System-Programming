/*
 * is_background.c :  check for & at end
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.
  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 *
	 * Fill in code.
	 */
	int count = 0, i; 
	while(myArgv[++count] != NULL);
	for (i=0; i < count; i++){
    		if(strcmp(myArgv[i],"&") == 0){
			return 1;
		}
  	}
	return 0;

}
