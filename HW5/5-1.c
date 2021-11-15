#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define BSIZE 1024

int main(){

	int i;
				//file descripter-> pd[0]:read, pd[1]:write
	int pd[2];		//pd: child寫給parent
	int pd2[2];		//pd2: parent寫給child
	char buf_c[BSIZE], buf_p[BSIZE];


	if(pipe(pd) == -1 || pipe(pd2) == -1){		//在fork()之前產生pipe()
		perror("pipe");
		exit(1);
	}

	switch(fork()){

		//fork出錯
		case -1:
			perror("fork");
			exit(1);
			break;

		//child
		case 0:

			close(pd2[1]);
			close(pd[0]);

			//讀取parent
			while(read(pd2[0], buf_c, BSIZE)){

				//轉換成大寫
				for(i=0; i<strlen(buf_c); i++){
					buf_c[i] = buf_c[i] - 32; 
				}
				
				//傳給parent
				write(pd[1], buf_c, strlen(buf_c));

				memset(buf_c,'\0',BSIZE);

			}
			break;

		//parent
		default:
			break;

	}
	
	close(pd2[0]);
	close(pd[1]);
	while(scanf("%s", buf_p)){
		//傳給child
		write(pd2[1], buf_p, strlen(buf_p));
		
		//讀取child
		read(pd[0], buf_p, BSIZE);
		printf("%s\n", buf_p);
		
	}

	return 0;
	
	
}
