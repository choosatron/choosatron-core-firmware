# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Add include paths.
INCLUDE_DIRS += applications/choosatron-core-v1/inc
INCLUDE_DIRS += applications/choosatron-core-v1/lib/elapsed_time/inc
INCLUDE_DIRS += applications/choosatron-core-v1/lib/memory_free/inc
INCLUDE_DIRS += applications/choosatron-core-v1/lib/sd/inc
INCLUDE_DIRS += applications/choosatron-core-v1/lib/flashee/inc
INCLUDE_DIRS += applications/choosatron-core-v1/lib/ymodem/inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(call target_files,./applications/choosatron-core-v1/,*.cpp)

# ASM source files included in this build.
ASRC +=