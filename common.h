#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>	//ture or false;
#include <jpeglib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>



int wait4touch(int ts);
bool is_jpeg(char *j_name);
/* read dir*/
// 设计好双向循环链表的节点
typedef struct node
{
	char *data;
	struct node *prev;
	struct node *next;
}listnode, *linklist;
linklist init_list(void);
linklist newnode(char *data);
void list_add_tail(linklist new, linklist head);
linklist readpicname( char *argv) ;

/*jpeg.c*/
void show_jpeg(char *argv);

#endif