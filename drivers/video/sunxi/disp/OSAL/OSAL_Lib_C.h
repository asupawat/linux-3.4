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
* Description 	: C库函数
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   2010-09-07          1.0         	create this word
*		holi		   2010-12-03		   1.1			增加了OSAL_io_remap
*************************************************************************************
*/
#ifndef  __OSAL_LIB_C_H__
#define  __OSAL_LIB_C_H__



/* 普通内存分配 */
void * OSAL_malloc(u32 Size);
void OSAL_free(void *pAddr);

/* 连续的物理内存分配 */
void * OSAL_PhyAlloc(u32 Size);
void OSAL_PhyFree(void *pAddr, u32 Size);

/* 虚拟内存和物理内存之间的转化 */
unsigned int OSAL_VAtoPA(void *va);
void *OSAL_PAtoVA(unsigned int pa);


/*
*******************************************************************************
*                     IO地址转换
*
* Description:
*    	将一块物理地址转化为虚拟地址
*
* Parameters:
*		phy_addr	：	物理地址
*		size		:	地址的长度
* 
* Return value:
*		==0			:	失败
*		!=0			:	虚拟地址
*
* note:
*    	size必须以4K为递增颗粒，既4k的整数倍
*
*******************************************************************************
*/
void *	 OSAL_io_remap(u32 phy_addr , u32 size);

int OSAL_printf(const char *, ...);



int OSAL_putchar(int);
int OSAL_puts(const char *);
int OSAL_getchar(void);
char * OSAL_gets(char *);

/* 字符串转长整形 */
long OSAL_strtol (const char *str, const char **err, int base);

/* 有符号十进制整形转字符串*/
void OSAL_int2str_dec(int input, char * str);

/* 十六进制整形转字符串*/
void OSAL_int2str_hex(int input, char * str, int hex_flag);

/* 无符号十进制整形转字符串*/
void OSAL_uint2str_dec(unsigned int input, char * str);


#endif  

