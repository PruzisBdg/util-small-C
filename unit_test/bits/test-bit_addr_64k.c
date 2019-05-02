#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* ----------------------------------- test_bit64K_MakeBE ---------------------------------------- */

void test_bit64K_MakeBE(void)
{
   typedef struct {U16 _byte; U8 _bit; U16 res; } S_Tst;

   S_Tst const tsts[] = {
      {._byte = 0,      ._bit = 7,     .res = 0  },
      {._byte = 0,      ._bit = 6,     .res = 1  },
      {._byte = 0,      ._bit = 0,     .res = 7  },

      {._byte = 1,      ._bit = 7,     .res = 8  },
      {._byte = 1,      ._bit = 6,     .res = 9  },
      {._byte = 1,      ._bit = 5,     .res = 0xA  },
      {._byte = 1,      ._bit = 4,     .res = 0xB  },
      {._byte = 1,      ._bit = 3,     .res = 0xC  },
      {._byte = 1,      ._bit = 2,     .res = 0xD  },
      {._byte = 1,      ._bit = 1,     .res = 0xE  },
      {._byte = 1,      ._bit = 0,     .res = 0xF },

      {._byte = 1,      ._bit = 8,     .res = 0x17  },
      {._byte = 1,      ._bit = 9,     .res = 0x16  },
      {._byte = 1,      ._bit = 0xA,   .res = 0x15  },
      {._byte = 1,      ._bit = 0xB,   .res = 0x14  },
      {._byte = 1,      ._bit = 0xC,   .res = 0x13  },
      {._byte = 1,      ._bit = 0xD,   .res = 0x12  },
      {._byte = 1,      ._bit = 0xE,   .res = 0x11  },
      {._byte = 1,      ._bit = 0xF,   .res = 0x10  },

      {._byte = 1,      ._bit = 0x10,  .res = 0x1F  },
      {._byte = 1,      ._bit = 0x11,  .res = 0x1E  },
      {._byte = 1,      ._bit = 0x12,  .res = 0x1D  },
      {._byte = 1,      ._bit = 0x13,  .res = 0x1C  },
      {._byte = 1,      ._bit = 0x14,  .res = 0x1B  },
      {._byte = 1,      ._bit = 0x15,  .res = 0x1A  },
      {._byte = 1,      ._bit = 0x16,  .res = 0x19  },
      {._byte = 1,      ._bit = 0x17,  .res = 0x18  },

      {._byte = 1,      ._bit = 0x18,  .res = 0x27  },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U16 rtn = bit64K_MakeBE(t->_byte, t->_bit);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x,0x%02x) -> 0x%02x", i, t->_byte, t->_bit, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }


}
// ====================================== EOF ==============================================
