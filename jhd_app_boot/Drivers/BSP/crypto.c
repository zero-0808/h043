#include "crypto.h"
#include "mem_my.h"
/***************************************************************************************
****************************************************************************************
* Project      : h043wqcm0101_raaa1_afa1
* FILE         : crypto.c
* Description  : Encrypt and decrypt
*                      
* Copyright (c) 2018 by ShenZhen JingHua Displays Co.,Ltd... All Rights Reserved.
* 
* History:
* Version		name			Date			Description
* 0.01			Wang Xianyin	2019/12/18		Initial Version
* 
****************************************************************************************
****************************************************************************************/
//��λ������λ��������Կ
static unsigned char cry_key[8]={25,52,39,32,2,41,11,57};
static unsigned char user_key[8]={0};


crypto_t crypt;
static unsigned char JHD_cal(unsigned char* key,unsigned char size);
//static unsigned int Aes_Encrypt_CBCString(unsigned char* key,unsigned char* data,unsigned char* endata,unsigned short key_size,unsigned int data_size);
//static unsigned int Aes_Decrypt_CBCString(unsigned char* key,unsigned char* data,unsigned char* dedata,unsigned short key_size,unsigned int data_size);
//static unsigned int JHD_encrypt(unsigned char* key,unsigned char* data,unsigned char* endata,unsigned short key_size,unsigned int data_size);
//static unsigned int JHD_decrypt(unsigned char* key,unsigned char* data,unsigned char* dedata,unsigned short key_size,unsigned int data_size);
static unsigned int MCU_JHD_encrypt_decrypt(unsigned char cal_key,unsigned char* data,unsigned int data_size);
/*******************************************************************************
* Function     : crypto_init
* Description  : ��ʼ��
* Input Para   : NULL
* Output Para  : NULL
* Return Value : NULL
*******************************************************************************/
void crypto_init(void)
{
    crypt.flag = 0; //Ĭ�ϲ�����
	crypt.key = cry_key;
	crypt.key_size = 8;
    
    crypt.user_key = user_key;
    crypt.user_key_size = 0;
    crypt.cal_key = JHD_cal(crypt.key,crypt.key_size);
    crypt.cal_user_key = 0;
    crypt.cal = JHD_cal;
    crypt.encrypt_decrypt = MCU_JHD_encrypt_decrypt;
	//crypt.encrypt = JHD_encrypt;
	//crypt.decrypt = JHD_decrypt;
}

/*******************************************************************************
* Function     : JHD_cal
* Description  : JHD�㷨���ɵ���ֵ
* Input Para   : NULL
* Output Para  : NULL
* Return Value : ������ֵ
*******************************************************************************/
static unsigned char JHD_cal(unsigned char* key,unsigned char size)
{
    unsigned char data0=0,data1=0,data2=0;
	unsigned int i;

	for(i=0;i<size;i++)
	{
		data0 |= (key[i]%2<<i);
		data1 += (key[i]%256);
	}
	data2 = ~(data0&data1);
    return data2;
}

///*******************************************************************************
//* Function     : Aes_Encrypt_CBCString
//* Description  : AES��CBCģʽ��������,�����ļ����Ȳ��ܱ�16���������ں��油��
//* Input Para   : 
//-->key:��Կ
//-->data:����
//-->endata:����
//-->key_size:��Կ����Ϊ16��24��32
//-->data_size:���ĳ���
//* Output Para  :
//-->endata:����ָ��
//* Return Value : 0��ʧ�ܣ����㣬�ļ���С
//*******************************************************************************/
//static unsigned int Aes_Encrypt_CBCString(unsigned char* key,unsigned char* data,unsigned char* endata,unsigned short key_size,unsigned int data_size)
//{
//    unsigned char fill_byte=0,fill_buf[16],i;
//	unsigned int count,count_i=0;
//	unsigned char* encry_buf;
//	void(*AES_en_func)(unsigned char*,unsigned char*);
//    if(data_size%16)
//    {
//        fill_byte = 16-data_size%16;
//    }
//	
//    
//    for(i=0;i<16;i++)
//    {
//        fill_buf[i] = 0;
//    }
//	switch (key_size)
//	{
//		case 16:
//		{
//			AES_en_func = Aes_key128bit_Encrypt;
//			break;
//		}
//		case 24:
//		{
//			AES_en_func =Aes_key192bit_Encrypt;
//			break;
//		}
//		case 32:
//		{
//			AES_en_func =Aes_key256bit_Encrypt;
//			break;
//		}
//		default:
//		{
//			return 1;
//		}
//	}

//	encry_buf = my_malloc(data_size+fill_byte);
//	if(encry_buf==NULL)
//	{
//		return 1;
//	}
//	memcpy(encry_buf, data, data_size); 
//    if(fill_byte>0)
//    {
//        memcpy(encry_buf+data_size, fill_buf, fill_byte); 
//    }
//    if(fill_byte>0)
//    {
//        count = data_size/16+1;
//    }
//    else
//    {
//        count = data_size/16;
//    }
//	for (count_i = 0; count_i < count; count_i++)
//	{
//		AES_en_func(&encry_buf[count_i * 16],key);
//	}
//	memcpy(endata, encry_buf, data_size+fill_byte);  //��������
//    my_free(encry_buf);
//	return (data_size+fill_byte);
//}

///*******************************************************************************
//* Function     : Aes_Decrypt_CBCString
//* Description  : ����AES��CBCģʽ����,���ܵ��ļ����ȱ���Ϊ16��������
//* Input Para   : 
//-->key:��Կ
//-->data:����
//-->dedata:����
//-->key_size:��Կ����Ϊ16��24��32
//-->data_size:���ĳ���
//* Output Para  :
//-->dedata:����ָ��
//* Return Value : 0��ʧ�ܣ����㣬�ļ���С
//*******************************************************************************/
//static unsigned int Aes_Decrypt_CBCString(unsigned char* key,unsigned char* data,unsigned char* dedata,unsigned short key_size,unsigned int data_size)
//{
//	unsigned int count,count_i=0;
//	unsigned char* decry_buf;
//	void(*AES_de_func)(unsigned char*,unsigned char*);
//    if((data_size%16)!=0)
//    {
//        return 0;
//    }
//	count = data_size/16;
//	switch (key_size)
//	{
//		case 16:
//		{
//			AES_de_func = Aes_key128bit_Decrypt;
//			break;
//		}
//		case 24:
//		{
//			AES_de_func =Aes_key192bit_Decrypt;
//			break;
//		}
//		case 32:
//		{
//			AES_de_func =Aes_key256bit_Decrypt;
//			break;
//		}
//		default:
//		{
//			return 0;
//		}
//	}

//	decry_buf = my_malloc(data_size);
//	if(decry_buf==NULL)
//	{
//		return 0;
//	}
//	memcpy(decry_buf, data, data_size); 
//	for (count_i = 0; count_i < count; count_i++)
//	{
//		AES_de_func(&decry_buf[count_i * 16], key);
//	}
//	memcpy(dedata, decry_buf, data_size);  //��������
//    my_free(decry_buf);
//	return data_size;
//}




///*******************************************************************************
//* Function     : JHD_encrypt
//* Description  : ʹ���Լ�������㷨����
//* Input Para   : 
//-->key:��Կ
//-->data:����
//-->endata:����
//-->key_size:��Կ���ȹ̶�Ϊ8�ֽ�
//-->data_size:���ĳ���
//* Output Para  :
//-->endata:����ָ��
//* Return Value : 0��ʧ�ܣ����㣬�ļ���С
//*******************************************************************************/
//static unsigned int JHD_encrypt(unsigned char* key,unsigned char* data,unsigned char* endata,unsigned short key_size,unsigned int data_size)
//{
//    unsigned char data0=0,data1=0,data2=0;
//	unsigned int i;

//	for(i=0;i<key_size;i++)
//	{
//		data0 |= (key[i]%2<<i);
//		data1 += (key[i]%256);
//	}
//	data2 = ~(data0&data1);
//	for(i=0;i<data_size;i++)
//	{
//		endata[i] = data[i]^data2;
//	}
//	return (data_size);
//}

///*******************************************************************************
//* Function     : JHD_decrypt
//* Description  : ʹ���Լ�������㷨����
//* Input Para   : 
//-->key:��Կ
//-->data:����
//-->endata:����
//-->key_size:��Կ���ȹ̶�Ϊ8�ֽ�
//-->data_size:���ĳ���
//* Output Para  :
//-->endata:����ָ��
//* Return Value : 0��ʧ�ܣ����㣬�ļ���С
//*******************************************************************************/
//static unsigned int JHD_decrypt(unsigned char* key,unsigned char* data,unsigned char* dedata,unsigned short key_size,unsigned int data_size)
//{
//    unsigned char data0=0,data1=0,data2=0;
//	unsigned int i;

//	for(i=0;i<key_size;i++)
//	{
//		data0 |= (key[i]%2<<i);
//		data1 += (key[i]%256);
//	}
//	data2 = ~(data0&data1);
//	for(i=0;i<data_size;i++)
//	{
//		dedata[i] = data[i]^data2;
//	}
//	return (data_size);
//}

/*******************************************************************************
* Function     : MCU_JHD_encrypt
* Description  : ʹ���Լ�������㷨����/����,MCU�Ż��ٶ�
* Input Para   : 
-->cal_key:��������Կ
-->data:����/����
-->data_size:����/���ĳ���
* Output Para  :
-->data:����/����
* Return Value : 0��ʧ�ܣ����㣬�ļ���С
*******************************************************************************/
static unsigned int MCU_JHD_encrypt_decrypt(unsigned char cal_key,unsigned char* data,unsigned int data_size)
{
	unsigned int i;

	for(i=0;i<data_size;i++)
	{
		data[i] = data[i]^cal_key;
	}
	return (data_size);
}

///*******************************************************************************
//* Function     : MCU_JHD_decrypt
//* Description  : ʹ���Լ�������㷨����,MCU�Ż��ٶ�
//* Input Para   : 
//-->cal_key:��������Կ
//-->data:����
//-->data_size:���ĳ���
//* Output Para  :
//-->data:����
//* Return Value : 0��ʧ�ܣ����㣬�ļ���С
//*******************************************************************************/
//static unsigned int MCU_JHD_decrypt(unsigned char cal_key,unsigned char* data,unsigned int data_size)
//{
//	unsigned int i;

//	for(i=0;i<data_size;i++)
//	{
//		data[i] = data[i]^cal_key;
//	}
//	return (data_size);
//}


