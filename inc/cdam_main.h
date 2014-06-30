// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC.
// All rights reserved.

#ifndef CDAM_MAIN_H
#define CDAM_MAIN_H

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
  	void updateKeypad();

  	//Keypad *_keypad;
};

}

#endif