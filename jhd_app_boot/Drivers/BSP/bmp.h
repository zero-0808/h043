#ifndef __BMP_H__
#define __BMP_H__

#include"stdint.h"

typedef struct __BMP
{
	/*BMP文件头（14字节）*/
	uint8_t  pic_head[2];						//位图文件的类型，必须为BM(1-2字节）
	uint16_t pic_size_l;			    	//位图文件的大小，以字节为单位（3-6字节，低位在前）
	uint16_t pic_size_h;			    	//
	uint16_t pic_nc1;				    		//位图文件保留字，必须为0(7-8字节）
	uint16_t pic_nc2;				    		//位图文件保留字，必须为0(9-10字节）
	uint16_t pic_data_address_l;	  //位图数据的起始位置，以相对于位图（11-14字节，低位在前）文件头的偏移量表示，以字节为单位
	uint16_t pic_data_address_h;		//	
	/*位图信息头（40字节）*/
	uint16_t pic_message_head_len_l;//本结构所占用字节数（15-18字节）
	uint16_t pic_message_head_len_h;//
	uint16_t pic_w_l;								//位图宽度，以像素为单位（19-22字节）
	uint16_t pic_w_h;				    		//
	uint16_t pic_h_l;				    		//位图高度，以像素为单位（23-26字节）
	uint16_t pic_h_h;				    		//
	uint16_t pic_bit;				    		//目标设备的级别，必须为1(27-28字节）
	uint16_t pic_dip;				    		//每个像素所需的位数，必须是1（双色），（29-30字节）4(16色），8(256色）16(高彩色)或24（真彩色）之一
	uint16_t pic_zip_l;			    		//位图压缩类型，必须是0（不压缩），（31-34字节）1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
	uint16_t pic_zip_h;			    		//
	uint16_t pic_data_size_l;		    //位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位（35-38字节）
	uint16_t pic_data_size_h;		    //
	uint16_t pic_dipx_l;			    	//位图水平分辨率，每米像素数（39-42字节）
	uint16_t pic_dipx_h;			    	//	
	uint16_t pic_dipy_l;			    	//位图垂直分辨率，每米像素数（43-46字节)
	uint16_t pic_dipy_h;			    	//
	uint16_t pic_color_index_l;	    //位图实际使用的颜色表中的颜色数（47-50字节）
	uint16_t pic_color_index_h;	    //
	uint16_t pic_other_l;			    	//位图显示过程中重要的颜色数（51-54字节）
	uint16_t pic_other_h;			    	//
	/*************颜色表*****************/
	//uint16_t pic_color_p01;		    	//28
	//uint16_t pic_color_p02;		    	//29
	//uint16_t pic_color_p03;		    	//30
	//uint16_t pic_color_p04;		    	//31
	//uint16_t pic_color_p05;		    	//32
	//uint16_t pic_color_p06;		    	//33
	//uint16_t pic_color_p07;		    	//34
	//uint16_t pic_color_p08;					//35			
}BMP_HEAD;

typedef struct BMP_point
{
	uint16_t x;
	uint16_t y;
	uint8_t  r;
	uint8_t  g;
	uint8_t  b;		
}BMP_POINT;


uint8_t is_bmp_file(uint8_t * filename);
uint32_t get_bmp_num(uint8_t *path);
void convert_bmp(uint8_t * dst, uint8_t * src,uint16_t pic_w,uint16_t pic_h);
void bmp_3to2(uint8_t *dst, uint8_t *src,uint16_t pic_w,uint16_t pic_h);

#endif
