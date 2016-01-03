# Choosatron README

make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=101 APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build DEBUG_BUILD=y v=1 program-dfu


dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D firmware.bin

dfu-util -d 1d50:607f -a 1 -s 0x80000 -v -D stories.bin