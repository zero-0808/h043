#include "ff.h"
#include "bmp.h"
#include "mem_my.h"
#include "string.h"
#include "nand.h"
#include "led.h"
#include "crypto.h"
#include "update.h"
#include "eeprom.h"
#include "w25qxx_qspi.h"


#define  TEMP_SIZE           8192
static merge_file_t merge;
source_configure src_cfg;
static uint8_t analysis_merge_head(uint8_t* buf,merge_file_t* merge);
static uint8_t check_addr(merge_file_t merge);
static uint8_t merge_next_file(merge_file_t* merge);
static void wirte_nand(uint32_t block,uint32_t page,uint8_t* buf,uint32_t size,uint8_t flag);

uint32_t read_source(FIL file,uint8_t * buf,uint32_t size);


uint32_t read_source(FIL file,uint8_t * buf,uint32_t size)
{
    uint32_t size1,size2,temp_size,bw;
    uint8_t * p;
    uint8_t res;

    if(buf == NULL)
    {
        return 0;
    }

    size1 = size;
    size2 = 0;
	

    while(size2 < size1)
    {
        p = buf + size2;
        temp_size = ((size1 - size2) > (TEMP_SIZE)) ? (TEMP_SIZE) : (size1 - size2);
        res = f_read(&file,(void *)p,temp_size,&bw);
        if(res == FR_OK)
        {
		    size2 += temp_size;
		}
		else
		{
		    break;
		}
    }

    return size2;
    
}



static uint8_t save_source(uint32_t addr,FIL file,uint32_t lseek,uint32_t size)
{
	uint16_t block,page;
	uint32_t size1,size2,temp_size;
	uint8_t * buffer;
//	uint8_t * p;
	
	buffer = (uint8_t *) my_malloc(nand_dev.page_mainsize);
	
	if(buffer == NULL)
	{
		return 1;
	}



	//read_source(file, buffer,size);
	
	block = (addr >> 16) & 0XFFFF;
	page = (addr) & 0XFFFF;
	size1 = size;
	size2 = 0;
	
	while(size2 < size1)
	{
		//p = buffer + size2;
		temp_size = (size1 - size2)>(nand_dev.page_mainsize)?(nand_dev.page_mainsize):(size1-size2);
		read_source(file, buffer,temp_size);
		if(crypt.flag>0)
        {
            crypt.encrypt_decrypt(crypt.cal_user_key,buffer,temp_size);
        }
		nand_dev.page_program(NAND_LBNToPBN(block),page,buffer,0);
		size2 += temp_size;
		f_lseek(&file,size2+lseek);
		page ++;
		if(page >= nand_dev.block_pagenum)
		{
			page = 0;
			block ++;
		}
	}

	my_free(buffer);

	return 0;

	
}












static void update_tci_source(FIL file,uint32_t lseek,uint16_t block)
{
    uint32_t size,addr;
//    size = f_size(&file);
	size = merge.size[merge.num];
    addr = (block << 16) & (0XFFFF0000);
    save_source(addr,file,lseek,size);
}
static uint16_t pic_cnt=0;

uint8_t update_source(void)
{
	uint8_t res,i;
	FIL file;
    FATFS fs;
	uint16_t temp[9],block_num,tmp;
	uint16_t pic_w,pic_h;
	//uint8_t flag = 2;
	uint32_t size,lseek=0;
	uint32_t block_size,screen_size,pic_offset;
	uint32_t page,block;
    uint16_t nand_block_off=0;
	uint8_t * buf;
	uint32_t  bw;
	uint8_t * buf1 = NULL;
    uint8_t * buf2 = NULL;
	BMP_HEAD head;
	
	for(i=0;i<MERGE_NUM;i++)
	{
		temp[i] = 0xffff;
	}

    res = f_mount (&fs,"0:",1);
	
	if(res != FR_OK)
	{
		return 1;
	}
    
	res = f_open(&file,"0:/JHD_CFG/JHD_CFG.out",FA_READ);

	if(res != FR_OK)
	{
        return 1;

		   
	}
    LED_RED
    buf = (uint8_t *)my_malloc(MERGE_HEAD_SIZE);
        
    res = f_read(&file,(void *)buf,MERGE_HEAD_SIZE,&bw);
    res = analysis_merge_head(buf,&merge);
    if(res!=0)
    {
        return 1;
    }
    merge.num = 0;
    my_free(buf);
    
	nand_dev.create_lut(0);

	block_size = nand_dev.block_pagenum * nand_dev.page_mainsize;
    block_num = 0;
    
    temp[0] = 0;
    lseek += MERGE_HEAD_SIZE;
	res = f_lseek(&file, lseek);
	if(res != FR_OK)
	{
		f_close(&file);
		return 1;
	}
    
    //font.bin
    update_tci_source(file,lseek,temp[merge.num]);
    size = merge.size[merge.num];
    block_num = size / block_size;
    if(size % block_size)
    {
        block_num++;
    }
    nand_block_off+= block_num;
    //block_num = 0;
    lseek += merge.size[merge.num];
    f_lseek(&file, lseek);
    merge_next_file(&merge);
    temp[merge.num] = nand_block_off;
    
    //12.bin  
    buf = (uint8_t *)my_malloc(28);
    f_read(&file,(void *)buf,merge.size[merge.num],&bw);
    if(crypt.flag>0)
    {
        crypt.encrypt_decrypt(crypt.cal_user_key,(void*)buf,28);
    }
    size = merge.size[merge.num];
    bsp_EraseCpuFlash((uint32_t)para_flash_area);
//    EEPROM_Erase(2);
//	EEPROM_Write(0,2,(void*)buf,MODE_8_BIT,size);
//    my_free(buf);
//    buf = NULL;
    bsp_WriteCpuFlash((uint32_t)para_flash_area + 32*0,(void*)buf,size);
	my_free(buf);
	buf = NULL;

    lseek += merge.size[merge.num];
    f_lseek(&file, lseek);
    merge_next_file(&merge);
    temp[merge.num] = nand_block_off;
    
	while(merge.num<MERGE_NUM-1)/*图片资源单独处理*/
	{
	    update_tci_source(file,lseek,temp[merge.num]);
	    size = merge.size[merge.num];
	    block_num = size / block_size;
	    if(size % block_size)
	    {
	        block_num++;
	    }
	    //f_close(&file);
		nand_block_off += block_num;
		//block_num = 0;
        lseek += merge.size[merge.num];
        f_lseek(&file, lseek);
		merge_next_file(&merge);
		temp[merge.num] = nand_block_off;
	}
	while(res == FR_OK)
	{
        //图片资源处理
        res = f_read(&file, (void *)&head,sizeof(BMP_HEAD), &bw);
        if(res != FR_OK)
        {
            f_close(&file);
            return 1;
        }
        if(crypt.flag>0)
        {
            crypt.encrypt_decrypt(crypt.cal_user_key,(void*)&head,sizeof(BMP_HEAD));
        }

        pic_w = (head.pic_w_h << 16) | head.pic_w_l;
        pic_h = (head.pic_h_h << 16) | head.pic_h_l;	
        pic_offset = (uint32_t)((head.pic_data_address_h << 16) | head.pic_data_address_l);
        lseek += pic_offset;
        f_lseek(&file,lseek);
        screen_size = pic_w*pic_h*head.pic_dip/8;

        if(buf==NULL)
        {
            buf = (uint8_t *) my_malloc(screen_size);
            if(buf == NULL)
            {
                f_close(&file);
                return 1;
            }
        }

        if(buf1==NULL)
        {
            buf1 = (uint8_t *) my_malloc(screen_size);
            if(buf1 == NULL)
            {
                f_close(&file);
                my_free(buf);
                return 1;
            }
        }

        if(buf2==NULL)
        {
            buf2 = (uint8_t *) my_malloc(screen_size);
            if(buf2 == NULL)
            {
                f_close(&file);
                my_free(buf);
                my_free(buf1);
                return 1;
            }
        }

        read_source(file, buf, screen_size);
        if(crypt.flag>0)
        {
            crypt.encrypt_decrypt(crypt.cal_user_key,buf,screen_size);
        }
        if(head.pic_dip == 24)
        {
            bmp_3to2(buf1, buf,pic_w,pic_h);                   
            convert_bmp(buf2,buf1,pic_w,pic_h);
        }
        else if(head.pic_dip == 16)
        {
            
            convert_bmp(buf2,buf,pic_w,pic_h);
        }

        page = screen_size * pic_cnt / nand_dev.page_mainsize;
        if(screen_size % nand_dev.page_mainsize)
        {
            page += pic_cnt; 
        }
        pic_cnt++;
        block = page / nand_dev.block_pagenum;
        block += temp[merge.num];
        page = page % nand_dev.block_pagenum;
        wirte_nand(block,page,buf2,screen_size,0);
        lseek += screen_size;
        res = f_lseek(&file,lseek);
        if(lseek>=f_size(&file))
        {
            break;
        }
	}
	
//	EEPROM_Erase(1);

	//src_cfg.block_font_addr = block_addr[0];  //font.bin
	//src_cfg.block_touch_addr = block_addr[2]; //13.bin
	//src_cfg.block_cmd_addr = block_addr[3];   //14.bin
	//src_cfg.block_lang_addr = block_addr[4];  //str.bin
	//src_cfg.block_icon_addr = block_addr[5];  //15.bin
	//src_cfg.block_screen_addr = block_addr[6];    //17.bin
	//src_cfg.block_bmp_addr = block_addr[7];   //bmp
    //temp[0] = block_addr[0];
    temp[1] = temp[0];
    tmp = temp[4];  //15.bin
    temp[4] = temp[5];//str.bin 
    temp[5] = tmp;
//    EEPROM_Write(0,1,(void*)&temp[1],MODE_16_BIT,7);
    bsp_WriteCpuFlash((uint32_t)para_flash_area + 32*1,(void*)&temp[1],7);
	
	//EEPROM_Erase(0);
    buf[0] = 'n';
    buf[1] = 'a';
    buf[2] = 'n';
    buf[3] = 'd';
//    EEPROM_Write(0,0,(void*)buf,MODE_8_BIT,4);
    bsp_WriteCpuFlash((uint32_t)para_flash_area + 32*2,(void*)buf,4);
    f_close(&file);
    f_mount(&fs,"0:",0);
	return res;
}



/*******************************************************************************
* Function     : analysis_merge_head
* Description  : 解析合并文件的文件头
* Input Para   : 
-->file:文件结构体
-->merge:合并文件结构体
* Output Para  : NULL
* Return Value : 0,正确 1，文件头数据不对 2，解析失败
*******************************************************************************/
static uint8_t analysis_merge_head(uint8_t* buf,merge_file_t* merge)
{
    uint16_t i,ret=0;
    uint32_t addr=0,size=0;
	ret = crypt.encrypt_decrypt(crypt.cal_key,buf,MERGE_HEAD_SIZE);
	if(ret!=MERGE_HEAD_SIZE)
	{
		//解密失败
		return 2;
	}
    for(i=0;i<MERGE_NUM*8;i+=8)
    {
		merge->num = i/8;
        addr = (uint32_t)(buf[i]<<(24))+(uint32_t)(buf[i+1]<<(16))+(uint32_t)(buf[i+2]<<(8))+(uint32_t)buf[i+3];
		if(i<(MERGE_NUM-1)*8)
		{
 			size = (uint32_t)(buf[i+4]<<(24))+(uint32_t)(buf[i+5]<<(16))+(uint32_t)(buf[i+6]<<(8))+(uint32_t)buf[i+7];
			merge->size[merge->num] = size;
		}
        
        merge->addr[merge->num] = addr;

		
    }

    merge->size[MERGE_NUM-1] = 0;/*图片文件固定大小，需要读取图片文件时再判断*/

    ret = check_addr(*merge);
    if(ret!=0)
    {
        return ret;
    }
	crypt.flag = buf[64];
	if(crypt.flag==1)        //加密
	{
		crypt.user_key_size = buf[65];
        if(crypt.user_key_size>0)
        {
            memcpy(crypt.user_key,&buf[66],crypt.user_key_size);
            crypt.cal_user_key = crypt.cal(crypt.user_key,crypt.user_key_size);
        }      
	}
    
	
    return ret;
}


/*******************************************************************************
* Function     : check_addr
* Description  : 校验文件头的资源文件地址和大小是否正确
* Input Para   : 
-->merge:合并文件结构体
* Output Para  : NULL
* Return Value : 0，正确；1，错误
*******************************************************************************/
static uint8_t check_addr(merge_file_t merge)
{
    uint8_t i,j;
    uint32_t tmp;
    for(i=0;i<MERGE_NUM-3;i++)
    {
        if(merge.addr[i]!=0xffffffff)
        {
            for(j=i+1;j<MERGE_NUM-2;j++)
            {
                if(merge.addr[j]!=0xffffffff)
                {
                    tmp = merge.addr[i]+merge.size[i];
                    if(tmp!=merge.addr[j])
                    {
                        return 1;
                    }
                    break;
                }
            }
            
        }
    }
    return 0;
}



/*******************************************************************************
* Function     : merge_next_file
* Description  : 查找合并文件中的下一个有效文件
* Input Para   : 
-->merge:合并文件结构体
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
static uint8_t merge_next_file(merge_file_t* merge)
{
    //uint8_t ret=0;
    merge->num++;
    while(merge->num<MERGE_NUM)
    {
        
        if(merge->addr[merge->num]!=0xffffffff)
        {
            return 1;
        }
        merge->num++;
    }
    return 0;
}


/*******************************************************************************
* Function     : wirte_nand
* Description  : 写入nand数据
* Input Para   : 
-->block:块地址
-->page:页地址
-->buf:数据
-->size:数据大小
* Output Para  : 无
* Return Value : 无
*******************************************************************************/
static void wirte_nand(uint32_t block,uint32_t page,uint8_t* buf,uint32_t size,uint8_t flag)
{
    uint16_t block1,page1;
    uint32_t size1=0,tmp_size;//size2=0;
    //uint8_t * wbuf = NULL;
    
    block1 = block;
    page1 = page;
    
    
    //wbuf = (uint8_t *) my_malloc(nand_dev.page_totalsize);
    //mem_set(wbuf,0xff,nand_dev.page_totalsize);
//    if(wbuf == NULL)
//    {
//        return;
//    }
    while(size1<size)
    {
        tmp_size = ((size-size1)>(nand_dev.page_mainsize)?(nand_dev.page_mainsize):(size-size1));
        if(flag==0)
        {
            nand_dev.page_program(NAND_LBNToPBN(block1),page1,buf+size1,0);
            size1+=tmp_size;
        }
        else
        {
            //size2 +=nand_dev.page_mainsize;
            //mem_copy(wbuf,buf+size1,tmp_size);
            //wbuf[nand_dev.page_mainsize + 1] = 0xee;
            //nand_dev.page_program(nand_dev.lut[block1],page1,wbuf,1);
            //size1+=nand_dev.page_mainsize;
        }
        
        page1++;
        if(page1>=nand_dev.block_pagenum)
        {
            block1++;
            page1 = 0;
        }
    }
}
void update_app(void)
{
#if 0
	uint8_t res;
	FATFS fs;
	FIL file;




	res = f_mount (&fs,"0:",1);

	if(res != FR_OK)
	{
		return;
	}

	res = f_open(&file,"0:/JHD_APP/JHD_APP",FA_READ);

	if(res != FR_OK)
	{
		return;
	}




	write_flash(APP_START_ADDR,file);
    f_close(&file);
    f_mount (&fs,"0:",0);
	GPIO_OutputValue(0, 1<<26, 0);
    Host_DelayMS(200);
    GPIO_OutputValue(0, 1<<26, 1);
#else

	FRESULT res;
	FIL fp;
	uint8_t read[4096];
	UINT br = 0 , readLen = 4096;
	int size = 0 , erasesector = 0, pos = 0;

	//DEBUG_MAIN("UpdateW25qxxFileCode %d !!!\r\n" , HAL_GetTick());
	res = f_open(&fp, "0:/JHD_APP/JHD_APP", FA_READ |FA_OPEN_EXISTING);
	if(res == FR_OK)
	{
//    		LTDC_Init();
//    		DEBUG_MAIN("Update Firmware\r\n");
		size = f_size(&fp);
		if(size < 4096)
		{
			//SetUpdateFwState(UPDATE_FW_FILE_SIZE_ERR);
			f_close(&fp);
			//DEBUG_MAIN("Firmware size is wrong\r\n");
			return;
		}
		erasesector = ((size%4096)==0)?(size/4096):(size/4096 + 1);

		//DEBUG_MAIN("start erase sector size %d\r\n" , erasesector);
		for(uint32_t i = 0; i < erasesector ; i++)
		{
			W25qxx_EraseSector(i*4096);
		}
		pos = 0;
		while(size > pos)
		{
			br = 0;
			f_lseek(&fp,pos);
			res = f_read(&fp, (void *)read , readLen , &br);

			//DEBUG_MAIN("read length  %d , write pos %x\r\n" , br , pos);
			if(res == FR_OK)
			{
				W25qxx_WriteNoCheck(read , pos , br);
			}
			else
			{
			///	SetUpdateFwState(UPDATE_FW_FILE_PROGRAM_ERR);

				//DEBUG_MAIN("down load Firmware is err pos %d\r\n" , pos);
				f_close(&fp);
				return;
			}
			pos += br;
		}
		///DEBUG_MAIN("end iap success\r\n");
		f_close(&fp);
		//f_unlink("0:\\UPDATE\\UPDATE_FW.bin");//delete file, and next time power up can not go into iap again
		//SetUpdateFwState(UPDATE_FW_FILE_COMPLETE);
	}
	else
	{
		//DEBUG_MAIN("no fw firmware skip\r\n");
		f_close(&fp);
	//	SetUpdateFwState(UPDATE_FW_FILE_SKIP);
	}
#endif



}

