

## 1.0.0

Features:

  - Updated text for initial release.

## 2.0.0a5 (unreleased)

Features:

  - Removed cdam_main to use application.cpp in new HAL repo structure.
  - Added firmware binaries for all tagged versions.
  - Cleaned up debug comments.
  - Ability to delete individual stories.
  - Revamping Choosatron API to reduce the number of calls apps need to make to it.
  - Added serial firmware update / flash writing, and initial structure for more serial commands.
  - Added random play mode - release all four buttons together on the 'Press any button to start!'
  - Increase 'Press any...' message delay from 30 secs to 40 (after a game ends)
  - Added feature to allow changing the keypad setup depending on PCB version.

## 2.0.0a4 (unreleased)

Features:

  - Added CHANGELOG.md
  - Unified Choosatron and original printer library to reduce space & update design.

## 2.0.0a3 (unreleased)

Bugfixes:

  - Fixed IF operation, was accepting 0 as true (now > 0 instead of >= 0).
  - Fixed edge case bugs for word wrap, including append scenario.

## 2.0.0a2 (unreleased)

Features:

  - Added special button presses for WiFi & SD card use.
  - New parsing engine complete, able to recursively evaluation conditions and updates.

Bugfixes:

  - Fixed spelling errors.

## 2.0.0a1 (unreleased)

Features:

  - SD card story reading is now functional.

Bugfixes:

  - WiFi loop bugs.

## 2.0.0 (unreleased)

Features:

  - Initial functioning v2 firmware (written from scratch).