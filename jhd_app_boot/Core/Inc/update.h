#ifndef __UPDATA_H__
#define __UPDATA_H__

#include "stdint.h"

#define  MERGE_HEAD_SIZE    (256)   //合并文件的文件头大小
#define  MERGE_NUM    (8)   //合并文件中的文件个数
typedef struct
{
    uint8_t num;        //当前传输的是第几个文件
    uint32_t  addr[MERGE_NUM];     //当前传输的起始地址
    uint32_t  size[MERGE_NUM];     //当前传输的原始文件大小，BMP文件大小固定
    //uint32_t  encry_size[MERGE_NUM];     //当前传输的加密文件大小，BMP文件大小固定，如果没有加密，密文大小等于原文大小
}merge_file_t;

typedef struct source_configure
{
	uint16_t block_font_addr;   //字体格式文件font.bin的存储地址
    //uint16_t block_font_size; 
	uint16_t block_touch_addr;  //触控配置文件13.bin的存储地址
    uint16_t block_touch_size;
	uint32_t text_index;
	uint16_t block_cmd_addr;    //按钮控件的对内外指令存储地址
    uint16_t block_cmd_size;
	uint16_t block_icon_addr;   //图标文件
    //uint16_t block_icon_size;
	uint16_t block_lang_addr;   //多国语言字符串编辑
    //uint16_t block_lang_size; 
	uint16_t block_screen_addr; //页面描述信息
    //uint16_t block_screen_size;
	uint16_t block_bmp_addr;    //bmp图片
	
	
}source_configure;
extern source_configure src_cfg;
uint8_t update_source(void);
void update_app(void);

#endif
