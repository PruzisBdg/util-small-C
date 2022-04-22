# ------------------------------------------------------------------
#
# TDD makefile timedate lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = weekdate
TARGET_BASE_DIR = timedate

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)days_to_weekdate.c \
								$(SRCDIR)weekdate_to_days.c \
								$(SRCDIR)legal_weekdate.c \
								$(SRCDIR)weekdate_A_GT_B.c \
								$(SRCDIR)weekdate_A_LT_B.c \
								$(SRCDIR)weekdate_equal.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

