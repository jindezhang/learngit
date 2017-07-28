#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/types.h>

int main(void)
{
	int ts = open("/dev/input/event0", O_RDONLY);

	struct input_event buf;

	while(1)
	{
		read(ts, &buf, sizeof(buf));

		// 读到了触摸屏的坐标信息
		if(buf.type == EV_ABS)
		{
			if(buf.code == ABS_X)
				printf("x: %d\n", buf.value);

			if(buf.code == ABS_Y)
				printf("y: %d\n", buf.value);
		}

		// 读到了按键的事件
		if(buf.type == EV_KEY)
		{
			if(buf.code == BTN_TOUCH && buf.value == 0)
				break;

		}
	}

	close(ts);

	return 0;
}
