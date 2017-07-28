#include "common.h"
#include "lcd_pic.h"


char *init_lcd(struct fb_var_screeninfo *pvinfo)
{
    int lcd = open("/dev/fb0",O_RDWR|O_EXCL);
    if(lcd == -1)
    {
            perror("open /dev/fb0 failed");
            exit(0);
    }   
    bzero(pvinfo, sizeof(struct fb_var_screeninfo));
    ioctl(lcd, FBIOGET_VSCREENINFO, pvinfo); // 获取LCD设备的详细信息
    char *fbmem = mmap(NULL,(pvinfo->xres * pvinfo->yres *pvinfo->bits_per_pixel /8),PROT_READ|PROT_WRITE,MAP_SHARED,lcd,0);

    if(*fbmem == -1) 
    {   
            perror("mmap（）failed!");
            exit(0);
    }   
    return fbmem;
}

char *load_bmp(const char *pathname,struct bmp_info *pimginfo)
{	
	int bmp = open(pathname,O_RDONLY);
	
	struct header *bmp_head = calloc(1,sizeof(struct header));
	
	if(bmp_head == NULL)
	{
		perror("bmp_head calloc() failed!");
		exit(0);
	}
	struct info *b_info = calloc(1,sizeof(struct info));
	if(b_info == NULL)
	{
		perror("bmp_info calloc() failed!");
		exit(0);
	}

	if(read(bmp,bmp_head,sizeof(struct header)) == -1)
	{
		perror("read () failed!");
		exit(0);
	}
	if(read(bmp,b_info,sizeof(struct info)) == -1)
	{
		perror("read () failed!");
		exit(0);
	}
	
	if(b_info->compression)
	{	
		printf("exist compression!");
		exit(0);
	}
	pimginfo->width = b_info->width;
	pimginfo->height = b_info->height;
	pimginfo->bpp = b_info->bit_count/8;

	lseek(bmp,bmp_head->offbits,SEEK_SET);	
	char *imgdata_buf = calloc(1,bmp_head->size - bmp_head->offbits);
	if(imgdata_buf == NULL)
	{
		perror("imgdata_buf calloc() failed!");
		exit(0);
	}
	int nread = bmp_head->size - bmp_head->offbits;
	int n;
	char *tmp = imgdata_buf;
	while(nread > 0)
	{

		n = read(bmp,tmp,nread);
		nread -=n;
		tmp +=n;
	}
	// printf("bmp_head->size:%d\n",bmp_head->size);
	// printf("bmp_head->offbits:%d\n",bmp_head->offbits);

	free(bmp_head);
	free(b_info);
	return imgdata_buf;

}
void display_bmp(char *fbmem,struct fb_var_screeninfo *pvinfo,char *imgdata,struct bmp_info *pimginfo) 
{
	int x = width_mid(pimginfo->width,pvinfo->xres);
	int y = height_mid(pimginfo->height,pvinfo->yres);

	int pad = (4-(pimginfo->width * pimginfo->bpp)%4)%4;
	printf("pad:%d",pad);	
	
	fbmem += y*pvinfo->xres*pvinfo->bits_per_pixel/8+x*pvinfo->bits_per_pixel/8;

	imgdata += (pimginfo->width * pimginfo->bpp+pad) * (pimginfo->height - 1);
	int w = ((pvinfo->xres-x) > pimginfo->width ? pimginfo->width:(pvinfo->xres-x));
	int h = ((pvinfo->yres-y) > pimginfo->height ?pimginfo->height:(pvinfo->yres-y));
	int i ,j;

	for(j = 0;j<h;j++)
	{
		long fb_offset,img_offset;
		for(i=0;i < w; i++)
		{
			fb_offset  = i * pvinfo->bits_per_pixel/8 + j * pvinfo->xres * pvinfo->bits_per_pixel/8;
			img_offset = i * pimginfo->bpp; 
			memcpy(fbmem + fb_offset + pvinfo->red.offset/8,imgdata+img_offset + RED,1);
			memcpy(fbmem + fb_offset + pvinfo->green.offset/8,imgdata+img_offset + GREEN,1);
			memcpy(fbmem + fb_offset + pvinfo->blue.offset/8,imgdata+img_offset + BLUE,1);
		
		}
		imgdata -= pimginfo->width * pimginfo->bpp +pad;

	}

}

void show_bmp(char *bmp_name)
{
	struct fb_var_screeninfo vinfo;
    char *fbmem = init_lcd(&vinfo);
	struct bmp_info imginfo;
    char *imgdata = load_bmp(bmp_name,&imginfo);
    display_bmp(fbmem,&vinfo,imgdata,&imginfo);
}