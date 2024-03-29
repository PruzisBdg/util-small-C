# ------------------------------------------------------------------
#
# TDD makefile bits lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = endians
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the utils.
include ../util_common_pre.mak

# Add 64 bit support to test leToU64() etc
CFLAGS := $(CFLAGS) -DUNITY_SUPPORT_64

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)to_sys_endian_u16.c \
								$(SRCDIR)to_sys_endian_u32.c \
								$(SRCDIR)endians_aligns.c \
								$(SRCDIR)reverse_in_place.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

