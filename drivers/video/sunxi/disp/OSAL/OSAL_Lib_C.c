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

#include "OSAL.h"

extern int kdb_trap_printk;

/* 普通内存分配 */
void * OSAL_malloc(u32 Size)
{
	return kmalloc(Size, GFP_KERNEL | __GFP_ZERO);
}

void OSAL_free(void *pAddr)
{
	kfree(pAddr);
}

/* 连续的物理内存分配 */
void * OSAL_PhyAlloc(u32 Size)
{
	return kmalloc(Size, GFP_KERNEL | __GFP_ZERO);
}

void OSAL_PhyFree(void *pAddr, u32 Size)
{
	kfree(pAddr);
}


/* 虚拟内存和物理内存之间的转化 */
unsigned int OSAL_VAtoPA(void *va)
{
#if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN6I)
    if((unsigned int)(va) > 0x40000000)
		return (unsigned int)(va) - 0x40000000;
#endif
	return (unsigned int)(va);
}

void *OSAL_PAtoVA(unsigned int pa)
{
	return (void*)(pa);
}


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
void *	 OSAL_io_remap(u32 phy_addr , u32 size)
{
	return (void* __force)ioremap(phy_addr,  size);
}

int OSAL_printf(const char *fmt, ...)
{
#if 0
	va_list args;
	int r;

#ifdef CONFIG_KGDB_KDB
	if (unlikely(kdb_trap_printk)) {
		va_start(args, fmt);
		r = vkdb_printf(fmt, args);
		va_end(args);
		return r;
	}
#endif
	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);
#endif
	return 0;
}

int OSAL_putchar(int value)
{
	return 0;
	}
int OSAL_puts(const char * value)
{
	return 0;
}
int OSAL_getchar(void)
{
	return 0;
}
char * OSAL_gets(char *value)
{
	return NULL;
}

/* 字符串转长整形 */
long OSAL_strtol (const char *str, const char **err, int base)
{
	return 0;
}

/* 有符号十进制整形转字符串*/
void OSAL_int2str_dec(int input, char * str)
{
}

/* 十六进制整形转字符串*/
void OSAL_int2str_hex(int input, char * str, int hex_flag)
{
}

/* 无符号十进制整形转字符串*/
void OSAL_uint2str_dec(unsigned int input, char * str)
{
}



