# ------------------------------------------------------------------
#
# TDD makefile print_S_BufC8
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = print_S_BufC8
TARGET_BASE_DIR = printfs

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)print_S_BufC8.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------
