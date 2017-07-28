#include "common.h"

struct image_info
{
	int width;
	int height;
	int pixel_size;
};
/*********
	用于返回可以让图片在屏幕上居中显示的坐标值。
*****/
int width_mid(int pic_w,int fb_w)
{
	if(pic_w >= fb_w)
		return 0;
	return ((fb_w - pic_w)/2);
}

int height_mid(int pic_h,int fb_h)
{
	if(pic_h >= fb_h)
		return 0;
	return ((fb_h - pic_h)/2);
}

read_image_from_file(int fd, unsigned char *jpg_buffer, unsigned long image_size)
{
	int nread = 0;
	int size = image_size;
	unsigned char *buf = jpg_buffer;
	while(size > 0)
	{
		nread = read(fd, buf, size);
		if(nread == -1)
		{
			printf("read jpg failed \n");
			exit(0);
		}
		size -= nread;
		buf +=nread;
	}
}

void write_lcd(unsigned char *FB, struct fb_var_screeninfo *vinfo, 
				unsigned char *bmp_buffer, struct image_info *imageinfo)
{
	int i = width_mid(imageinfo->width,vinfo->xres);
	int j = height_mid(imageinfo->height,vinfo->yres);
	
	/** FB是填写 **/
	FB += j*vinfo->xres*vinfo->bits_per_pixel/8+i*vinfo->bits_per_pixel/8;

	int x,y;
	for(y = 0; y<imageinfo->height && y<vinfo->yres; y++)
	{
		for(x = 0; x<imageinfo->width && x<vinfo->xres; x++)
		{
			int image_offset = x * 3 + y * imageinfo->width * 3;
			int lcd_offset = x * 4 + y * vinfo->xres * 4;

			memcpy(FB+lcd_offset, bmp_buffer+image_offset+2, 1);
			memcpy(FB+lcd_offset+1, bmp_buffer+image_offset+1, 1);
			memcpy(FB+lcd_offset+2, bmp_buffer+image_offset, 1);
		}
	}
	usleep(500000);
}


void show_jpeg(char *argv)
{	
	

	struct stat file_info;
	stat(argv, &file_info);
	int fd = open(argv, O_RDWR);
	if(fd == -1)
	{
		printf("open the picture failed\n");
	}

	unsigned char *jpg_buffer;
	jpg_buffer = calloc(1, file_info.st_size);
	read_image_from_file(fd, jpg_buffer, file_info.st_size);

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpg_buffer, file_info.st_size);

	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		fprintf(stderr, "jpeg_read_header failed: %s\n", strerror(errno));
		exit(1);
	}

	jpeg_start_decompress(&cinfo);

	struct image_info *image_info = calloc(1, sizeof(struct image_info));
	if(image_info == NULL)
	{
		printf("malloc image_info failed\n");
	}	

	image_info->width = cinfo.output_width;
	image_info->height = cinfo.output_height;
	image_info->pixel_size = cinfo.output_components;

	int row_stride = image_info->width * image_info->pixel_size;

	unsigned long bmp_size;
	unsigned char *bmp_buffer;
	bmp_size = image_info->width * image_info->height * image_info->pixel_size;
	bmp_buffer = calloc(1, bmp_size);

	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	int lcd = open("/dev/fb0", O_RDWR);
	if(lcd == -1)
	{
		printf("open the lcd failed %s", strerror(errno));
		exit(1);
	}

	struct fb_var_screeninfo vinfo;
	ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

	unsigned char *FB;
	unsigned long bpp = vinfo.bits_per_pixel;
	FB = mmap(NULL, vinfo.xres * vinfo.yres * bpp/8,
			PROT_READ|PROT_WRITE, MAP_SHARED, lcd, 0);

	write_lcd(FB, &vinfo, bmp_buffer, image_info);

}
