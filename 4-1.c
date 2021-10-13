#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>

#define BUF_SIZE 512


void output(int fd)
{
    int i = 0;
    ssize_t checkR = -1, checkW = -1;
    char buffer[BUF_SIZE] = {0};
    char ch;

    fflush(stdin);	//清空輸入緩衝區，為了確保不影響後面的資料讀取
    fflush(stdout);	//清空輸出緩衝區

    while((checkR = read(STDIN_FILENO, &ch, 1)) != 0){
	if (checkR == -1){
		perror("read");
		exit(EXIT_FAILURE);
	}
        buffer[i++] = ch;
        if(ch = '\n' || i == BUF_SIZE){
            checkW = write(STDOUT_FILENO, buffer, i); 	//輸出至stdout
	    if (checkW == -1){
		perror("writr");
		exit(EXIT_FAILURE);
	    }
            if(fd > 0){
		checkW = write(fd, buffer, i);
		if (checkW == -1){
			perror("writr");
			exit(EXIT_FAILURE);
	    	}
	    }
            memset(buffer, 0, sizeof(buffer));
            i = 0;
        }
    }
}

void usage(){
    printf("Usage: ./4-1 [-a-h] [FILE]\n\n"
       	   "Reads its standard input until end-of-file, writing a copy of the input to standard output and to the file named in its command-line argument. \n\n"
           "  -a, append text to the end of a file if it already exists\n"
	   "  -h, display this help and exit\n"
    );
    exit(EXIT_FAILURE);		//異常退出
}

int main(int argc, char *argv[])	//argc:參數個數 argv:參數值
{
    int opt, fd = -1;
    int flag_append = 0;
    int flags = O_WRONLY | O_CREAT;	//以寫的方式打開文件 | 若文件不存在將創建一個新文件

    while((opt = getopt(argc, argv, "ah"))!=-1) {
        switch(opt){
            case 'a':
                flag_append = 1;
                break;
	    case 'h':
                usage();
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }


    flags += flag_append ? O_APPEND:O_TRUNC;	//加內容在後面 ｜ 覆蓋現有文件
    fd = open(argv[argc-1], flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fd == -1){
         printf("invaild file -- \"%s\"\n", argv[optind]);
         exit(EXIT_FAILURE);
    }


    output(fd);
    if(fd > 0)
        close(fd);

    return 0;
}
