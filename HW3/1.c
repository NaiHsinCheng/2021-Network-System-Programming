#include <stdio.h>

#include <stdlib.h>

#include <pwd.h>

#include <grp.h>

#include <ctype.h>

#include <dirent.h>

#include <string.h>


/* Return 'name'對應的UID, 出錯時return -1 */
uid_t  userIdFromName(const char *name)

{

    struct passwd *pwd;

    uid_t u;

    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */

        return -1;                      /* return an error */

    u = strtol(name, &endptr, 10);      /* As a convenience to caller */

    if (*endptr == '\0')                /* allow a numeric string */

        return u;

    pwd = getpwnam(name);

    if (pwd == NULL)

        return -1;

    return pwd->pw_uid;

}

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
}

/* 檢查/proc/PID/status檔案中的"Name:"和"Uid:" */
int check_dir(char *dirname, uid_t uid)
{
	char uidstr[128];
	sprintf(uidstr, "%d", uid);		//uid_t轉string

	char filename[256];		
	strcpy(filename, dirname);		//filename:/proc/.../status
	strcat(filename, "/status");

	/* 開檔 */
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		return 1; 			/* 開檔失敗忽略 */
	}

	char linebuf[256];
	char name[256];
	char pid[32];
	int keepme = 0;
	char *key;
	char *value;
	
	while (fgets(linebuf, sizeof(linebuf), fp) != NULL) {

		key = strtok(linebuf, ":");	//key:value:....
		value = strtok(NULL, ":");

		if (key != NULL && value != NULL) {

			str_trim(key);
			str_trim(value);

			if(strcmp(key, "Uid") == 0){		
				if(strstr(value, uidstr) != NULL)		//strstr()：返回子字串
					keepme = 1;
			}

			else if (strcmp(key, "Name") == 0) {
				strcpy(name, value);
			}

			else if (strcmp(key, "Pid") == 0) {
				strcpy(pid, value);
			}
		}
	}

	if (keepme) {
		printf("%s : %s\n", pid, name);
	}

	return 0;
}


int main(int argc, char **argv)
{

	uid_t uid;

	DIR *dirp;
	struct dirent *dp;	//directory_entry
	char dirname[256];

	/* comandline參數錯誤 */
	if (argc != 2) {
		printf("Usage: %s <username>\n", argv[0]);
		return 1;
	}

	/* 找出name對應的uid */

	uid = userIdFromName(argv[1]);

	/* 對應出錯的情況 */
	if (uid == (uid_t)-1) {
		printf("Invalid username specified (not found)\n");
		return 1;
	}
	
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

		if (dp->d_type == DT_DIR || dp->d_type == DT_LNK) {	//DT_DIR：目錄 -- DT_LNK ：符號連結symbolic link
				strcpy(dirname, "/proc");
				strcat(dirname, "/");
				strcat(dirname, dp->d_name);	//dirname[0]:/proc/...
				//printf("%s\n", dirname);
				check_dir(dirname, uid);
		}
	}

	return 0;
}

