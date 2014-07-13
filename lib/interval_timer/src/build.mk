# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Define the prefix to this directory.
# Note: The name must be unique within this build and should be
#       based on the root of the project
TARGET_TIMER_PATH = lib/interval_timer
TARGET_TIMER_SRC_PATH = $(TARGET_TIMER_PATH)/src

# Add include paths.
INCLUDE_DIRS += $(TARGET_TIMER_PATH)/inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(TARGET_TIMER_SRC_PATH)/interval_timer.cpp

# ASM source files included in this build.
ASRC +=