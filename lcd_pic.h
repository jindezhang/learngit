#ifndef __LCD__PIC_H_
#define __LCD__PIC_H_

#define RED 2
#define GREEN 1
#define BLUE 0
struct header
{
	int16_t type;
	int32_t size; //pic size
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits; // pic data start
}__attribute__((packed));

struct info
{
    int32_t size; // 本结构大小    
    int32_t width; // 图像宽度（单位像素）
    int32_t height; // 图像高度（单位像素）
    int16_t planes; // 总为零
    int16_t bit_count; // 色深
    int32_t compression; // 是否压缩
    int32_t size_img;
    int32_t X_pel;
    int32_t Y_pel;
    int32_t clrused;
    int32_t clrImportant;
}__attribute__((packed));

struct quad
{
   int8_t blue;
   int8_t green;
   int8_t red;
   int8_t reserved;
}__attribute__((packed));

	
struct bmp_info
{
	int width;
	int height;
	int bpp;
};
char *init_lcd(struct fb_var_screeninfo *pvinfo);
char *load_bmp(const char *pathname,struct bmp_info *pimginfo);
void display_bmp(char *fbmem,struct fb_var_screeninfo *pvinfo,char *imgdata,struct bmp_info *pimginfo);

#endif
