//#include "lpc177x_8x_lcd.h"
#include "ff.h"
#include "string.h"
#include "bmp.h"
#include "mem_my.h"


uint32_t get_bmp_num(uint8_t *path)
{
	FRESULT res;
	DIR dp;
	FILINFO tfileinfo;
	uint32_t count = 0;
	res = f_opendir(&dp,(const char *)path);
	if(res == FR_OK)
	{
		while(1)
		{
			res=f_readdir(&dp,&tfileinfo); 
			if((res != FR_OK) || (tfileinfo.fname[0] == 0))
			{
				break;
			}
			else
			{
				if(is_bmp_file((uint8_t *)tfileinfo.fname))
				{
					count++;
				}
			}
		}
		
		f_closedir(&dp);
	}
	return count;
}

uint8_t is_bmp_file(uint8_t * filename)
{
	uint8_t len;
	len = strlen((const char*)filename);
	if(len < 3)
	{	
		return 0;
	}
	
	if(((filename[len-3]=='b') || (filename[len-3]=='B')) && ((filename[len-2]=='m')||(filename[len-2]=='M')) && 
		((filename[len-1]=='p')||(filename[len-1]=='P')))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

void convert_bmp(uint8_t * dst, uint8_t * src,uint16_t pic_w,uint16_t pic_h)
{
	uint8_t * pd = dst;
	uint8_t * ps;
	
	uint16_t i;
	
	for(i = 0;i < pic_h ;i++ )
	{
		ps = &src[(pic_h - 1 - i) * pic_w * 2];
		
		mem_copy(pd,ps,pic_w * 2);
		
		pd += pic_w * 2;
		
	}
	
}


void bmp_3to2(uint8_t *dst, uint8_t *src,uint16_t pic_w,uint16_t pic_h)
{
	uint8_t *src1;
	uint16_t *dst1;
	uint8_t r,g,b;

	
	uint32_t i;

	src1 = src;
	dst1 = (uint16_t *)dst;

	for(i=0;i<pic_w*pic_h;i++)
	{
		
		b = src1[0];
	    g = src1[1];
		r = src1[2];
		
		


		dst1[i] = (b >> 3) | ((g >> 3)<<5) | ((r >> 3) << 10);
		
	
		src1 += 3;
	/*	
		if(i == 151)
		{
			src1 += 1;
		}
	*/
	}

	
}
