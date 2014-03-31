// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC.
// All rights reserved.

#ifndef __CHOOSATRON_H
#define __CHOOSATRON_H

#include "cdam_constants.h"

namespace cdam
{

class Keypad;

class Choosatron
{
 public:
    Choosatron(void);
    bool setup(void);
    int loop(void);

 private:
  	void enableTimerInterrupt(void);

  	//Keypad *_keypad;
};

}

#endif