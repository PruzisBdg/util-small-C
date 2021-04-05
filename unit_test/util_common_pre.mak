# --------------------------------------------------------------------------
#
# Util Library: TDD Unit Tests, common defs.
#
# ---------------------------------------------------------------------------

# Import TDD harness defs
include ../../../unity_tdd/tdd_common_pre_build.mak

# (Additional) compiler flags
CFLAGS := $(CFLAGS) -DGNRC_UNIT_TEST -D__COMPILER_IS_GENERIC__ -D__SYSTEM_IS_ANY__ -D__TARGET_IS_UNITY_TDD

# File of tests is here.
SRCDIR = ../../src/$(TARGET_BASE_DIR)/

# Most test harnesses need arith support.
INTEGER_BASIC_FULLPATH = $(SRCDIR)arith_integer_basic$(CEXT)

OUT_FILE = -o $(TARGET)

# All util tests reference the same includes
INC_DIRS := $(INC_DIRS) -I. -I../../src -I$(SPJ_SWR_LOC)/arith/public

# --------------------------------- eof ------------------------------------


