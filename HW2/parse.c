/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL) {
    		return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
    	 * Return NULL if no tokens in line.
	 *
	 * Fill in code.
   	 */

	token = strtok(line, delim);
	if(token == NULL)
		return NULL;

  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
	newArgv = (char**)malloc(sizeof(char*));	//生成一個二維陣列用來存多個一維陣列

  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *  - Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	while(token){
		newArgv[count] = (char*)malloc(sizeof(char)*20);	//二維陣列中每個元素用來存至多20字元的陣列
		memset(newArgv[count], '\0', sizeof(newArgv[count]));	//該一維陣列先全部初始化為\0
		strcpy(newArgv[count], token);				//複製token存到newArgv[count]中

		printf("[%d] : %s\n", count, newArgv[count]);		//印出結果
		count = count + 1;
		newArgv = (char**)realloc(newArgv, sizeof(char*)*(count+1));	//將二維陣列擴展一個空間存放下一個一維陣列
		token = strtok((char*)NULL, delim);
	}	
	

  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */
	int count = 0; 
	while(oldArgv[++count] != NULL);
	for (i=0; i < count; i++){
    		free(oldArgv[i]);
  	}

	free(oldArgv);

	
}
