# Choosatron Core Firmware [![Backlog](https://badge.waffle.io/choosatron/choosatron-core-firmware.png?label=backlog&title=backlog)](https://waffle.io/choosatron/choosatron-core-firmware) [![Build Status](https://travis-ci.org/choosatron/choosatron-core-firmware.svg)](https://travis-ci.org/choosatron/choosatron-core-firmware)

This is the main source code repository of the Choosatron firmware for the Spark Core hardware device.

This firmware depends on one other repository: the [Spark Core Firmware](https://github.com/spark/firmware) - specifically the [HAL Feature Branch](https://github.com/spark/firmware/tree/feature/hal). HAL stands for *Hardware Abstraction Layer*, allowing for a single repository to support multiple hardware devices, including their upcoming Spark Photon.

1. [Download and Install Dependencies](#1-download-and-install-dependencies)
2. [Download and Build Repositories](#2-download-and-build-repositories)
3. [Edit and Rebuild](#3-edit-and-rebuild)
4. [Flash It!](#4-flash-it)
 
## 1. Download and Install Dependencies 

1. [GCC for ARM Cortex processors](#1-gcc-for-arm-cortex-processors)
2. [Make](#2-make)
3. [Device Firmware Upgrade Utilities](#3-device-firmware-upgrade-utilities)
4. [Zatig](#4-zatig) (for windows users only)
5. [Git](#5-git)


#### 1. GCC for ARM Cortex processors
The Spark Core uses an ARM Cortex M3 CPU based microcontroller. All of the code is built around the GNU GCC toolchain offered and maintained by ARM.  

Download and install the latest version from: https://launchpad.net/gcc-arm-embedded

See [this Gist](https://gist.github.com/joegoggins/7763637) for how to get setup on OS X.

#### 2. Make 
In order to turn your source code into binaries, you will need a tool called `make`. Windows users need to explicitly install `make` on their machines. Make sure you can use it from the terminal window.

Download and install the latest version from: http://gnuwin32.sourceforge.net/packages/make.htm

#### 3. Device Firmware Upgrade Utilities
Install dfu-util 0.7. Mac users can install dfu-util with [Homebrew](http://brew.sh/) or [Macports](http://www.macports.org), Linux users may find it in their package manager, and everyone can get it from http://dfu-util.gnumonks.org/index.html

#### 4. Zatig
In order for the Core to show up on the dfu list, you need to replace the USB driver with a utility called [Zadig](http://zadig.akeo.ie/). Here is a [tutorial](https://community.spark.io/t/tutorial-installing-dfu-driver-on-windows/3518) on using it. This is only required for Windows users.

#### 5. Git

Download and install Git: http://git-scm.com/

## 2. Download and Build Repositories

The HAL (*hardware abstraction layer*) version of the Spark firmware allows for a single repository dependency. The main firmware is located under *core-firmware*, but we are dependent on the *feature/hal* branch.

This repository, *choosatron-core-firmware*, is what allows us to build Choosatron firmware specifically for the Spark Core hardware.

#### How do we *download* these repositories?
You can access all required repositories via any git interface or download it directly from the website.

The Spark firmware repository is capable of containing and building any number of project firmwares, including the Choosatron, which is why we are naming their base repository genericly. The Choosatron repository will be contained within. Here we made a directory called *Choosatron* and checked out the Spark firmware to *spark-firmware*.

```
D:\Choosatron\spark-firmware
```

*Method 1: Through the git command line interface.*  

Open up a terminal window, navigate to your destination directory, which was *Choosatron* above, and type the following commands:

(Make sure you have git installed on your machine!)

* `git clone https://github.com/spark/firmware.git spark-firmware`
* `cd spark-firmware`
* `git checkout feature/hal`

Next, continuing form the same terminal, we need to checkout the actual Choosatron firmware. The name of the location we checkout to is important, and without other changes *must* be *choosatron-core-firmware*. 

* `cd main`
* `git clone https://github.com/choosatron/choosatron-core-firmware applications/choosatron-core-firmware`

Keep your terminal open, skip ahead to: [How do we *build* these repositories?][How do we *build* these repositories?]

*Method 2: Download the zipped files directly from the Spark and Choosatron's GitHub websites*

* [hal-firmware](https://github.com/spark/firmware/archive/feature/hal.zip)
* [choosatron-core-firmware](https://github.com/choosatron/choosatron-core-firmware/archive/master.zip)

First, unarchive the Spark HAL firmware. Once again, let's say you put it here:

```
D:\Choosatron\spark-firmware
```

Navigate to the following sub-folder:

```
D:\Choosatron\spark-firmware\main\applications
```

Here is where you want to unarchive the Choosatron firmware. Make sure to rename the folder to *choosatron-core-firmware*. It should contain files like this *README.md* directly inside.

```
D:\Choosatron\spark-firmware\main\applications\choosatron-core-firmware\README.md
```

If everything looks right,

#### How do we *build* these repositories?

Make sure you have downloaded and installed all the required dependencies as mentioned [previously.](#1-download-and-install-dependencies). Note, if you've downloaded or cloned these previously, you'll want to `git pull` or redownload all of them before proceeding.

Open up a terminal window, navigate to the main folder under spark-firmware
(i.e. `cd spark-firmware/main`) and type:

    make APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build

This will build the Choosatron firmware repository in `applications/`` and its required dependencies. The Choosatron firmware binary will be placed, in relation to your current location, here: `applications/choosatron-core-firmware/build/choosatron-core-firmware.bin`

You won't see any verbose compiler output for about 20 seconds or so since verbose output is disabled by default and can be enabled with the v=1 switch.

    make APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build v=1

You can adjust behavior with these other options as well:

* If you are having a problem building, or there have been major changes, it can help to build fresh by adding *clean all* to the end of the make command. It will take longer as it is starting from scratch.
* To change where the built firmware is placed, change the `TARGET_DIR` value above to whatever path you like.
* To change the filename of the built firmware, add `TARGET_NAME=[filename]`. You do not need to include a file extention.
* To build the firmware as a Choosatron product (meaning the cloud recognizes your device to be a Choosatron, not just a generic device), you must add `PRODUCT_ID=7`.
 
**WARNING ABOUT PRODUCT_ID:**
The cloud will automatically trigger an over-the-air update if your Choosatron `PRODUCT_ID` is set, **and** if it has a newer version! You should also add `PRODUCT_FIRMWARE_VERSION=999` to your make command.

The three digit number represents the major, minor, and revision of the version. 999 is such a high version number that it won't trigger an OTA update in the forseeable future (the current version being *100*).

##### Common Errors

* `arm-none-eabi-gcc` and other required gcc/arm binaries not in the PATH.
  Solution: Add the /bin folder to your $PATH (i.e. `export PATH="$PATH:<SOME_GCC_ARM_DIR>/bin`).
  Google "Add binary to PATH" for more details.

* You get `make: *** No targets specified and no makefile found.  Stop.`.
  Solution A: `cd spark-firmware/main`.
  Solution B: `APP=choosatron-core-firmware` is in your make command.
  Solution C: The Choosatron repository is properly named `choosatron-core-firmware`.

Please issue a pull request if you come across similar issues/fixes that trip you up.

### Navigating the Choosatron code base

The Choosatron firmware is sub divided into functional folders:

1. `/src` holds all the source code files
2. `/inc` holds all the header files
3. `/lib` holds the source and headers for any libraries used.
4. `/releases` hold pre-compiled firmware for all existing release versions.
5. `/build` is the default destination for the compiled `.bin`, `.hex`, `.elf`, and `.map` files. Only exists if you used the `make` command above with the default `TARGET_DIR` value.

## 3. Edit and Rebuild

Now that you have your hands on the entire Choosatron firmware, its time to start hacking!

### How is the code structured?

**This is a TODO area for us to fill out!**

After you are done editing the files, you can rebuild the repository by running the `make` command listed above in the `spark-firmware/main` directory.

## 4. Flash It!

Its now time to transfer the Choosatron firmware to the Spark Core! You can always do this using the Over The Air update feature or, if you like wires, do it over the USB.

*Make sure you have the `dfu-util` command installed and available through the command line*

#### Steps:
1. Put you Spark Core into the DFU mode by holding down the MODE button on the Core and then tapping on the RESET button once. Release the MODE button after you start to see the RGB LED flashing in yellow. It's easy to get this one wrong: Make sure you don't let go of the left button until you see flashing yellow, about 3 seconds after you release the right/RESET button. A flash of white then flashing green can happen when you get this wrong. You want flashing yellow.

2. Open up a terminal window on your computer and type this command to find out if the Spark Core indeed being detected correctly. 

   `dfu-util -l`   
   you should get the following in return:
   ```
   Found DFU: [1d50:607f] devnum=0, cfg=1, intf=0, alt=0, name="@Internal Flash  /0x08000000/20*001Ka,108*001Kg" 
   Found DFU: [1d50:607f] devnum=0, cfg=1, intf=0, alt=1, name="@SPI Flash : SST25x/0x00000000/512*04Kg"
   ```

   (Windows users will need to use the Zatig utility to replace the USB driver as described earlier)

3. Now, navigate to the build folder in your choosatron-core-firmware repository and use the following command to transfer the *.bin* file into the Spark Core.
   ```
   dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D choosatron-core-firmware.bin
   ```

   For example, this is how my terminal looks like:
   ```
D:\Choosatron\spark-firmware\main\applications\choosatron-core-firmware\build dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D choosatron-core-firmware.bin
   ```
Upon successful transfer, the Spark Core will automatically reset and start the running the program.

##### Common Errors
* As of 12/4/13, you will likely see `Error during download get_status` as the last line from 
the `dfu-util` command. You can ignore this message for now.  We're not sure what this error is all about.

* If you are having trouble with dfu-util, (like invalid dfuse address), try a newer version of dfu-util. v0.7 works well.

**Still having troubles?** Checkout the Spark [resources page](https://www.spark.io/resources), or get help from our community [here.](https://community.choosatron.com/)

### CREDITS AND ATTRIBUTIONS

The Choosatron team: Jerry Belich, Megan Dowd, and Alex Barrett

The entire Spark team.

The firmware uses the GNU GCC toolchain for ARM Cortex-M processors, ARM's CMSIS libraries, TI's CC3000 host driver libraries, STM32 standard peripheral libraries and Arduino's implementation of Wiring.

### LICENSE

Unless stated elsewhere, file headers or otherwise, all files herein are licensed under an LGPLv3 license. For more information, please read the LICENSE file.

### CONTRIBUTE

Want to contribute to the Choosatron project? Follow [this link](https://github.com/choosatron/open-source/blob/master/sign-cla.md) to find out how.

### CONNECT

Having problems or have awesome suggestions? Connect with us [here.](https://community.choosatron.com/)

### VERSION HISTORY

Latest Version: v1.0.0
