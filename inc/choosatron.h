#ifndef __CHOOSATRON_H
#define __CHOOSATRON_H

#include "thermal_printer.h"

class Choosatron
{
  public:
    Choosatron(void);
    bool setup(ThermalPrinter&);
    int loop(void);

  private:
    ThermalPrinter *thermal_printer;
};

#endif