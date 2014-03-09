#include "choosatron.h"

Choosatron::Choosatron(void)
{
  // do stuff
}

bool Choosatron::setup(ThermalPrinter& tp)
{
  thermal_printer = &tp;
  return true;
}

int Choosatron::loop(void)
{
  return 42;
}
