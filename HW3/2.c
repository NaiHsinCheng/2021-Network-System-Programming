#include <stdio.h>

#include <stdlib.h>

#include <pwd.h>

#include <grp.h>

#include <ctype.h>

#include <dirent.h>

#include <string.h>


struct pstree_node {
	char name[128];
	uid_t pid;
	uid_t ppid;
	struct pstree_node * parent;
	struct pstree_node * children[128];
	struct pstree_node * next;
};

static struct pstree_node * head;

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

/* 檢查/proc/PID/status檔案中的"PPid:" 建立node */
int catalog(char *dirname)
{

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
	char ppid[32];
	char *key;
	char *value;
	
	while (fgets(linebuf, sizeof(linebuf), fp) != NULL) {

		key = strtok(linebuf, ":");	//key:value:....
		value = strtok(NULL, ":");

		if (key != NULL && value != NULL) {

			str_trim(key);
			str_trim(value);

			if(strcmp(key, "PPid") == 0){		
				strcpy(ppid, value);
			}

			else if (strcmp(key, "Name") == 0) {
				strcpy(name, value);
			}

			else if (strcmp(key, "Pid") == 0) {
				strcpy(pid, value);
			}
		}
	}

	struct pstree_node *node;

	node = (struct pstree_node*)malloc(sizeof(struct pstree_node));

	if (node == NULL) {
		printf("Unable to allocate memory for node\n");
		return 1;
	}
	
	strcpy(node->name, name);
	node->pid = atoi(pid);
	node->ppid = atoi(ppid);
	node->children[0] = NULL;
	node->parent = NULL;
	node->next = head;
	head = node;
	
	return 0;
}

/* Return "pid"所對應的node 若沒有則return NULL */
struct pstree_node* find_node_with_pid(int pid)
{
	struct pstree_node *node;

	for (node = head; node != NULL; node = node->next) {
		if (node->pid == pid) {
			return node;
		}
	}
	return NULL;
}

/* 印出tree */
void print_tree(struct pstree_node * root, int level)
{
	int i;
	struct pstree_node *node;

	for (i = 0; i < level; i++) {
		printf("|__");
	}

	printf("%s (pid: %d, ppid: %d)\n\n", root->name, root->pid, root->ppid);

	/* 遞迴children 印出 */
	i = 0;
	while ((node = root->children[i++]) != NULL) {
		print_tree(node, level + 1);
	}

	return;
}

/* 利用建立好的linked list 連結parent-child關係 建立tree */
int tree(void)
{
	int i;
	struct pstree_node *node, *p_node;

	//遍歷每一個節點 連結其parent-child關係
	for (node = head; node != NULL; node = node->next) {
		i = 0;
		p_node = find_node_with_pid(node->ppid);
		if (p_node != NULL) {
			node->parent = p_node;
			while (p_node->children[i++] != NULL);	//把目前的node連結到p_node->children的第一個空位置
			p_node->children[i - 1] = node;
			p_node->children[i] = NULL;
		}
		
	}

	
	for (node = head; node != NULL; node = node->next) {
		if (node->parent == NULL) {	//找出根節點root
			print_tree(node, 0);
		}
	}


	return 0;
}

int main(int argc, char **argv)
{

	DIR *dirp;
	struct dirent *dp;	//directory_entry
	char dirname[256];

	/* comandline參數錯誤 */
	if (argc != 1) {
		printf("Usage: %s <username>\n", argv[0]);
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
				catalog(dirname);
		}
	}

	tree();

	return 0;
}

