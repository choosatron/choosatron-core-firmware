// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_coin_acceptor.h"

namespace cdam
{

/* Public Methods */

CoinAcceptor::CoinAcceptor() {
}

void CoinAcceptor::initialize(uint8_t aCoinsPerCredit, uint8_t aCoinsToContinue) {
   coinsPerCredit = aCoinsPerCredit;
   coinsToContinue = aCoinsToContinue;
   coins = 0;
   _coinSensed = false;
   this->active = true;
}

void CoinAcceptor::updateState() {
   if (this->active) {
      checkForCoin();
   }
}

void CoinAcceptor::addCredits(uint8_t aCredits) {
   int8_t adjustment = aCredits * coinsPerCredit;
   if ((adjustment < 0) && ((-1 * adjustment) > coins)) {
      coins = 0;
   } else {
      coins += adjustment;
   }
}

bool CoinAcceptor::consumeCredit() {
   if (coins >= coinsPerCredit) {
      coins -= coinsPerCredit;
      return true;
   }
   return false;
}

bool CoinAcceptor::consumeContinue() {
   if (coins >= coinsToContinue) {
      coins -= coinsToContinue;
      return true;
   }
   return false;
}

/* Accessors */

void CoinAcceptor::setCredits(uint8_t aCredits) {
   coins = (aCredits * coinsPerCredit);
}

uint8_t CoinAcceptor::getCredits() {
   return coins / coinsPerCredit;
}

/* Private Methods */

void CoinAcceptor::checkForCoin() {
   uint8_t pinRead = digitalRead(PIN_COIN);
   if (!_coinSensed && pinRead) {
      //DEBUG("Coin Start");
      _coinSensed = true;
      _coinElapsed = 0;
   } else if (_coinSensed && !pinRead) {
      uint8_t elapsed = (uint8_t)_coinElapsed;
      //DEBUG("Coin Stop: %d", elapsed);
      _coinSensed = false;
      if ((elapsed >= kCoinAcceptorSenseMin) &&
         (elapsed <= kCoinAcceptorSenseMax)) {
         coins++;
         //DEBUG("Coins: %d", coins);
      }
   }
}

}