// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_STORY_LOAD_H
#define CDAM_STORY_LOAD_H

// Instead of "Arduino.h"
//#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
#include "cdam_constants.h"

namespace cdam
{

class Printer;

class StoryLoad {

public:
    StoryLoad(Printer * aPrinter);
    ~StoryLoad();
    static unsigned long getFileSize(const char *path);

    static byte getByteFromFile(char *path, unsigned long index);
    static char *getStringFromFile(const char *path, unsigned long length, unsigned long index);
    char *getPassagePath(char aFilename);
    void loadPassage();
    void recordChoiceNode();
    void loadPassageStr();
    char *loadManifest();

    unsigned int makeWord(unsigned char h, unsigned char l);

    // If manifest has been loaded.
    boolean manLoaded;
    // Only one choice, autojump.
    boolean autoJump;
    // Number of chars to indent for autojump text.
    byte jumpIndent;
    // Number of stories to choose from.
    byte storyCount;
    // Selected story.
    char storyId;
    // Total points for a gameplay.
    unsigned int totalPoints;
    // Perfect Score for the Story.
    unsigned int perfectScore;
    // Keep track of autojump points for continues.
    byte jumpPoints;
    // Total choices made.
    byte totalChoices;
    // Point penalty for a continue.
    byte continuePenalty;
    // 2D array of last 9 passages visited.
    static char continuePath[9][4];
    // Current passage.
    char passageId[3];
    byte ending;
    byte hasEnded;
    byte quality;
    byte canContinue;
    byte choiceCount;
    byte points;

    // Input & choice.
    byte choiceNum;
    char choice;
private:
    Printer * _printer;
};

}

#endif