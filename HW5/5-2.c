#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>



struct node{
	FILE* f;
	pid_t pid;
};

struct node hashTable[10000];
int len = 0;

int lookup(FILE *f){
	int i;
	if(len == 0)
		return -1;
	for(i = 0; i < len; i++){
		if(hashTable[i].f == f)
			return hashTable[i].pid;
	}
	return -1;
}

void insert(FILE *f, pid_t pid){

	hashTable[len].f = f;
	hashTable[len].pid = pid;
	len++;

}


FILE *my_popen (const char *cmd){

	int fd[2];	//file descripter-> pd[0]:read, pd[1]:write
	int pid;               

	pipe(fd);
	if(pipe(fd) == -1){		//在fork()之前產生pipe()
		perror("pipe");
		exit(1);
	}

	pid = fork();
	switch(pid){

		//fork出錯
		case -1:
			perror("fork");
			exit(1);
			break;

		//child
		case 0:
			close(fd[0]);	//關閉read
			dup2(fd[1],1);	//將stdout(1)綁定到fd[1]
			close(fd[1]);	//關閉write
			execl("/bin/sh", "sh", "-c", cmd, NULL);	//通過shell執行命令 取代當前的process
			return NULL;

		//parent
		default:
			break;
		
	}

	close(fd[1]);	//關閉write

	FILE *fp = fdopen(fd[0], "r");	// 從file descriptor創建FILE*

	insert(fp, pid);

	return fp;
}

int my_pclose (FILE *stream){

	int stat = 0;

	pid_t pid = lookup(stream);

	fclose(stream);

	while (waitpid(pid, &stat, 0) == -1) {
		if (errno != EINTR){
			stat = -1;
			break;
		}
	}
	
	
	return stat;
}


int main ()
{
    FILE *p = my_popen ("ls -l");
    char buffer[1024];
    while (fgets(buffer, 1024, p)) {
        printf(" => %s", buffer);
    }
    my_pclose(p);
}

