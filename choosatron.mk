# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Add include paths.
INCLUDE_DIRS += applications/choosatron/inc
INCLUDE_DIRS += applications/choosatron/lib/elapsed_time/inc
INCLUDE_DIRS += applications/choosatron/lib/memory_free/inc
INCLUDE_DIRS += applications/choosatron/lib/sd/inc
INCLUDE_DIRS += applications/choosatron/lib/flashee/inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(call target_files,./applications/choosatron/,*.cpp)

# ASM source files included in this build.
ASRC +=