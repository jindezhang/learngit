#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char const *argv[])
{
	int i = copy(argv[1]);
	printf("%d\n",i );
	/* code */
	return 0;
}


int copy(char *argv)
{
	// i代表当前已经打印了的行数
	int i = 1;
	char *target = argv;
	printf("target:%s\n",target);
	// 要查看的目标是个目录
		DIR *dp = opendir(target);

		if(dp == NULL)
		{
			printf("targetAA:%s\n",target);
			perror("opendir() failed");
			exit(0);
		}
	
		// 进入指定的目录
		chdir(target);
	       printf("target:%s\n",target);
	return i;
}
