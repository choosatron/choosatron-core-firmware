## 1.0.7

Features:

  - A bunch missed between this and 1.0.0!
  - Logic features enhanced, support for custom Inky editor Choosatron story export.

Bugfixed:

  - The logic system worked for basic things previously, but now that I'm able to properly test, there were a lot of issues. They have been resolved.

## 1.0.0

Features:

  - Updated text for initial release.

## 0.2.5 (unreleased)

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

## 0.2.4 (unreleased)

Features:

  - Added CHANGELOG.md
  - Unified Choosatron and original printer library to reduce space & update design.

## 0.2.3 (unreleased)

Bugfixes:

  - Fixed IF operation, was accepting 0 as true (now > 0 instead of >= 0).
  - Fixed edge case bugs for word wrap, including append scenario.

## 0.2.2 (unreleased)

Features:

  - Added special button presses for WiFi & SD card use.
  - New parsing engine complete, able to recursively evaluation conditions and updates.

Bugfixes:

  - Fixed spelling errors.

## 0.2.1 (unreleased)

Features:

  - SD card story reading is now functional.

Bugfixes:

  - WiFi loop bugs.

## 0.2.0 (unreleased)

Features:

  - Initial functioning v2 firmware (written from scratch).