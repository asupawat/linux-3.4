/*
*************************************************************************************
*                         			eBsp
*					   Operation System Adapter Layer
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: OSAL_Lib_C.h
*
* Author 		: javen
*
* Description 	: C�⺯��
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   2010-09-07          1.0         	create this word
*		holi		   2010-12-03		   1.1			������OSAL_io_remap
*************************************************************************************
*/
#ifndef  __OSAL_LIB_C_H__
#define  __OSAL_LIB_C_H__



/* ��ͨ�ڴ���� */
void * OSAL_malloc(u32 Size);
void OSAL_free(void *pAddr);

/* �����������ڴ���� */
void * OSAL_PhyAlloc(u32 Size);
void OSAL_PhyFree(void *pAddr, u32 Size);

/* �����ڴ�������ڴ�֮���ת�� */
unsigned int OSAL_VAtoPA(void *va);
void *OSAL_PAtoVA(unsigned int pa);


/*
*******************************************************************************
*                     IO��ַת��
*
* Description:
*    	��һ�������ַת��Ϊ�����ַ
*
* Parameters:
*		phy_addr	��	�����ַ
*		size		:	��ַ�ĳ���
* 
* Return value:
*		==0			:	ʧ��
*		!=0			:	�����ַ
*
* note:
*    	size������4KΪ������������4k��������
*
*******************************************************************************
*/
void *	 OSAL_io_remap(u32 phy_addr , u32 size);

int OSAL_printf(const char *, ...);



int OSAL_putchar(int);
int OSAL_puts(const char *);
int OSAL_getchar(void);
char * OSAL_gets(char *);

/* �ַ���ת������ */
long OSAL_strtol (const char *str, const char **err, int base);

/* �з���ʮ��������ת�ַ���*/
void OSAL_int2str_dec(int input, char * str);

/* ʮ����������ת�ַ���*/
void OSAL_int2str_hex(int input, char * str, int hex_flag);

/* �޷���ʮ��������ת�ַ���*/
void OSAL_uint2str_dec(unsigned int input, char * str);


#endif  

