/*
 * arch/arm/mach-sunxi/dram-freq/mdfs/mdfs.c
 *
 * Copyright (C) 2012 - 2016 Reuuimlla Limited
 * Pan Nan <pannan@reuuimllatech.com>
 *
 * SUNXI dram frequency dynamic scaling driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include "sunxi-mdfs.h"

#if (0)
	#define __MDFS_DEBUG
#else
	#undef  __MDFS_DEBUG
#endif

#ifdef __MDFS_DEBUG
	#define MDFS_DBG(format,args...)    mdfs_printk("[mdfs] "format,##args)
#else
	#define MDFS_DBG(format,args...)
#endif

static unsigned int __sramdata sp_backup;
__dram_para_t __sramdata softfreq_info;
static unsigned int __sramdata zqdqssave[32];

#ifdef __MDFS_DEBUG
static void __sram __mdfs_serial_put_char(char c)
{
	while (!(mctl_read_w(SUART_USR) & 2));
	mctl_write_w(c, SUART_THR);
}
static int __sram __mdfs_serial_puts(const char *string)
{
	while (*string != '\0') {
		if (*string == '\n') {
			__mdfs_serial_put_char('\r');
		}
		__mdfs_serial_put_char(*string++);
	}

	return 0;
}

static unsigned int __sram __mdfs_strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
	{
		/* nothing */
		;
	}
	return sc - s;
}
static char __sram *__mdfs_strcpy(char *dest, const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
	{
		/* nothing */
		;
	}
	return tmp;
}
static char __sram *__mdfs_itoa(int value, char *string, int radix)
{
	char stack[16];
	int  negative = 0; 
	int  i;
	int  j;
	char digit_string[] = "0123456789ABCDEF";

	if(value == 0)
	{
		string[0] = '0';
		string[1] = '\0';
		return string;
	}

	if(value < 0)
	{
		negative = 1;
		value = -value ;
	}

	for(i = 0; value > 0; ++i)
	{
		stack[i] = digit_string[value % radix];
		value /= radix;
	}

	j = 0;
	if(negative)
	{
		string[j++] = '-';
	}
	for(--i; i >= 0; --i, ++j)
	{
		string[j] = stack[i];
	}
	string[j] = '\0';

	return string;
}
static char __sram *__mdfs_utoa(unsigned int value, char *string, int radix)
{
	char stack[16];
	int  i;
	int  j;
	char digit_string[] = "0123456789ABCDEF";

	if(value == 0)
	{
		string[0] = '0';
		string[1] = '\0';
		return string;
	}

	for(i = 0; value > 0; ++i)
	{
		stack[i] = digit_string[value % radix];
		value /= radix;
	}

	for(--i, j = 0; i >= 0; --i, ++j)
	{
		string[j] = stack[i];
	}
	string[j] = '\0';

	return string;
}
static char __sram *__mdfs_strncat(char *dest, const char *src, unsigned int count)
{
	char *tmp = dest;

	if (count)
	{
		while (*dest)
		{
			dest++;
		}
		while ((*dest++ = *src++) != 0)
		{
			if (--count == 0)
			{
				*dest = '\0';
				break;
			}
		}
	}
	return tmp;
}
static int __sram __mdfs_print_align(char *string, int len, int align)
{
	char fill_ch[] = "                ";
	if (len < align)
	{
		__mdfs_strncat(string, fill_ch, align - len);
		return align - len;
	}
	return 0;
}
static char __sramdata mdfs_debugger_buffer[256];
int __sram mdfs_printk(const char *format, ...)
{
	va_list args;
	char string[16];
	char *pdest, *psrc;
	int align, len = 0;

	pdest = mdfs_debugger_buffer;
	va_start(args, format);
	while(*format)
	{
		if(*format == '%')
		{
			++format;
			if (('0' < (*format)) && ((*format) <= '9'))
			{
				align = *format - '0';
				++format;
			}
			else
			{
				align = 0;
			}
			switch(*format)
			{
				case 'd':
				{
					__mdfs_itoa(va_arg(args, int), string, 10);
					len = __mdfs_strlen(string);
					len += __mdfs_print_align(string, len, align);
					__mdfs_strcpy(pdest, string);
					pdest += len;
					break;
				}
				case 'x':
				case 'p':
				{
					__mdfs_utoa(va_arg(args, int), string, 16);
					len = __mdfs_strlen(string);
					len += __mdfs_print_align(string, len, align);
					__mdfs_strcpy(pdest, string);
					pdest += len;
					break;
				}
				case 'u':
				{
					__mdfs_utoa(va_arg(args, int), string, 10);
					len = __mdfs_strlen(string);
					len += __mdfs_print_align(string, len, align);
					__mdfs_strcpy(pdest, string);
					pdest += len;
					break;
				}
				case 'c':
				{
					*pdest = (char)va_arg(args, int);
					break;
				}
				case 's':
				{
					psrc = va_arg(args, char *);
					__mdfs_strcpy(pdest, psrc);
					pdest += __mdfs_strlen(psrc);
					break;
				}
				default :
				{
					*pdest++ = '%';
					*pdest++ = *format;
				}
			}
		}
		else
		{
			*pdest++ = *format;
		}
		++format;
	}
	va_end(args);

	*pdest = '\0';
	pdest++;
	__mdfs_serial_puts(mdfs_debugger_buffer);

	return (pdest - mdfs_debugger_buffer);
}
#endif /* __MDFS_DEBUG */

void __sram mdfs_memcpy(void *dest, const void *src, int n)
{
	char *tmp = dest;
	const char *s = src;

	if (!dest || !src)
		return;

	while (n--)
		*tmp++ = *s++;

	return;
}

__u32 __sram load_pte_to_tlb(__u32 addr, __u32 len)
{
	__u32 tmp_addr  = addr;
	__u32 tmp_value = 0;

	while (tmp_addr < (addr + len))
	{
		tmp_value = *((__u32 *)tmp_addr);

		tmp_addr += 4*1024;
	}
	return tmp_value;
}

u64 __sram cpucfg_counter_read(void)
{
	volatile __u32 high;
	volatile __u32 low;
	u64          counter;

	low = mctl_read_w(CNT64_CTRL_REG);
	low |= (1<<1);
	mctl_write_w(CNT64_CTRL_REG, low);
	while(mctl_read_w(CNT64_CTRL_REG) & (1<<1));

	low  = mctl_read_w(CNT64_LOW_REG);
	high = mctl_read_w(CNT64_HIGH_REG);

	counter = high;
	counter = (counter << 32) + low;

	return counter;
}

void __sram time_udelay(__u32 us)
{
	u64 expire = 0;

	if (us == 0) {
		return;
	}

	expire = (us * 24) + cpucfg_counter_read();
	while (expire > cpucfg_counter_read()) {
		;
	}
}

void __sram mctl_self_refresh_entry(void)
{
	unsigned int reg_val;

	mctl_write_w(MX_DBG, 1);

	mctl_write_w(MX_PWRCTL, 0);

	reg_val = 0x1;
	mctl_write_w(MX_RFSHCTL3, reg_val);

	reg_val = mctl_read_w(MX_PWRCTL);
	reg_val |= 0x1<<0;
	mctl_write_w(MX_PWRCTL, reg_val);

	while(((mctl_read_w(MX_STATR)&0x7) != 0x3)){}

	reg_val = mctl_read_w(VDD_SYS_PWROFF_GATING);
	reg_val |= (0x1<<0);
	mctl_write_w(VDD_SYS_PWROFF_GATING, reg_val);

	while((mctl_read_w(VDD_SYS_PWROFF_GATING) & 0x1) != 1){}
}

void __sram mctl_deep_sleep_entry(__dram_para_t *para)
{
#if 0      
	unsigned int reg_val, i;

	for(i=0; i<16; i++)
		zqdqssave[i] = mctl_read_w(0xc0000000 + 4*i);

	reg_val = mctl_read_w(MP_PGCR(0));
	reg_val &= ~(0x3F<<0);
	mctl_write_w(MP_PGCR(0), reg_val);


	zqdqssave[30] = ((mctl_read_w(MP_DXMDLR(0)) & 0xFF) | (para->high_freq <<16));
	zqdqssave[31] = ((mctl_read_w(MP_DXMDLR(1)) & 0xFF) | (para->high_freq <<16));

	mctl_self_refresh_entry();

	reg_val = mctl_read_w(MC_CCR);
	reg_val &= ~(1<<19);
	mctl_write_w(MC_CCR, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val &= ~(0x1u<<31);
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val |= (0x1u<<16);
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	while(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<16)){}

	{
		reg_val = ((para->dram_clk/24 - 1)<<8);        
		reg_val |= 0<<4;                               
		reg_val |= (2-1)<<0;                           
		reg_val |= (0x1u<<31);
	}
	if( para->dram_tpr13 & (0xf<<18) )
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val |= 0x1<<20;
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	time_udelay(2000);

	while(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<20)){}

	while(!(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<28))){}


	reg_val = mctl_read_w(MC_CCR);
	reg_val |= (1<<19);
	mctl_write_w(MC_CCR, reg_val);


	if(para->dram_clk > 480)
	{
		mctl_write_w(MP_DLLGCR, 0x5c000);
	}
	else
	{
		mctl_write_w(MP_DLLGCR, 0xdc000);
	}

	if( (para->dram_clk <= 480) || ((para->dram_odt_en&0x1) == 0) )
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val &= ~(0x3<<9);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val &= ~(0x3<<9);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}
	else
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val |= (0x3<<9);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val |= (0x3<<9);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}

	if(para->dram_clk <= 480)
	{
		mctl_write_w(MP_ODTCR, 0);
		mctl_write_w(MX_ODTMAP, 0);
	}
	else
	{
		mctl_write_w(MP_ODTCR, 0x10000);
		mctl_write_w(MX_ODTMAP, 0x11);
	}

	if((para->dram_tpr13 & (1<<16)) == 0)
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val &= ~(0x7<<11);
		reg_val |= (0x4<<11);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val &= ~(0x7<<11);
		reg_val |= (0x4<<11);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}

	if((para->dram_tpr13 & (1<<17)) == 0)
	{
		mctl_write_w(MX_PWRCTL, 0x2);
		mctl_write_w(MX_PWRTMG, 0x8001);
	}

	if(para->dram_clk > 480)   
	{
		reg_val = 0 | 0x40000071;
		mctl_write_w(MP_PIR, reg_val);

		time_udelay(1);
		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);
	}
	else
	{
		reg_val = zqdqssave[30] & 0xFF;
		reg_val *= 552;
		reg_val /= (para->dram_clk * 2);
		reg_val &= 0xFF;
		mctl_write_w(MP_DXLCDLR1(0), reg_val | (reg_val<<8));

		reg_val = zqdqssave[31] & 0xFF;
		reg_val *= 552;
		reg_val /= (para->dram_clk * 2);
		reg_val &= 0xFF;
		mctl_write_w(MP_DXLCDLR1(1), reg_val | (reg_val<<8));

		mctl_write_w(MP_DXLCDLR2(0), 0x000000FF);
		mctl_write_w(MP_DXLCDLR2(1), 0x000000FF);

		reg_val = mctl_read_w(MP_DXGTR(0));
		reg_val &= ~(0x7);
		mctl_write_w(MP_DXGTR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGTR(1));
		reg_val &= ~(0x7);
		mctl_write_w(MP_DXGTR(1), reg_val);
	}

	reg_val = mctl_read_w(VDD_SYS_PWROFF_GATING);
	reg_val &= ~(0x1<<0);
	mctl_write_w(VDD_SYS_PWROFF_GATING, reg_val);

	while((mctl_read_w(VDD_SYS_PWROFF_GATING) & 0x1) != 0){}


	reg_val = mctl_read_w(MX_PWRCTL);
	reg_val &= ~(0x1<<0);
	mctl_write_w(MX_PWRCTL, reg_val);

	while(((mctl_read_w(MX_STATR)&0x7) != 0x1)){}

	time_udelay(1000);

	if(para->dram_clk >= 480)
	{
		reg_val = mctl_read_w(MP_PGCR(0));
		reg_val |= (0x1<<25);
		mctl_write_w(MP_PGCR(0), reg_val);

		time_udelay(1);

		reg_val = 0 |  0x401;  
		mctl_write_w(MP_PIR, reg_val);

		time_udelay(1);

		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);

		reg_val = mctl_read_w(MP_PGCR(0));
		reg_val &= ~(0x1<<25);
		reg_val |= 0x3F;
		mctl_write_w(MP_PGCR(0), reg_val);

		reg_val = 0x0;
		mctl_write_w(MX_RFSHCTL3, reg_val);

		reg_val = mctl_read_w(MP_PGSR(0));
		if(reg_val & (0x1<<22))
		{
			MDFS_DBG("[DRAM]MP_PGSR(0) = 0x%x\n", reg_val);
		}
	}
	else
	{
		reg_val = 0x0;
		mctl_write_w(MX_RFSHCTL3, reg_val);
	}


	mctl_write_w(MX_DBG, 0);

	for(i=0; i<16; i++)
		mctl_write_w(0xc0000000 + 4*i, zqdqssave[i]);

	return;

#else      
	unsigned int reg_val, i,n,m,k;

	for(i=0; i<16; i++)
		zqdqssave[i] = mctl_read_w(0xc0000000 + 4*i);

	reg_val = mctl_read_w(MP_PGCR(0));
	reg_val &= ~(0x3F<<0);
	mctl_write_w(MP_PGCR(0), reg_val);

	zqdqssave[30] = ((mctl_read_w(MP_DXMDLR(0)) & 0xFF) | (para->high_freq <<16));
	zqdqssave[31] = ((mctl_read_w(MP_DXMDLR(1)) & 0xFF) | (para->high_freq <<16));

	mctl_self_refresh_entry();

	reg_val = mctl_read_w(MC_CCR);
	reg_val &= ~(1<<19);
	mctl_write_w(MC_CCR, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val &= ~(0x1u<<31);
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val |= (0x1u<<20);
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	while(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<20)){}

	if (para->dram_clk <= 240)     
	{
		m = 1;
		n = para->dram_clk/6;      
		k = 1;
	}
	else                                                   
	{
		m = 2;
		n = para->dram_clk/24;     
		k = 1;
	}
	reg_val = ((n - 1)<<8) | ((k - 1)<<4) | ((m - 1)<<0) | (0x1u<<31);
	if( para->dram_tpr13 & (0xf<<18) )//------------------------------------------------------------------------------------------------------------------------------
	reg_val|=(0x1<<24);
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_PLL_DDR_CTRL);
	reg_val |= 0x1<<20;
	mctl_write_w(CCM_PLL_DDR_CTRL, reg_val);

	time_udelay(4000);

	while(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<20)){}

	while(!(mctl_read_w(CCM_PLL_DDR_CTRL) & (0x1<<28))){}


	if (para->dram_clk <= 240)
	{
		reg_val = mctl_read_w(MX_RFSHTMG);
		reg_val&=(~(0xfff<<16));
		reg_val |= ((((para->dram_tpr10>>0)&0xFFF)>>2)<<16);
		mctl_write_w(MX_RFSHTMG, reg_val);
	}
	else
	{
		reg_val = mctl_read_w(MX_RFSHTMG);
		reg_val&=(~(0xfff<<16));
		reg_val |= ((((para->dram_tpr10>>0)&0xFFF))<<16);
		mctl_write_w(MX_RFSHTMG, reg_val);
	}
	if(para->dram_clk < 240)   
	{

		reg_val = mctl_read_w(MC_CCR);
		reg_val &= ~(0xffff);
		reg_val |= 0x7<<16;
		mctl_write_w(MC_CCR, reg_val);

		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val |= ((0x1u<<31) | (0x1<<29));
		mctl_write_w(MP_DLLGCR, reg_val);

		time_udelay(1);

		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val |= (0x1<<30);
		mctl_write_w(MP_DLLGCR, reg_val);

		time_udelay(1);

		reg_val = mctl_read_w(MC_CCR);
		reg_val &= ~(1<<22);
		mctl_write_w(MC_CCR, reg_val);

		time_udelay(1);

		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val &= ~(0x1<<30);
		mctl_write_w(MP_DLLGCR, reg_val);

		time_udelay(1);

		reg_val = mctl_read_w(MC_CCR);
		reg_val |= (1<<19);
		mctl_write_w(MC_CCR, reg_val);

		reg_val = mctl_read_w(MC_CCR);
		reg_val |= (1<<22);
		mctl_write_w(MC_CCR, reg_val);

		time_udelay(1);
	}
	else
	{
		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val &= ~((0x1<<29) | (0x1u<<31));
		mctl_write_w(MP_DLLGCR, reg_val);

		reg_val = mctl_read_w(MC_CCR);
		reg_val &= ~(0x7<<16);
		mctl_write_w(MC_CCR, reg_val);

		reg_val = mctl_read_w(MC_CCR);
		reg_val &= ~(0x1<<22);
		mctl_write_w(MC_CCR, reg_val);

		reg_val = mctl_read_w(MC_CCR);
		reg_val |= (1<<19);
		mctl_write_w(MC_CCR, reg_val);

		time_udelay(1);
	}

	if(para->dram_clk > 360)//if(para->dram_clk > 480)
	{
		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val &= ~(0x3<<18);
		reg_val |= 0x1<<18;
		mctl_write_w(MP_DLLGCR, reg_val);
	}
	else
	{
		reg_val = mctl_read_w(MP_DLLGCR);
		reg_val &= ~(0x3<<18);
		reg_val |= 0x3<<18;
		mctl_write_w(MP_DLLGCR, reg_val);
	}

	if( (para->dram_clk <= 360) || ((para->dram_odt_en&0x1) == 0) )
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val &= ~(0x3<<9);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val &= ~(0x3<<9);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}
	else
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val |= (0x3<<9);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val |= (0x3<<9);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}

	if(para->dram_clk <= 360)//if(para->dram_clk <= 480)
	{
		mctl_write_w(MP_ODTCR, 0);
		mctl_write_w(MX_ODTMAP, 0);
	}
	else
	{
		mctl_write_w(MP_ODTCR, 0x10000);
		mctl_write_w(MX_ODTMAP, 0x11);
	}

	if((para->dram_tpr13 & (1<<16)) == 0)
	{
		reg_val = mctl_read_w(MP_DXGCR(0));
		reg_val &= ~(0x7<<11);
		reg_val |= (0x4<<11);
		mctl_write_w(MP_DXGCR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGCR(1));
		reg_val &= ~(0x7<<11);
		reg_val |= (0x4<<11);
		mctl_write_w(MP_DXGCR(1), reg_val);
	}

	if((para->dram_tpr13 & (1<<17)) == 0)
	{
		mctl_write_w(MX_PWRCTL, 0x2);
		mctl_write_w(MX_PWRTMG, 0x8001);
	}

	if(para->dram_para2 & 0xF)
	{
		mctl_write_w(MP_DXGCR(1), 0);
		reg_val = mctl_read_w(MX_MSTR);
		reg_val |= 0x1<<12;
		mctl_write_w(MX_MSTR, reg_val);
	}

	if(para->dram_clk > 360)   
	{
		reg_val = 0 | 0x40000071;
		mctl_write_w(MP_PIR, reg_val);

		time_udelay(20);
		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);
	}
	else
	{
		reg_val = 0 | 0x40020061;//enable DDL CAL//reg_val = 0 | 0x40020041;
		mctl_write_w(MP_PIR, reg_val);

		time_udelay(20);

		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);

		reg_val = mctl_read_w(MP_DXGTR(0));
		reg_val &= ~(0x7);
		mctl_write_w(MP_DXGTR(0), reg_val);

		reg_val = mctl_read_w(MP_DXGTR(1));
		reg_val &= ~(0x7);
		mctl_write_w(MP_DXGTR(1), reg_val);

		time_udelay(10);
	}

	reg_val = mctl_read_w(VDD_SYS_PWROFF_GATING);
	reg_val &= ~(0x1<<0);
	mctl_write_w(VDD_SYS_PWROFF_GATING, reg_val);

	while((mctl_read_w(VDD_SYS_PWROFF_GATING) & 0x1) != 0){}
	reg_val = mctl_read_w(MC_CCR);
	reg_val |= (1<<19);
	mctl_write_w(MC_CCR, reg_val);

	time_udelay(10);

	reg_val = mctl_read_w(MX_PWRCTL);
	reg_val &= ~(0x1<<0);
	mctl_write_w(MX_PWRCTL, reg_val);

	while(((mctl_read_w(MX_STATR)&0x7) != 0x1)){}

	time_udelay(1500);

	if(para->dram_clk >= 360)//if(para->dram_clk >= 480)
	{
		 reg_val = mctl_read_w(MP_PGCR(0));
		 reg_val |= (0x1<<25);
		 mctl_write_w(MP_PGCR(0), reg_val);

		 time_udelay(10);

		reg_val = 0 |  0x401;  
		mctl_write_w(MP_PIR, reg_val);

		time_udelay(10);

		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);

		reg_val = mctl_read_w(MP_PGCR(0));
		reg_val &= ~(0x1<<25);
		reg_val |= 0x3F;
		mctl_write_w(MP_PGCR(0), reg_val);

		reg_val = 0x0;
		mctl_write_w(MX_RFSHCTL3, reg_val);

		reg_val = mctl_read_w(MP_PGSR(0));
		if(reg_val & (0x1<<22))
		{
			MDFS_DBG("[DRAM]MP_PGSR(0) = 0x%x\n", reg_val);
		}
	}
	else
	{
		reg_val = 0 |  0x20401;
		mctl_write_w(MP_PIR, reg_val);
		time_udelay(10);
		while((mctl_read_w(MP_PGSR(0))&0x1) != 0x1);
		reg_val = 0x0;
		mctl_write_w(MX_RFSHCTL3, reg_val);

		time_udelay(10);
	}


	mctl_write_w(MX_DBG, 0);

	time_udelay(5);

	for(i=0; i<16; i++)
		mctl_write_w(0xc0000000 + 4*i, zqdqssave[i]);

	return;
#endif
}

int __sram mdfs_main(__dram_para_t *softfreq_switch)
{
	MDFS_DBG("%s enter\n", __func__);

	/* save stack pointer registger, switch stack to sram */
	DDR_SAVE_SP(sp_backup);
	/* load tlb */
	load_pte_to_tlb(0xf0000000, 0x2000);
	load_pte_to_tlb(0xf1c62000, 0x5000);
	load_pte_to_tlb(0xf1c20000, 0x1000);
	load_pte_to_tlb(0xf1f01000, 0x1000);

	/* copy softfreq_switch info to sram space */
	mdfs_memcpy(&softfreq_info, softfreq_switch, sizeof(__dram_para_t));
	/* start softfreq_switch */
	mctl_deep_sleep_entry(&softfreq_info);

	/* restore stack pointer */
	DDR_RESTORE_SP(sp_backup);
	MDFS_DBG("%s done\n", __func__);

	return 0;
}
