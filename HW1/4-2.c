#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define BUF_SIZE 512


void copy(int fdin, int fdout)
{

    char buffer[BUF_SIZE] = {0};
    ssize_t size = -1;

    while (size = read(fdin, buffer, BUF_SIZE)){
	if (write(fdout, buffer, size) != size){
		perror("Write Error");
		exit(EXIT_FAILURE);
	}
    }
    if (size < 0){
		perror("Read Error");
		exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])	//argc:參數個數 argv:參數值
{
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;

    if(argc != 3){
	perror("Argc Error");
	exit(EXIT_FAILURE);
    }

    if((fd_in = open(argv[1], O_RDONLY)) == -1){
	perror("Open Error");
	exit(EXIT_FAILURE);
    }

    if((fd_out = creat(argv[2], 0664)) == -1){	//0664 COPYMODE
	perror("Open Error");
	exit(EXIT_FAILURE);
    }

    copy(fd_in, fd_out);
    close(fd_in);
    close(fd_out);

    return 0;
}
