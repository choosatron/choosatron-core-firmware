// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC.
// All rights reserved.

#ifndef CDAM_MAIN_H
#define CDAM_MAIN_H

namespace cdam
{

class DataManager;
class HardwareManager;
class ServerManager;

class Choosatron
{
 public:
    Choosatron(void);

    bool setup(void);
    int loop(void);

 private:
 	DataManager* _dataManager;
 	HardwareManager* _hardwareManager;
 	ServerManager* _serverManager;

};

}

#endif