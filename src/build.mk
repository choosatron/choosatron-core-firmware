# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Define the prefix to this directory.
# Note: The name must be unique within this build and should be
#       based on the root of the project
TARGET_CHOOSATRON_SRC_PATH = src

# Add include paths.
INCLUDE_DIRS += inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/choosatron.cpp
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/cdam_printer.cpp
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/cdam_keypad.cpp
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/cdam_story_load.cpp
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/cdam_coin_acceptor.cpp

# ASM source files included in this build.
ASRC +=
