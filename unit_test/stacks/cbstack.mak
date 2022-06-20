# ------------------------------------------------------------------
#
# S2LP Radio Driver; Radio support Test harness.
#
# ---------------------------------------------------------------------

# Test folder and test file all get same name.
TARGET_BASE = cbstack
# Code folder is $(ProjDir)/src itself, not any subdir.
TARGET_BASE_DIR = stacks

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
# Some of these files are included, even though they are built into libraries, because the linker
# can't resolve some references without them.
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)callbacks_stack.c \
								$(SPJ_SWR_LOC)/util/src/printfs/array_print_line.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

