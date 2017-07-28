#include "common.h"

linklist init_list(void)
{
	linklist head = malloc(sizeof(listnode));
	if(head != NULL)
	{
		head->prev = head;
		head->next = head;
		head->data = 0;
	}

	return head;
}

linklist newnode(char *data)
{
	linklist new = malloc(sizeof(listnode));
	if(new != NULL)
	{
		new->data = data;
		new->prev = new;
		new->next = new;
	}

	return new;
}

// 函数功能：将new节点，插入到head节点的前面，即链表的末尾
void list_add_tail(linklist new, linklist head)
{
	if(new == NULL)
		return;

	new->prev = head->prev;
	new->next = head;

	head->prev->next = new;
	head->prev = new;
}

linklist readpicname( char *argv) 
{
	linklist head = init_list();
	char *target = argv;

	// 要查看的目标是个目录
	DIR *dp = opendir(target);

	if(dp == NULL)
	{
		perror("opendir() failed");
		exit(0);
	}
	/**********************
	opendir() 之前不能进入该目录，不然会找不到对应的目录去open。
	***************/
	chdir(target);
	struct stat fileinfo;
	struct dirent *ep = NULL;
	while(1)
	{
		errno = 0;

		ep = readdir(dp);

		// 读取目录项发生了错误
		if(ep == NULL && errno != 0)
		{
			perror("readdir() failed");
			break;
		}

		// 目录项读完了
		else if(ep == NULL)
		{
			break;
		}
		bzero(&fileinfo, sizeof(fileinfo));
		stat(ep->d_name, &fileinfo);
		printf("%s\n",ep->d_name );
		if(!S_ISREG(fileinfo.st_mode))
				continue;
		if(!is_jpeg(ep->d_name))
			continue;
		
		if(head->data == 0)
			head->data = ep->d_name;
		else
		{
			linklist new = newnode(ep->d_name);
			list_add_tail(new,head);
		}
	}

	return head;
}

