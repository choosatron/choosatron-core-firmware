# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Define the prefix to this directory.
# Note: The name must be unique within this build and should be
#       based on the root of the project
TARGET_CDAM_PRINTER_PATH = lib/thermal_printer
TARGET_CDAM_PRINTER_SRC_PATH = $(TARGET_CDAM_PRINTER_PATH)/src

# Add include paths.
INCLUDE_DIRS += $(TARGET_CDAM_PRINTER_PATH)/inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
#CPPSRC += $(TARGET_CDAM_PRINTER_SRC_PATH)/csna5_thermal_printer.cpp

# ASM source files included in this build.
ASRC +=
