# ------------------------------------------------------------------
#
# TDD makefile WordList lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = tiny1_sprintf
TARGET_BASE_DIR = printfs

# Defs common to the utils.
include ../util_common_pre.mak

CFLAGS := $(CFLAGS) -DTPRINT_IS_FLOAT

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)tiny1_printf.c \
								tiny1_printf_test_support.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

