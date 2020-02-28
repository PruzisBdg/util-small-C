#include "util.h"

// Deafult CRC-16 configuration (XMODEM).
PRIVATE const crc16_S_Cfg dfltCRC16 =
{
	.poly			= 0x1021,
	.seed	= 0x0000,
	.finalXor		= 0x0000,
};


PUBLIC U16 crc16_Block(crc16_S_Cfg* cfg, U8 const * src, U16 numBytes)
{
	if(cfg == NULL)
	{
		/* Use default config if none supplied */
		cfg = (crc16_S_Cfg*)&dfltCRC16;
	}

	U16 msg;
	U16 crc = cfg->seed;

	for(U32 i = 0; i < numBytes; i++)
	{
		msg = (*src++ << 8);

		for(U32 j = 0; j < 8; j++)
		{
			if((msg ^ crc) >> 15)
			{
				crc = (crc << 1) ^ cfg->poly;
			}
			else
			{
				crc <<= 1;
			}

			msg <<= 1;
		}
	}

	crc ^= cfg->finalXor;

	return crc;
}

// --------------------------------------------- eof ----------------------------------------------------------
