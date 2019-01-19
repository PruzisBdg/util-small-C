# ------------------------------------------------------------------
#
# TDD makefile for Basic Integer lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = network
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the arithmetic.
include ../util_common_pre.mak

SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c $(SRCDIR)ip_addr.c $(SRCDIR)mac_addr.c $(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

