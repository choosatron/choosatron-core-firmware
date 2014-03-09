// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2013 Monkey with a Mustache Entertainment, Inc.
// All rights reserved.

#include "cdam_story_load.h"
#include "memory_free.h"
#include "cdam_printer.h"
#include "SD.h"

namespace cdam
{

char StoryLoad::continuePath[9][4] = {0};

StoryLoad::StoryLoad(Printer* aPrinter) :
  _printer(aPrinter)
{
}

StoryLoad::~StoryLoad() {
}

unsigned long StoryLoad::getFileSize(const char *path) {
  File file = SD.open(path);
  uint32_t fileSize = 0;
  if (file) {
    fileSize = file.size();
  }
  file.close();
  return fileSize;
}

byte StoryLoad::getByteFromFile(char *path, unsigned long index = 0) {
  File file = SD.open(path);
  byte b = '\0';
  if (file) {
    if ((index != 0) && (index <= file.size())) {
      file.seek(index);
    }
    b = file.read();
  } else {
    //slog(PSTR("[ERROR] File not found: "));
    //slogln(path);
  }
  file.close();
  return b;
}

char *StoryLoad::getStringFromFile(const char *path, unsigned long length = MAX_PASSAGE_BYTES, unsigned long index = 0) {
  File file = SD.open(path);
  char *text = NULL;
  if (file) {
    if ((file.size() - index) < length) {
        length = file.size() - index;
    }
    text = (char *)malloc(length+1);
    if ((index != 0) && (index <= file.size())) {
      file.seek(index);
    }
    if (file.available()) {  
      file.read(text, length);
    }
    text[length] = '\0';
  }
  file.close();
  return text;
}

char *StoryLoad::getPassagePath(char aFilename) {
  char *path = (char *)malloc(8);
  path[5] = '\0';
  path[6] = '\0';
  path[7] = '\0';
  byte i = 0;
  path[i] = this->storyId;
  i++;
  path[i] = '/';
  i++;
  path[i] = this->passageId[0];
  i++;
  if (this->passageId[1] != '\0') {
    path[i] = this->passageId[1];
    i++;
    if (this->passageId[2] != '\0') {
      path[i] = this->passageId[2];
      i++;
    }
  }
  path[i] = '/';
  i++;
  path[i] = aFilename;
  return path;
}

unsigned int StoryLoad::makeWord(unsigned char h, unsigned char l) {
  return (h << 8) | l;
}

void StoryLoad::loadPassage() {
  _printer->setPrinting(true);

  if (this->hasEnded > 0) {
    // We've printed out the ending continue and the key was pressed.
    this->hasEnded++;
    slog("Has ended: ");
    slogln(this->hasEnded);
    return;
  }
  char *dataPath = getPassagePath('E');
  this->ending = getByteFromFile(dataPath);
  slogln(dataPath);
  slog(PSTR("Ending: "));
  slogln(this->ending);
  if (this->ending) {
    // An ending of the story.
    this->quality = getByteFromFile(dataPath, 1);
    this->canContinue = getByteFromFile(dataPath, 2);
    slog(PSTR("Quality: "));
    slogln(this->quality);
    this->hasEnded = 1;
  } else if (this->totalChoices == 0) {
    slogln("Total choices 0");
    // First passage of the story.
    this->continuePenalty = getByteFromFile(dataPath, 1);
    this->perfectScore = (unsigned int)(makeWord(getByteFromFile(dataPath, 2), getByteFromFile(dataPath, 3)));
    slog(PSTR("Continue Penalty: "));
    slog(this->continuePenalty);
    slog(PSTR(", Perfect Score: "));
    slogln(this->perfectScore);
  }
  free(dataPath);
  char *psgPath = getPassagePath('P');
  this->points = getByteFromFile(psgPath);
  free(psgPath);
  this->totalPoints += this->points;
  
  byte indent = 0;
  boolean linefeed = true;
  byte lastCount = this->choiceCount;
  if (!this->ending) {
    char *countPath = getPassagePath('N');
    this->choiceCount = getByteFromFile(countPath);
    free(countPath);
  }

  if (lastCount > 1) {
    if (this->choice == '9') {
      _printer->print("10.");
      indent = 3;
    } else {
      _printer->print(++this->choice);
      _printer->print('.');
      indent = 2;
    }
  } else if (this->jumpIndent == 1) {
    indent = _printer->jumpIndent;
    slogln(_printer->jumpIndent);
  }
  // DOESN't WORK???
  //this->autoJump = false;
  this->jumpIndent = 0;
  // After this passage is an autojump.
  if (this->choiceCount == 1) {
    char *choicePath = getPassagePath('C');
    byte jump = getByteFromFile(choicePath);
    if (jump == '*') {
      //this->autoJump = true; // DOESN'T WORK??
      this->jumpIndent = 1;
      this->jumpPoints += this->points;
      linefeed = false;
    }
  }

#if (DEBUG && MEMORY)
  slog(PSTR("\t* PrintFile Start - Free Memory: "));
  slogln(freeMemory());
#endif

  char *textPath = getPassagePath('T');
  slog("Text Path: ");
  slogln(textPath);
  _printer->printFile(textPath, true, linefeed, indent, 0);
  free(textPath);

#if (DEBUG && MEMORY)
  slog(PSTR("\t* PrintFile End - Free Memory: "));
  slogln(freeMemory());
#endif

  if (!this->ending && (this->choiceCount > 1)) {
    // Only record a choice point if it isn't an ending.
    recordChoiceNode();

    if (this->choiceCount > 4) {
      _printer->println(PSTR("*Press buttons at the same time to add up to higher choices.*"));
    }
  }
  
  // Print out choices is it isn't an end and isn't an autojump.
  if (!this->ending && !this->autoJump) {
    for (byte i = 0; i < this->choiceCount; ++i) {
      char *choicePath = getPassagePath(i + '0');
      slog("Choice Path: ");
      slogln(choicePath);
      _printer->printFile(choicePath, true, true, 0, 3);
      free(choicePath);
    }
    _printer->feed(2);
  } else if (this->hasEnded == 1) {
    slogln("Ending");
    _printer->println(PSTR("1.Continue..."));
    _printer->feed(2);
  }
}

void StoryLoad::recordChoiceNode() {
  StoryLoad::continuePath[this->totalChoices % 9][0] = this->passageId[0];
  StoryLoad::continuePath[this->totalChoices % 9][1] = this->passageId[1];
  StoryLoad::continuePath[this->totalChoices % 9][2] = this->passageId[2];
  StoryLoad::continuePath[this->totalChoices % 9][3] = this->points;
  if (this->jumpPoints > 0) {
    StoryLoad::continuePath[this->totalChoices % 9][3] += this->jumpPoints;
    this->jumpPoints = 0;
  }
}

void StoryLoad::loadPassageStr() {
  char *passagePath = getPassagePath(this->choice);
  this->passageId[0] = getByteFromFile(passagePath);
  this->passageId[1] = getByteFromFile(passagePath, 1);
  this->passageId[2] = getByteFromFile(passagePath, 2);
  free(passagePath);
  if (this->passageId[1] == '-') {
    this->passageId[1] = '\0';
  } else if (this->passageId[2] == '-') {
    this->passageId[2] = '\0';
  }
}

char *StoryLoad::loadManifest() {
  this->storyCount = getByteFromFile("M/C");
  slog(PSTR("Story Count: "));
  slogln(this->storyCount);
  char path[4] = {'M', '/', 'T'};
  char *stories = getStringFromFile(path);
  return stories;
}

}