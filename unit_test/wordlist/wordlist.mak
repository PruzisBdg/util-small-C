# ------------------------------------------------------------------
#
# TDD makefile WordList lib
#
# ---------------------------------------------------------------------

# Code folder, test folder and test file all get same name.
TARGET_BASE = wordlist
TARGET_BASE_DIR = $(TARGET_BASE)

# Defs common to the utils.
include ../util_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) $(UNITYDIR)unity.c \
								$(SRCDIR)str_find_word.c \
								$(SRCDIR)str_1st_word_has_char.c \
								$(SRCDIR)str_word_char_cnt.c \
								$(SRCDIR)str_word_cnt.c \
								$(SRCDIR)str_word_in_str.c \
								$(SRCDIR)str_words_in_str.c \
								$(SRCDIR)str_get_end_word.c \
								$(SRCDIR)str_insert.c \
								$(SRCDIR)str_delete.c \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../util_common_build.mak

# ------------------------------- eof ------------------------------------

