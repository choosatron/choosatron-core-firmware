# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Define the prefix to this directory. 
# Note: The name must be unique within this build and should be
#       based on the root of the project
TARGET_CHOOSATRON_PATH = src
TARGET_CHOOSATRON_SRC_PATH = src

# Add include paths.
INCLUDE_DIRS += ../core-firmware/inc
INCLUDE_DIRS += $(TARGET_CHOOSATRON_PATH)

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/choosatron.cpp
CPPSRC += $(TARGET_CHOOSATRON_SRC_PATH)/thermal_printer.cpp

# ASM source files included in this build.
ASRC +=
