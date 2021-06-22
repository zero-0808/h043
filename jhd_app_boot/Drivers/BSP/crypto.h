#ifndef _CRYPTO_H_
#define _CRYPTO_H_



typedef struct{
	unsigned char		flag;		//1�����ܣ�0��������
    unsigned char   	key_size;	//�ļ�ͷ��Կ����
	unsigned char   	user_key_size;
    unsigned char       cal_key;        //
    unsigned char       cal_user_key;
	unsigned char*	    key;		//�ļ�ͷ��Կ
	unsigned char*	    user_key;   //�ͻ���Կ
    unsigned char (*cal)(unsigned char*,unsigned char);
	unsigned int (*encrypt_decrypt)(unsigned char ,unsigned char* ,unsigned int);
	//unsigned int(*encrypt)(unsigned char* ,unsigned char* ,unsigned char* ,unsigned short,unsigned int);
	//unsigned int(*decrypt)(unsigned char* ,unsigned char* ,unsigned char* ,unsigned short,unsigned int);
}crypto_t;

extern crypto_t crypt;

void crypto_init(void);
void crypt_test(void);

#endif
