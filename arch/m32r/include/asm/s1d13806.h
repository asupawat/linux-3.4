

#define SWIVEL_VIEW		0	/* 0:none, 1:90 not completed */

static struct s1d13xxxfb_regval s1d13xxxfb_initregs[] = {

    {0x0001,0x00},  
    {0x01FC,0x00},  
#if defined(CONFIG_PLAT_MAPPI)
    {0x0004,0x00},  
    {0x0005,0x00},  
    {0x0008,0x00},  
    {0x0009,0x00},  
    {0x0010,0x00},  
    {0x0014,0x00},  
    {0x0018,0x00},  
    {0x001C,0x00},  
/*
 * .. 10MHz: 0x00
 * .. 30MHz: 0x01
 * 30MHz ..: 0x02
 */
    {0x001E,0x02},  
    {0x0021,0x02},  
    {0x002A,0x11},  
    {0x002B,0x13},  
    {0x0020,0x80},  
    {0x0030,0x25},  
    {0x0031,0x00},  
    {0x0032,0x4F},  
    {0x0034,0x12},  
    {0x0035,0x01},  
    {0x0036,0x0B},  
    {0x0038,0xDF},  
    {0x0039,0x01},  
    {0x003A,0x2C},  
    {0x003B,0x0A},  
    {0x003C,0x01},  

    {0x0041,0x00},  
    {0x0042,0x00},  
    {0x0043,0x00},  
    {0x0044,0x00},  

#elif defined(CONFIG_PLAT_M32700UT) || defined(CONFIG_PLAT_OPSPUT) || defined(CONFIG_PLAT_MAPPI3)
    {0x0004,0x07},  
    {0x0005,0x00},  
    {0x0008,0x00},  
    {0x0009,0x00},  
    {0x0008,0x04},  
    {0x0008,0x05},  
    {0x0010,0x01},  
    {0x0014,0x30},  
    {0x0018,0x00},  
    {0x001C,0x00},  
    {0x001E,0x00},  
    {0x0020,0x80},  
    {0x0021,0x03},  
    {0x002A,0x00},  
    {0x002B,0x01},  
    {0x0030,0x25},  
    {0x0031,0x00},  
    {0x0032,0x1d},  
    {0x0034,0x05},  
    {0x0035,0x01},  
    {0x0036,0x01},  
    {0x0038,0x3F},  
    {0x0039,0x01},  
    {0x003A,0x0b},  
    {0x003B,0x07},  
    {0x003C,0x02},  

    {0x0041,0x00},  
#if (SWIVEL_VIEW == 0)
    {0x0042,0x00},  
    {0x0043,0x00},  
    {0x0044,0x00},  

#elif (SWIVEL_VIEW == 1)
    {0x0042,0xC0},  
    {0x0043,0x02},  
    {0x0044,0x00},  
    {0x0046,0x00},  
    {0x0047,0x02},  
#else
#error unsupported SWIVEL_VIEW mode
#endif
#else
#error no platform configuration
#endif  /* CONFIG_PLAT_XXX */

    {0x0048,0x00},  
    {0x004A,0x00},  
    {0x004B,0x00},  
    {0x0050,0x4F},  
    {0x0052,0x13},  
    {0x0053,0x01},  
    {0x0054,0x0B},  
    {0x0056,0xDF},  
    {0x0057,0x01},  
    {0x0058,0x2B},  
    {0x0059,0x09},  
    {0x005A,0x01},  
    {0x005B,0x10},  

    {0x0062,0x00},  
    {0x0063,0x00},  
    {0x0064,0x00},  

    {0x0068,0x00},  
    {0x006A,0x00},  
    {0x006B,0x00},  
    {0x0070,0x00},  
    {0x0071,0x01},  
    {0x0072,0x00},  
    {0x0073,0x00},  
    {0x0074,0x00},  
    {0x0075,0x00},  
    {0x0076,0x00},  
    {0x0077,0x00},  
    {0x0078,0x00},  
    {0x007A,0x1F},  
    {0x007B,0x3F},  
    {0x007C,0x1F},  
    {0x007E,0x00},  
    {0x0080,0x00},  
    {0x0081,0x01},  
    {0x0082,0x00},  
    {0x0083,0x00},  
    {0x0084,0x00},  
    {0x0085,0x00},  
    {0x0086,0x00},  
    {0x0087,0x00},  
    {0x0088,0x00},  
    {0x008A,0x1F},  
    {0x008B,0x3F},  
    {0x008C,0x1F},  
    {0x008E,0x00},  
    {0x0100,0x00},  
    {0x0101,0x00},  
    {0x0102,0x00},  
    {0x0103,0x00},  
    {0x0104,0x00},  
    {0x0105,0x00},  
    {0x0106,0x00},  
    {0x0108,0x00},  
    {0x0109,0x00},  
    {0x010A,0x00},  
    {0x010C,0x00},  
    {0x010D,0x00},  
    {0x0110,0x00},  
    {0x0111,0x00},  
    {0x0112,0x00},  
    {0x0113,0x00},  
    {0x0114,0x00},  
    {0x0115,0x00},  
    {0x0118,0x00},  
    {0x0119,0x00},  
    {0x01E0,0x00},  
    {0x01E2,0x00},  
    {0x01F0,0x10},  
    {0x01F1,0x00},  
    {0x01F4,0x00},  
#if (SWIVEL_VIEW == 0)
    {0x01FC,0x01},  
#elif (SWIVEL_VIEW == 1)
    {0x01FC,0x41},  
#else
#error unsupported SWIVEL_VIEW mode
#endif  /* SWIVEL_VIEW */

#if defined(CONFIG_PLAT_M32700UT) || defined(CONFIG_PLAT_OPSPUT) || defined(CONFIG_PLAT_MAPPI3)
    {0x0008,0x07},  
#endif

    {0x0040,0x05},  
#if defined(CONFIG_PLAT_MAPPI)
    {0x0046,0x80},  
    {0x0047,0x02},  
#elif defined(CONFIG_PLAT_M32700UT) || defined(CONFIG_PLAT_OPSPUT) || defined(CONFIG_PLAT_MAPPI3)
    {0x0046,0xf0},  
    {0x0047,0x00},  
#endif
    {0x0060,0x05},  
    {0x0066,0x80},  
    {0x0067,0x02},  
};
