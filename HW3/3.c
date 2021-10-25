#include <stdio.h>

#include <stdlib.h>

#include <pwd.h>

#include <grp.h>

#include <ctype.h>

#include <dirent.h>

#include <string.h>

#include <sys/stat.h>

#include <sys/types.h>

#include <unistd.h>

/* 刪除字符串中的空格 */
void str_trim(char* s)
{
	int i, j = 0;
	for(i = 0;;i++){
		if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\0'){
			s[j] = s[i];
			j++;
		}
		else if(s[i] == '\0')
			break;
	}

	char* tmp = strstr(s, "\n");
	if(tmp != NULL)
		*tmp = '\0';
}

/* 印出檔案中的"Name:"和"Pid:" */
static int print_process(char *dirname)
{
	char filename[256];
	strcpy(filename, dirname);
	strcat(filename, "/status");

	FILE *fp;
	fp = fopen(filename, "r");

	if (fp == NULL) {
		return 1;
	}

	char linebuf[256];
	char name[256];
	char pid[32];
	char *key;
	char *value;
	

	while (fgets(linebuf, sizeof(linebuf), fp) != NULL) {

		key = strtok(linebuf, ":");
		value = strtok(NULL, ":");

		if (key != NULL && value != NULL) {
			str_trim(key);
			str_trim(value);
			if (strcmp(key, "Pid") == 0) {
				strcpy(pid, value);
			} else if (strcmp(key, "Name") == 0) {
				strcpy(name, value);
			}
		}
	}

	printf("%s (pid: %s)\n", name, pid);

	return 0;
}



/* 先檢查這個目錄下有fd子目錄 */
int check_process(char *dirname, char* match)
{
	char linkpath[256];
	strcpy(linkpath, dirname);
	strcat(linkpath, "/fd");

	char linktarget[1024];

	DIR *dirp;
	struct dirent *dp;
	int len;

	/* 打開linkpath */
	dirp = opendir(linkpath);

	/* 無法打開linkpath */
	if (dirp == NULL) {
		return 1;
	}

	/* 先檢查這個目錄下有fd子目錄 */
	for(;;){
		dp = readdir(dirp);
		if (dp == NULL)
			break;

		if (dp->d_type == DT_LNK) {
				strcpy(linkpath, dirname);
				strcat(linkpath, "/fd/");
				strcat(linkpath, dp->d_name);

				len = readlink(linkpath, linktarget, sizeof(linktarget) - 1);

				if (len > 0) {
					linktarget[len] = '\0';
					if (strcmp(linktarget, match) == 0) {
						return 0;
					}
				}
			}
	}

	return 1;
}


int main(int argc, char **argv)
{

	DIR *dirp;
	struct dirent *dp;	//directory_entry
	char dirname[256];

	/* comandline參數錯誤 */
	if (argc != 2) {
		printf("Usage: %s <filepath>\n", argv[0]);
		return 1;
	}

	printf("Processes with handles to \"%s\":\n", argv[1]);
	
	/* 打開/proc */
	dirp = opendir("/proc");

	/* 無法打開/proc */
	if (dirp == NULL) {
		perror("Opendir dailed on /proc");
		return 1;
	}

	/* 遍歷/proc中的目錄 */
	for(;;){

		dp = readdir(dirp);
		if (dp == NULL)
			break;

		if (dp->d_type == DT_DIR) {	//DT_DIR：目錄 -- DT_LNK ：符號連結symbolic link
			strcpy(dirname, "/proc");
			strcat(dirname, "/");
			strcat(dirname, dp->d_name);	//dirname[0]:/proc/...
			//printf("%s\n", dirname);
			if(!check_process(dirname, argv[1])){
					print_process(dirname);
			}
		}
	}

	return 0;
}

