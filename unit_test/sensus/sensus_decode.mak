# ------------------------------------------------------------------
#
# TDD makefile bits lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = sensus_decode
TARGET_BASE_DIR = sensus

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)sensus_decode.c \
								$(SRCDIR)sensus_codec_support.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

