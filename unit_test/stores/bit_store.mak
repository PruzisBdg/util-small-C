# ------------------------------------------------------------------
#
# ---------------------------------------------------------------------

TARGET_BASE = bit_store
TARGET_BASE_DIR = stores

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)bit_store.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

