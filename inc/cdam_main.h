// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC.
// All rights reserved.

#ifndef CDAM_MAIN_H
#define CDAM_MAIN_H

namespace cdam
{

class StateController;

class Choosatron
{
 public:
 	/* Public Methods */
    Choosatron(void);

    bool setup(void);
    int loop(void);

 private:
 	/* Private Methods */

 	/* Private Variables */
 	StateController* _stateController;

};

}

#endif