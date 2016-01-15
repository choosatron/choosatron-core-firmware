#ifndef CDAM_CONST_STRINGS_H
#define CDAM_CONST_STRINGS_H

namespace cdam
{

const char CDAM_TITLE[] = "The Choosatron";
const char CDAM_SUBTITLE[] = "Deluxe Adventure Matrix v1.0.1\n\nby Jerry Belich - choosatron.com\n@j3rrytron - jerrytron.com";
const char CDAM_PRESS_BUTTON[] = "Press any button to start!";
const char CDAM_START[] = "Choose your story below!\n";
const char CDAM_EMPTY[] = "No stories installed! For help,\nhead to choosatron.com/start\n";
const char CDAM_STORY_BY[] = "a story by";
const char CDAM_CREDITS[] = "Credits to %s";
const char CDAM_ADMIN_ON[] = "ON";
const char CDAM_ADMIN_OFF[] = "OFF";
//const char CDAM_POINTS[] = "%d Points";
//const char CDAM_POINTS_OUT_OF[] = "%d out of %d Points";
//const char CDAM_POINTS_PERFECT[] = "A perfect score!!!";
const char CDAM_ARCADE_CREDITS[] = "CREDITS %d/%d - INSERT COIN(S)";
const char CDAM_ARCADE_CONTINUE[] = "*INSERT %d COIN(S) TO CONTINUE*";
const char CDAM_TEAR_GENTLY[] = "<- TEAR GENTLY TO THE SIDE ->\n- - - - - - - - - - - - - - - - ";
const char CDAM_BIG_NUMBERS[] = "+Add Buttons for Higher Choices+\n [1]+[4] = 5 or [2]+[3]+[4] = 9";

const char CDAM_SERVER_RECEIVE_STORY[] = "Receiving story data...";
const char CDAM_SERVER_REBOOT[] = "Complete! Rebooting...";
//const char CDAM_SERVER_COMPLETE[] = "Complete!\nPress any button for menu.";

#ifndef DEBUG_BUILD
const char CDAM_LINE[]          = "------------------------------";
//const char CDAM_LINE_TOP[]    = "/------------------------------\\";
const char CDAM_JERRY_ONE[]     = "|    Hi, Jerry Belich here.    |";
const char CDAM_JERRY_TWO[]     = "| ~~Certified Mad Scientist~~  |";
//const char CDAM_LINE[]        = "|------------------------------|";
const char CDAM_CONTACT_ONE[]   = "|  @j3rrytron - jerrytron.com  |";
const char CDAM_CONTACT_TWO[]   = "|   Press kit pages: ^/press   |";
//const char CDAM_LINE[]        = "|------------------------------|";
const char CDAM_CONTACT_THREE[] = "|  Email jerry@jerrytron.com   |";
const char CDAM_CONTACT_FOUR[]  = "|    Phone +1 612-306-8258     |";
//const char CDAM_LINE[]        = "|------------------------------|";
const char CDAM_WHAT[]          = "|    What can I do for you?    |";
const char CDAM_BULLET1[]       = "| * custom hardware toys/games |";
const char CDAM_BULLET2[]       = "| * desktop/mobile game design |";
const char CDAM_BULLET3[]       = "| * electronics/game workshops |";
const char CDAM_BULLET4[]       = "| * talks/guest lecturing      |";
const char CDAM_BULLET5[]       = "| * narrative & storytelling   |";
const char CDAM_BULLET6[]       = "| * interactive installations  |";
const char CDAM_BULLET7[]       = "|   & augmented experiences    |";
//const char CDAM_LINE_BOTTOM[]= "\\------------------------------/";

//const char CDAM_WHERE_ONE[]     = "|   We met on May 19th, 2015   |";
//const char CDAM_WHERE_TWO[]     = "|   at the Mini Maker Faire!   |";
//const char CDAM_LINE_SPACE[]    = "|                              |";
//const char CDAM_BULLET8[]       = "| * classical theremin         |";
//const char CDAM_BULLET9[]       = "| * comedy shows for events    |";
#endif

}

#endif