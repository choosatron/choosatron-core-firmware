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
    /* Public Methods */
    CoinAcceptor(uint8_t aCoinsPerCredit = 2, uint8_t aCoinsToContinue = 1);
    void initialize();
    void updateState();
    //static void waitForCoin(const Keypad *aKeypad);
    // bool checkForCoinOld();
    void addCredits(uint8_t aCredits);
    void setCredits(uint8_t aCredits);
    uint8_t getCredits();
    bool consumeCredit();
    bool consumeContinue();

    /* Accessors */
    // uint8_t getCoinsPerCredit();
    // void setCoinsPerCredit(uint8_t aCoinsPerCredit);
    // void setCoinsToContinue(uint8_t aCoinsToContinue);

    /* Public Variables */
    // Total coins inserted while machine active.
    uint8_t coins;
    // Coins Per Credit
    uint8_t coinsPerCredit;
    // Coins To Continue
    uint8_t coinsToContinue;
    // Should the coin acceptor respond to update requests.
    bool active;

private:
    /* Private Methods */
    void checkForCoin();

    /* Private Variables */
    uint8_t _coinSenseCounter;

};

}

#endif