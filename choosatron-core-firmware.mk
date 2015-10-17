# This file is a makefile included from the top level makefile which
# defines the sources built for the target.

# Add include paths.
INCLUDE_DIRS += applications/choosatron-core-firmware/inc
INCLUDE_DIRS += applications/choosatron-core-firmware/lib/elapsed_time/inc
INCLUDE_DIRS += applications/choosatron-core-firmware/lib/Memory_Free/inc
INCLUDE_DIRS += applications/choosatron-core-firmware/lib/sd/inc
INCLUDE_DIRS += applications/choosatron-core-firmware/lib/flashee/inc
INCLUDE_DIRS += applications/choosatron-core-firmware/lib/ymodem/inc

# C source files included in this build.
CSRC +=

# C++ source files included in this build.
CPPSRC += $(call target_files,./applications/choosatron-core-firmware/,*.cpp)

# ASM source files included in this build.
ASRC +=