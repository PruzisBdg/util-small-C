# ------------------------------------------------------------------
#
# TDD makefile timedate lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = timedate
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)ISO8601_Str_to_secs.c \
								$(SRCDIR)legal_YMDHMS.c \
								$(SRCDIR)Secs_to_HMS.c \
								$(SRCDIR)secs_to_hms_str.c \
								$(SRCDIR)secs_to_hr_min_str.c \
								$(SRCDIR)secs_to_YMDHMS.c \
								$(SRCDIR)secs_to_ymdhms_str.c \
								$(SRCDIR)ymdhms_to_secs.c \
								$(SRCDIR)ymdhms_to_str.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

