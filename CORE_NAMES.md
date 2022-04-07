Choosatron Community SSH Access
ssh root@45.55.133.164

USB Upload

1. Put you Core into the DFU mode by holding down the MODE button on the Core and then tapping on the RESET button once. Release the MODE button after you start to see the RGB LED flashing in yellow. It's easy to get this one wrong: Make sure you don't let go of the left button until you see flashing yellow, about 3 seconds after you release the right/RESET button. A flash of white then flashing green can happen when you get this wrong. You want flashing yellow.

2. Open up a terminal window on your computer and type this command to find out if the Core indeed being detected correctly.

dfu-util -l
you should get the following in return:

Found DFU: [1d50:607f] devnum=0, cfg=1, intf=0, alt=0, name="@Internal Flash  /0x08000000/20*001Ka,108*001Kg" 
Found DFU: [1d50:607f] devnum=0, cfg=1, intf=0, alt=1, name="@SPI Flash : SST25x/0x00000000/512*04Kg"
(Windows users will need to use the Zatig utility to replace the USB driver as described earlier)

Now, navigate to the build folder in your core-firmware repository and use the following command to transfer the .bin file into the Core.

dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D choosatron-core.bin

curl "https://api.spark.io/v1/devices/48ff6f065067555019501287/last_cmd?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c"

curl https://api.spark.io/v1/devices/48ff6f065067555019501287/cmd \
     -d access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c \
     -d "args=ping"

curl -H "Authorization: Bearer 3cbce66f21cd4e0ede781cbe95d558464515ef3c"  https://api.spark.io/v1/events/get_seconds

make v=1 PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=1 APP=choosatron TARGET_FILE=cdam-firmware clean all

make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=1 APP=choosatron DEBUG_BUILD=y TARGET_DIR=applications/choosatron/build v=1

make DEBUG_BUILD=y
make clean all SPARK_PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=1 DEBUG_BUILD=y

make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=101 APP=choosatron-core DEBUG_BUILD=y TARGET_DIR=applications/choosatron-core/build v=1

make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=101 APP=choosatron-core-firmware TARGET_DIR=applications/choosatron-core-firmware/build debug_build=y v=1 program-dfu


make PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=101 APPDIR=../user/applications/choosatron-core-firmware TARGET_DIR=../user/applications/choosatron-core-firmware/build DEBUG_BUILD=y v=1

curl https://api.spark.io/v1/devices/48ff70065067555020101587/dfu -d access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -m 2

make clean all PRODUCT_ID=7 PRODUCT_FIRMWARE_VERSION=1

yModem Protocol
DEV=/dev/cu.usbmodem621     # <------ or whatever
sz -b -v --ymodem choosatron.bin > $DEV < $DEV

echo -n $'\x01' for binary bytes

or if you need to hit the 'f' and send the file in the same command:

echo -n f > $DEV && sz -b -v --ymodem choosatron.bin > $DEV < $DEV

Testing Sending Slugfest Over Serial
Bytessize: 6822
Index: 20
$'\x00\x00\x1a\xa6\\x14'

Inking Tattoo - 63080000225114

A Very Very Very Scary House - 6308000107C814
Escape from Pompeii - 6308000023E114
Finding Your Shadow - 630800002D4014
Mustache Rangers - 630800003A0A14
My Super Hero - 63080000235E14
On a Dark and Stormy Night - 630800002C6D14
Operation Brain Drain - 63080000205314
Slugfest - 630800001ABA14
The Librarian's Apprentice - 630800002F1014
Wrestlefest - 630800006A0F14

A Very Very Very Scary House - 000107C8
Another Day at the MIA - 0000127A
Bones and What They Mean - 00012E66
Cutting Through the Corn - 00004EE5

Escape from Pompeii - 000023CD
Eyes That Watch - 00002A63
Finding Your Shadow - 00002D2C
Groove Driver - 00004785
John Stamos - 000063EC
My Super Hero - 0000234A

No Brakes Valet - 00002AAC
Operation Brain Drain - 0000203F
Revolver - 000005A5
Slugfest - 00001AA6
Thanksgiving of Despair - 00003F7B
The Chandos - 00002068
The Gauche Criminal - 00003CDA
The Librarians Apprentice - 00002EFC
The Spy Who Killed Me - 00004F39
Wormsign - 00002E84

printf '%b%b%b%b%b%b' '\x63\x08\x00\x00\x1a\xa6\x14' > $DEV && sz -b -v --ymodem slugfest_BIN.dam > $DEV < $DEV
printf '%b%b%b%b%b%b' '\x63\x08\x00\x00\x63\xec\x14' > $DEV && sz -b -v --ymodem john_stamos_BIN.dam > $DEV < $DEV
printf '%b%b%b%b%b%b' '\x63\x08\x00\x00\x05\xa5\x14' > $DEV && sz -b -v --ymodem revolver_BIN.dam > $DEV < $DEV

Flash Raw
630300080000
printf '%b%b%b%b%b%b' '\x63\x03\x00\x08\x00\x00' > $DEV && sz -b -v --ymodem flash_backup.bin > $DEV < $DEV
echo -n $'\x63\x03\x31' > $DEV && sz -b -v --ymodem flash_backup.bin > $DEV < $DEV
echo -n cf1 > $DEV && sz -b -v --ymodem flash_backup.bin > $DEV < $DEV

echo -n $'\x77\x66' > $DEV && sz -b -v --ymodem choosatron.bin > $DEV < $DEV

echo -n $'\x02\x00\x00\x1a\xa6\x14' > $DEV
sz -b -v --ymodem slugfest_BIN.dam > $DEV < $DEV

Size Info
text + data = flash used
data + bas = RAM used

flash total = 110592
RAM total = 20480

Tab One
cd ~/Code/MwaM/Choosatron/cdam-core-firmware/build/
make DEBUG_BUILD=y
dfu-util -d 1d50:607f -a 0 -s 0x08005000:leave -D cdam-core-firmware.bin

Tab Two
cd ~/Code/MwaM/Choosatron/cdam-core-firmware/build/
../../DevelopmentTools/Scripts/make-dfu.sh 48ff70065067555020101587

Tab Three
cd ~/Code/MwaM/Choosatron/cdam-twine/
python send_binary.py story_bins/
curl https://api.spark.io/v1/devices/53ff6c065067544835590687/command -d access_token=0bf476f99bfc311a85c3193e31fed9c4abf20894 -d "args=remove_all_stories"
curl https://api.spark.io/v1/devices/53ff6c065067544835590687/command -d access_token=0bf476f99bfc311a85c3193e31fed9c4abf20894 -d "args=set_flag|000"

Tab Four
cd ~/Code/MwaM/Choosatron/cdam-twine/
./generate_story_bins.sh

Flags
<flag byte index><flag index><on or off>
Flag 0:
0 = auth
1 = random
2 = continues
3 = arcade
4 = multiplayer
5 = sdCard
6 = demo
7 = offline

Flag 1:
0 - 3: unused
4 = logPrint
5 = logLive
6 = logLocal
7 = logging

keypad_input <int of event><int value>
Event Vals
0 = no event
1 = multi down event
2 = multi up event

button_input <int of event><button number>
Event Vals
0 = no event
1 = button down
2 = button held
3 = button up

adjust_credits <num of credits><1 = positive, 0 = negative>
add or subtract credits using this

FOR DEBUG USAGE
Add DEBUG_BUILD=y to make.
	make DEBUG_BUILD=y


https://community.spark.io/t/lets-get-non-volatile/2200/3
http://docs.spark.io/#/hardware/memory-mapping-external-flash-memory-map

334 byte test file
dfu-util -d 1d50:607f -a 1 -s 0x00080000 -v -D cdam-test-in.cdam
dfu-util -d 1d50:607f -a 1 -s 0x80000:0x0014e -U cdam-test-out.cdam

D3-D7 JTAG PINS

Backing up entire Flash Memory (User Space)
dfu-util -d 1d50:607f -a 1 -s 0x80000:1572864 -U flash_backup.bin

Restoring entire Flash Memory (User Space)
dfu-util -d 1d50:607f -a 1 -s 0x80000 -v -D flash_backup.bin

651-222-6500

WiFi Upload

Jerry Belich
Access Token: 0bf476f99bfc311a85c3193e31fed9c4abf20894


Dan Kane
Access Token: 0bff41a55ceed0ada75033458a6ba83bc69532e1


Janel Dowd Spark Core
Core ID: 53ff6e065075535143541587

afterglow-`one (sparkle one)
Core ID: 55ff6d065075555357220487

alfred
Core ID: 48ff6f065067555046281587

curl -X PUT https://api.spark.io/v1/devices/48ff6f065067555046281587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


belvedere - LENT TO ALEX BARRETT
Core ID: 53ff6b065067544813371287

curl -X PUT https://api.spark.io/v1/devices/53ff6b065067544813371287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


coraline
Core ID: 48ff6c065067555050271587

curl -X PUT https://api.spark.io/v1/devices/48ff6c065067555050271587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


dandroid
Core ID: 51ff6b065067545714490187

curl -X PUT https://api.spark.io/v1/devices/51ff6b065067545714490187\?access_token=0bff41a55ceed0ada75033458a6ba83bc69532e1 -F file=@cdam-core-firmware.bin

curl https://api.spark.io/v1/devices/51ff6b065067545714490187/dfu -d access_token=0bff41a55ceed0ada75033458a6ba83bc69532e1 -m 2
curl https://api.spark.io/v1/devices/51ff6b065067545714490187/command \
	-d access_token=0bff41a55ceed0ada75033458a6ba83bc69532e1 \
	-d “args=ping”

ender
Core ID: 48ff6b065067555045541587

curl -X PUT https://api.spark.io/v1/devices/48ff6b065067555045541587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


fidget
Core ID: 48ff70065067555020101587

curl -X PUT https://api.spark.io/v1/devices/48ff70065067555020101587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


grendel
Core ID: 55ff71064989495317442587

curl -X PUT https://api.spark.io/v1/devices/55ff71064989495317442587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


hildegard - ZKM
Core ID: 48ff6b065067555042101587

curl -X PUT https://api.spark.io/v1/devices/48ff6b065067555042101587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


istanbul - ZKM
Core ID: 51ff6c065067545717380687

curl -X PUT https://api.spark.io/v1/devices/51ff6c065067545717380687\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


jellybean
Core ID: 48ff6a065067555060331387

curl -X PUT https://api.spark.io/v1/devices/48ff6a065067555060331387\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


kilroy
Core ID: 53ff6e065067544845371287

curl -X PUT https://api.spark.io/v1/devices/53ff6e065067544845371287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


larrick
Core ID: 48ff6d065067555038391187

curl -X PUT https://api.spark.io/v1/devices/48ff6d065067555038391187\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


monet
Core ID: 48ff73065067555047561287

curl -X PUT https://api.spark.io/v1/devices/48ff73065067555047561287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


natalie - DEAD
Core ID: 48ff6f065067555019501287

curl -X PUT https://api.spark.io/v1/devices/48ff6f065067555019501287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


olivia
Core ID: 53ff6c065067544835590687

curl -X PUT https://api.spark.io/v1/devices/53ff6c065067544835590687\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


pilk - Sold to Robin Arnott
Core ID: 50ff6a065067545626240587

curl -X PUT https://api.spark.io/v1/devices/50ff6a065067545626240587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


quixote (RETIRED)
Core ID: 55ff6b065075555320141787 (SENT TO Bart Clark)

curl -X PUT https://api.spark.io/v1/devices/55ff6b065075555320141787\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


ryanair
Core ID: 53ff64065067544830330187

curl -X PUT https://api.spark.io/v1/devices/53ff64065067544830330187\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


snape
Core ID: 53ff6b065067544835331287

curl -X PUT https://api.spark.io/v1/devices/53ff6b065067544835331287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin


tickler
Core ID: 55ff6f065075555345270287

curl -X PUT https://api.spark.io/v1/devices/55ff6f065075555345270287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@cdam-core-firmware.bin

----------------------

msminotaur
Core ID: 51ff6c065067545752380687

curl -X PUT https://api.spark.io/v1/devices/51ff6c065067545752380687\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.0.bin


gsc_one
Core ID: 53ff6f065067544841431087

curl -X PUT https://api.spark.io/v1/devices/53ff6f065067544841431087\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.3.bin


gsc_two
Core ID: 48ff70065067555031281587

curl -X PUT https://api.spark.io/v1/devices/48ff70065067555031281587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.3.bin


gsc_three
Core ID: 48ff6e065067555043531587

curl -X PUT https://api.spark.io/v1/devices/48ff6e065067555043531587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.3.bin


gsc_four
Core ID: 48ff6a065067555028101587

curl -X PUT https://api.spark.io/v1/devices/48ff6a065067555028101587\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.3.bin


gsc_five
Core ID: 53ff6c065067544834361287

curl -X PUT https://api.spark.io/v1/devices/53ff6c065067544834361287\?access_token=3cbce66f21cd4e0ede781cbe95d558464515ef3c -F file=@choosatron_v1.3.3-pcb_v2.3.bin