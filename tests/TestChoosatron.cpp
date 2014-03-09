#include "UnitTest++.h"
#include "Choosatron.h"

struct ChoosatronFixture
{
  int some_fixture_var;

  ChoosatronFixture();
};

ChoosatronFixture::ChoosatronFixture()
{
  some_fixture_var = -98765;
}

SUITE(Choosatron)
{
  TEST_FIXTURE(ChoosatronFixture, TheVariableIsInitialized)
  {
    CHECK_EQUAL(-98765, some_fixture_var);
  }

  TEST(ChoosatronSetsUp)
  {
    Choosatron choosatron;
    ThermalPrinter thermal_printer;
    bool success = choosatron.setup(thermal_printer);
    CHECK(success);
  }

  TEST(ChoosatronLoops)
  {
    Choosatron choosatron;
    int result = choosatron.loop();
    CHECK_EQUAL(42, result);
  }
}
