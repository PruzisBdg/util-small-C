# ------------------------------------------------------------------
#
# TDD makefile bits lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = bits
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)idx_to_1st_bit_set_u16.c \
								$(SRCDIR)idx_to_1st_bit_set_u32.c \
								$(SRCDIR)num_bits_set_u16.c \
								$(SRCDIR)num_bits_set_u32.c \
								$(SRCDIR)swar_32.c \
								$(SRCDIR)swar_u8.c \
								$(SRCDIR)make_masks.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

