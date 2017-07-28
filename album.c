#include "common.h"

/**
	实现简单的JPEG/JPG/bmp相册浏览功能.
	当图片比屏幕小时候，居中显示。图片大于屏幕分辨率时候，只能显示屏幕的一部份；
	可以判断是否是.jpg/.jpeg/.bmp文件。
	程序运行时，识别：./album  ./album dir/..  ./album  xxx.jpeg/.jpg/.bmp
	程序实现思路：
	1）判断输入的目录还是文件，如果是文件，直接显示。如果是目录，则2）
	2）opendir() 获取该目录的指针类型，chdir()进入该目录，把目录中所有JPEG/JPG/BMP文件的文件名读取存放。
	3）判断点击屏幕位置，点击屏幕中间则退出程序；点击左边或者右边 ，那么显示上一张图片 OR 下一张图片。
	4）显示图片，把文件名传给JPEG/JPG文件解码函数或者BMP文件解码函数显示。
**/
int main(int argc, char *argv[]) 
{
	
	if(argc > 2)
	{
		printf("invalid argument\n");
		exit(0);
	}

	char *target;
	if(argc == 1)
		target = ".";

	if(argc == 2)
	{
		target = argv[1];
		/* only one pic */

		struct stat fileinfo;
		bzero(&fileinfo,sizeof(fileinfo));
		stat(target,&fileinfo);
		if(S_ISREG(fileinfo.st_mode) )
		{
			if(is_jpeg(target))
				show_jpeg(target);
			else
				show_bmp(target);
			return 0;
		}
	}
	
	//读取目录中的文件名。存放在双向循环列表的head 中。
	linklist head = readpicname(target);
	
	// 准备好触摸屏
	int ts = open("/dev/input/event0", O_RDONLY);
	if(ts == -1)
	{
		perror("open /dev/input/event0 failed");
		exit(0);
	}

	///////////////////
	/* show picture*/
	////////////
	linklist tmp = head;
	int i;
	printf("点击屏幕中间可以exit！\n");

	while(1)
	{
		printf("%s\n", tmp->data);
		if(is_jpeg(tmp->data))
			show_jpeg(tmp->data);
		else
			show_bmp(tmp->data);
		i = wait4touch(ts);
		if(i == 1)
			tmp = tmp->next;
		else
			tmp = tmp->prev;
	}
	return 0;
}


bool is_jpeg(char *j_name)
{
	if(strstr(j_name,".jpeg") == 0 && strstr(j_name,".jpg") == 0)
		return false;
	else
		return true;
}

bool is_bmp(char *b_name)
{
	if(strstr(b_name,".bmp") == 0 )
		return false;
	else
		return true;
}

///////////////
/* get the touch x y ;*/
int wait4touch(int ts)
{
	struct input_event buf;

	/*****
		用于判断是否读取到了点击的位置的x值和y值，触摸屏的buf.value值每次只能读取到一个
	********/
	bool x_done = false;
	bool y_done = false;
	int ABS_x;
	while(1)
	{
		read(ts, &buf, sizeof(buf));

		// 读到了触摸屏的坐标信息
		if(buf.type == EV_ABS)
		{
			if(buf.code == ABS_X)
			{
				ABS_x = buf.value;
				x_done = true;
			}	
			if (buf.code == ABS_Y)
			{
				y_done = true;
			}
		}

		// 读到了按键的事件
		if(buf.type == EV_KEY)
		{
			if(buf.code == BTN_TOUCH && buf.value == 0)
				if(x_done && y_done)
					break;

		}
	}
	if(ABS_x > 500)
		return 1;
	else if(ABS_x <300)
		return 0;
	else 
		exit(0);
}
