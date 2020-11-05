# ------------------------------------------------------------------
#
# TDD makefile for Basic Integer lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = ascii_num
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the arithmetic.
include ../util_common_pre.mak

SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
							$(SRCDIR)read_dirty_int.c \
							$(SRCDIR)read_thruto_int.c \
							$(SRCDIR)read_dirty_s32.c \
							$(SRCDIR)read_dirty_int_pass_char.c \
							$(SRCDIR)read_next_float.c \
							$(SRCDIR)read_next_number.c \
							$(SRCDIR)read_tagged_int.c \
							$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

