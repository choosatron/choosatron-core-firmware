# Choosatron README

From ./main (makes ./main/applications/choosatron-core-firmware)

make v=1 PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=104 APP=choosatron TARGET_FILE=cdam-core-firmware GCC_ARM_PATH=/Users/jerry/Code/Particle/gcc-arm-none-eabi-4_8-2013q4/bin/ clean all

make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=104 APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build GCC_ARM_PATH=/Users/jerry/Code/Particle/gcc-arm-none-eabi-4_8-2013q4/bin/ DEBUG_BUILD=y v=1 program-dfu

IF YOU WANT BUSINESS CARDS
make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=104 APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build GCC_ARM_PATH=/Users/jerry/Code/Particle/gcc-arm-none-eabi-4_8-2013q4/bin/ BIZ_CARD=y v=1 program-dfu

dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D firmware.bin

dfu-util -d 1d50:607f -a 1 -s 0x80000 -v -D stories.bin

applications/choosatron-core-firmware/releases/choosatron-flash-09-kids.bin

HS Tariff #: 9504.90.40