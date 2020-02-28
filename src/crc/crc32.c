#include "util.h"

static const crc32_S_Cfg dfltCRC32 =
{
	.poly = 0xEDB88320,
	.initRemainder = 0xFFFFFFFF,
	.finalXor = 0xFFFFFFFF,
};


PUBLIC U32 crc32_Init(crc32_S_Cfg const *cfg)
{
	if(cfg == NULL)
	{
		/* Use default config if none supplied */
		cfg = (crc32_S_Cfg*) &dfltCRC32;
	}
	return cfg->initRemainder;
}

PUBLIC U32 crc32_Update(crc32_S_Cfg const *cfg, U32 crc32, U8 *src, U32 len)
{
	U32 i, j;
	U32 msg;

	if(cfg == NULL)
	{
		/* Use default config if none supplied */
		cfg = (crc32_S_Cfg*) &dfltCRC32;
	}

	for(i = 0 ; i < len ; i++)
	{
		msg = *src++;

		for(j = 0 ; j < 8 ; j++)
		{
			if((msg ^ crc32) & (0x00000001))
				crc32 = (crc32 >> 1) ^ cfg->poly;
			else
				crc32 >>= 1;

			msg >>= 1;
		}
	}
	return crc32;
}

PUBLIC U32 crc32_Finalize(crc32_S_Cfg const *cfg, U32 crc32)
{
	if(cfg == NULL)
	{
		/* Use default config if none supplied */
		cfg = (crc32_S_Cfg*) &dfltCRC32;
	}

	return (crc32 ^ cfg->finalXor);
}

PUBLIC U32 crc32_Block(crc32_S_Cfg const *cfg, U8 *src, U32 len)
{
	return crc32_Finalize(cfg, crc32_Update(cfg, crc32_Init(cfg), src, len));
}

// ---------------------------------------- eof ---------------------------------------------------
