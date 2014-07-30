// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_COIN_ACCEPTOR_H
#define CDAM_COIN_ACCEPTOR_H

#include "cdam_constants.h"

namespace cdam
{


class CoinAcceptor {

public:
    CoinAcceptor();
    void initialize();
    void updateState();
    //static void waitForCoin(const Keypad *aKeypad);
    boolean checkForCoin();
    void coinInserted();
    boolean consumeCredit();
    boolean consumeContinue();

    // Coins towards next credit.
    byte nextCredit;
    // Timestamp for last "Insert Coin" message displayed.
    unsigned long insertCoinTime;
    // Coins Per Credit
    byte coinsPerCredit;
    // Coins To Continue
    byte coinsToContinue;

    // Should the coin acceptor respond to update requests.
    bool active;

private:
    // Plays queued
    byte _credits;
    // Total coins inserted while machine active.
    byte _coins;

};

}

#endif