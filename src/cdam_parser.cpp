#include "cdam_parser.h"
#include "cdam_manager.h"
// #include <regex>

namespace cdam
{

/* Public Methods */

void Parser::initialize() {
   _dataManager = Manager::getInstance().dataManager;
   _hardwareManager = Manager::getInstance().hardwareManager;
   _state = PARSE_IDLE;

   // if (std::regex_match ("subject", std::regex("(sub)(.*)") )) {

   // }
}

bool Parser::initStory(uint8_t aStoryIndex) {
   if (!_dataManager->loadStory(aStoryIndex)) {
      return 0;
   }
   _offset = _dataManager->startOffset;
   _state = PARSE_UPDATES;
   _appended = false;
   // The story number should then print out in front of the first passage.
   _choiceSelected = _dataManager->currentStory;
   _parsingChoices = false;
   _dataLength = 0;

   _hardwareManager->printer()->printStoryIntro(_dataManager->storyHeader.title, _dataManager->storyHeader.author);
   return 1;
}

ParseState Parser::parsePassage() {
   // Keep the Spark loop running!
   Spark.process();
   if (_state == PARSE_UPDATES) {
      uint32_t processedBytes = 0;
      if (!_parsingChoices) {
         // Get passage attributes.
         _passage = new Passage();
         _passage->attribute = _dataManager->readByte(_offset);
         _offset++;
      }
      if ((processedBytes = parseValueUpdates(_offset))) {
         _offset += processedBytes;
         // Only occurs if a choice has been selected.
         if (_parsingChoices) {
            // Choice - 1 for index, get actual index from _choiceLinks which
            // compensates for potentially invisible choices.
            DEBUG("Choice selected: %d, link index: %d", _choiceSelected, _choiceLinks[_choiceSelected - 1]);
            Choice choice = _choices[_choiceLinks[_choiceSelected - 1]];
            _offset = _dataManager->startOffset + _dataManager->getPassageOffset(choice.passageIndex);
            DEBUG("Jumping to passage %d at offset: %lu", choice.passageIndex, _offset);
            cleanupAfterPassage();
         } else {
            // Finished value updates, move on to passage body data.
            _state = PARSE_DATA;
         }
      }
   } else if (_state == PARSE_DATA) {
      // Needed to make room for passage or choice numbering.
      uint8_t bufferPadding = 0;
      bool dataStart = false;
      uint8_t choiceData = 0;
      if (_dataLength == 0) {
         dataStart = true;
         if ((!_appended && !_parsingChoices && (_visibleCount > 1)) ||
            (_parsingChoices && !_passage->append)) {
            _lastIndent = 0;
            // Setup padding for printing a number in front of the text.
            choiceData = _parsingChoices ? _visibleCount : _choiceSelected;
            if (choiceData > 0) {
               bufferPadding = (_choiceSelected == 10) ? 4 : 3;
            }
         } else if (_appended) {
            // Add padding for a single inserted space between previous passage
            // and the one we are about to append.
            bufferPadding = 1;
         }
         // Get the length of the data bytes.
         if (!_dataManager->readData(&_dataLength, _offset, kDataLengthSize)) {
            _state = PARSE_ERROR;
         }
         _offset += kDataLengthSize;
      }
      // Allocate a buffer of data length, up to a maximum.
      uint16_t bufferSize = (_dataLength < (kPassageBufferReadSize + _lastIndent)) ? _dataLength : (kPassageBufferReadSize + _lastIndent);
      //DEBUG("Bufsize: %d", bufferSize);
      _buffer = new char[bufferSize + bufferPadding + 1](); // + 1 to null terminate if it is all text.
      // Insert the numbering.
      if (dataStart) {
         sprintf(_buffer, "%d. ", choiceData);
      }

      // Now that we've loaded our buffer data, insert the space.
      if (_appended) {
         _appended = false;
         _buffer[0] = ' ';
      }
      // Will either fill buffer with raw text or text generated from a command.
      // It will always stop on a command byte barrier to print what it has so far,
      // just after a command, or if it hits the end of a buffer that is only text.
      uint32_t processedBytes = parseData(_offset, _buffer + bufferPadding, bufferSize);

      if (processedBytes > 0) {
         // Word wrap the text.
         _lastIndent = _hardwareManager->printer()->wrapText(_buffer, kPrinterColumns, _lastIndent);
         // Print the text up to the command.
         _hardwareManager->printer()->print(_buffer);
         DEBUG("Buffer: %s", _buffer);
         _offset += processedBytes;
         _dataLength -= processedBytes;
      }

      if (_dataLength == 0) {
         delete[] _buffer;
         _buffer = NULL;
         if (_parsingChoices) { // Passage Index is next.
            if (_dataManager->readData(&_choices[_choiceIndex].passageIndex, _offset, kPassageIndexSize)) {
               DEBUG("Passage Index: %d", _choices[_choiceIndex].passageIndex);
               _offset += kPassageIndexSize;
               _state = PARSE_CHOICE;
               // Don't parse updates yet, we need user input first.
               //_state = PARSE_UPDATES;
               _choiceIndex++;
               if (!_passage->append) {
                  if (_choiceIndex < _choiceCount) {
                     _hardwareManager->printer()->feed(1);
                  } else {
                     _hardwareManager->printer()->feed(3);
                  }
               }
            } else {
               _state = PARSE_ERROR;
            }
         } else { // Choice Count is next.
            _choiceCount = _dataManager->readByte(_offset);
            _offset++;
            if (_choiceCount > 0) {
               _choices = new Choice[_choiceCount];
               _choiceLinks = new uint8_t[_choiceCount];
               _choiceIndex = 0;
               _visibleCount = 0;
               _state = PARSE_CHOICE;
            } else {
               _hardwareManager->printer()->feed(1);
               _state = PARSE_ENDING;
            }
         }
      }
   } else if (_state == PARSE_CHOICE) {
      _parsingChoices = true;
      if (_choiceIndex < _choiceCount) {
         // Get choice attribute.
         _choices[_choiceIndex].attribute = _dataManager->readByte(_offset);
         _offset++;
         _state = PARSE_CONDITIONALS;
         // Print spacing for the body passage, only makes sense if this is the first choice.
         if (!_passage->append && (_choiceIndex == 0)) {
            _hardwareManager->printer()->feed(2);
            if (_choiceCount > 4) {
               _hardwareManager->printer()->printBigNumbers();
            }
         }
      } else {
         if (_passage->append) {
            DEBUG("Append to next passage!");
            _appended = true;
            _offset = _dataManager->startOffset + _dataManager->getPassageOffset(_choices[0].passageIndex);
            DEBUG("Jumping to passage %d at offset: %lu", _choices[0].passageIndex, _offset);
            cleanupAfterPassage();
            _state = PARSE_UPDATES;
         } else {
            _state = PARSE_USER_INPUT;
         }
      }
   } else if (_state == PARSE_CONDITIONALS) {
      uint32_t processedBytes = 0;
      if ((processedBytes = parseConditions(_offset, _choices[_choiceIndex].visible))) {
         _offset += processedBytes;

         // Track variable update memory offset in case this choice is picked.
         uint16_t updatesLength = 0;
   
         // Is this choice an invisible default choice.
         if (_choices[_choiceIndex].isInvisibleDefault) {
            // Check that this is the last choice.
            if (_choiceIndex == (_choiceCount - 1)) {
               if (_visibleCount == 0) {
                  // Forward to the passage index.
                  _offset += 5;
                  if (_dataManager->readData(&_choices[_choiceIndex].passageIndex, _offset, kPassageIndexSize)) {
                     DEBUG("Append to next passage!");
                     _offset = _dataManager->startOffset + _dataManager->getPassageOffset(_choices[_choiceIndex].passageIndex);
                     DEBUG("Jumping to passage %d at offset: %lu", _choices[_choiceIndex].passageIndex, _offset);

                     cleanupAfterPassage();
                     _state = PARSE_UPDATES;
                     return _state;
                  } else {
                     _state = PARSE_ERROR;
                     return _state;
                  }
               }
            }
         }

         DEBUG("Choice #%d %s visible.", (_choiceIndex + 1), (_choices[_choiceIndex].visible ? "is" : "not"));

         // Read two bytes of update length.
         _dataManager->readData(&updatesLength, _offset, kDataLengthSize);
         _offset += kDataLengthSize;
         // Bookmark offset for value update count.
         _choices[_choiceIndex].updatesOffset = _offset;
         // Move offset to just after value updates.
         _offset += updatesLength + kOperationCountSize;
         if (_choices[_choiceIndex].visible) {
            // Associate choice indexes with visible choice indexes.
            _choiceLinks[_visibleCount] = _choiceIndex;
            _visibleCount++;
            // Parse and print this choices text.
            _state = PARSE_DATA;
         } else {
            // Choice isn't visible, skip over the rest of this choices bytes.
            _dataManager->readData(&_dataLength, _offset, kDataLengthSize);
            _offset += (kDataLengthSize + _dataLength + kPassageIndexSize);
            _dataLength = 0;

            _choiceIndex++;
            if (_choiceIndex == _choiceCount) {
               _hardwareManager->printer()->feed(2);
            }
            _state = PARSE_CHOICE;
         }
      }
   } else if (_state == PARSE_USER_INPUT) {
      if (_dataManager->randomPlay) {
         delay(250);
         uint8_t value = random(1, _visibleCount + 1);
         //_hardwareManager->keypad()->setKeypadEvent(KEYPAD_MULTI_UP_EVENT, value);
         _hardwareManager->keypad()->setPressedValue(value);
      }
      // Once we have a valid choice, execute it's value updates and setup for the next passage.
      // Wait for multi button up event for story selection.
      //_choiceSelected = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, _visibleCount);
      bool success = false;
      _choiceSelected = _hardwareManager->keypad()->multiUpValInRange(success, 1, _visibleCount);
      if (success) {
         _lastIndent = 0;
         // Choice has been selected.
         _offset = _choices[_choiceLinks[_choiceSelected - 1]].updatesOffset;
         _dataManager->turns++;
         _state = PARSE_UPDATES;
      }
   } else if (_state == PARSE_ENDING) {
      // TODO: User ending attribute (contains ending quality).
      uint8_t endingAttr = _dataManager->readByte(_offset);
      if (endingAttr) {}; // Warning treated as error workaround.
      _offset++;
      // TODO: User ending quality.
      uint8_t endingQual = endingAttr & 0x07;
      if (endingQual) {}; // Warning treated as error workaround.
      //DEBUG("Ending Quality: %d", endingQual);
      _appended = false;
      _visibleCount = 0;
      _lastIndent = 0;
      cleanupAfterPassage();
      _hardwareManager->printer()->feed(1);
      _hardwareManager->printer()->printEnding(_dataManager->storyHeader.credits, _dataManager->storyHeader.contact);
      _dataManager->unloadStory();
      _state = PARSE_IDLE;
   } else if (_state == PARSE_ERROR) {
      Errors::setError(E_STORY_PARSE_FAIL);
      //ERROR(Errors::errorString());
   }
   return _state;
}

/* Accessors */

/* Private Methods */

void Parser::cleanupAfterPassage() {
   _parsingChoices = false;

   delete _passage;
   _passage = NULL;
   delete[] _choices;
   _choices = NULL;
   delete[] _choiceLinks;
   _choiceLinks = NULL;
}

uint32_t Parser::parseData(uint32_t aOffset, char* aBuffer, uint16_t aLength) {
   uint32_t offset = aOffset;
   if (!_dataManager->readData(aBuffer, offset, aLength)) {
      return 0;
   }
   if (aBuffer[0] == kAsciiCommandByte) { // Parse Command
      // Offset command byte.
      offset++;

      // Get command length as little endian 2 byte uint.
      uint16_t commandLength = (((uint16_t)aBuffer[offset + 1] << 8) +
                              (uint16_t)aBuffer[offset]);
      offset += 2;

      if (commandLength > kPassageBufferReadSize) {
         Errors::setError(E_DATA_TOO_LARGE_FAIL);
         //ERROR(Errors::errorString());
         return 0;
      }

      // At this point we should have the entire command
      // in the buffer, ready to parse.
      if (!parseCommand(offset, aBuffer, commandLength)) {
         return 0;
      }
      offset += commandLength;
   } else {
      int i = 0;
      for (; i < aLength; ++i) {
         if (aBuffer[i] == kAsciiCommandByte) {
            aBuffer[i] = '\0';
            break;
         }
      }
      offset += i;
   }
   return offset - aOffset;
}

uint32_t Parser::parseCommand(uint32_t aOffset, char* aBuffer, uint16_t aLength) {
   uint32_t offset = aOffset;
   //DEBUG("Parsing command of length: %d");
   return offset - aOffset;;
}

// aOffset: The byte offset to begin processing at.
// Returns: The number of bytes processed beyond the provided offset.
uint32_t Parser::parseValueUpdates(uint32_t aOffset) {
   uint32_t offset = aOffset;
   // 1 byte - Value of how many distinct value updates there are.
   uint8_t count = _dataManager->readByte(offset);
   offset += kOperationCountSize;

   int16_t address = 0;
   int16_t result = 0;
   bool assigned = false;
   for (int i = 0; i < count; ++i) {
      // Jump ahead slightly to get the address of the left operand. This is base of
      // a potentially recursive set of operations, and the only variable to be set.
      _dataManager->readData(&address, offset + kOperationInfoSize, kOperationOperandSize);
      // Get the result of all the operations in result. Add the # of bytes read to the offset.
      offset += parseOperation(offset, result, true, assigned);
      assigned = false;
      // NEW BEHAVIOR: Result should simply indicate true / false.
      // DEBUG("Address: %d, Result: %d", address, result);
      // Set the variable to the result.
      // DEBUG("PreVal: %d", _dataManager->varAtIndex(address));
      // if (!_dataManager->setVarAtIndex(address, result)) {
      //    return 0;
      // }
      // DEBUG("PostVal: %d", _dataManager->varAtIndex(address));
   }

   return offset - aOffset;
}

uint32_t Parser::parseConditions(uint32_t aOffset, int16_t &aResult) {
   uint32_t offset = aOffset;
   // 1 byte - Value of how many distinct value updates there are.
   uint8_t count = _dataManager->readByte(offset);
   offset += kOperationCountSize;

   aResult = 1;
   int16_t result = 0;
   bool assigned = false;
   for (int i = 0; i < count; ++i) {
      offset += parseOperation(offset, result, false, assigned);
      // DEBUG("Final Result: %d, Result: %d", aResult, result);
      aResult = (aResult && result ? 1 : 0);
   }

   return offset - aOffset;
}

uint32_t Parser::parseOperation(uint32_t aOffset, int16_t &aResult, bool aSetVars, bool &aAssigned) {
   int16_t result = 0;
   uint32_t offset = aOffset;
   int16_t leftIdx = -1;

   Operation op;
   _dataManager->readData(&op, offset, kOperationInfoSize);
   // DEBUG("lType: %s, rType: %s, op: %s", kDataTypeStr[op.leftType], kDataTypeStr[op.rightType], kOpStr[op.operationType]);
   offset += kOperationInfoSize;
   if (op.leftType == kOpTypeOperation) {
      offset += parseOperation(offset, op.leftOperand, aSetVars, aAssigned);
   } else {
      _dataManager->readData(&op.leftOperand, offset, kOperationOperandSize);
      offset += kOperationOperandSize;
      if (op.leftType == kOpTypeVar) {
         leftIdx = op.leftOperand;
         // DEBUG("lVar[%d] == %d", leftIdx, _dataManager->varAtIndex(leftIdx));
         op.leftOperand = _dataManager->varAtIndex(leftIdx);
      } else if (op.leftType == kOpTypeRaw) {
         // DEBUG("lVal: %d", op.leftOperand);
      } else if (op.leftType == kOpTypeChoiceCount) {
         // DEBUG("lVis: %d", _visibleCount);
         op.leftOperand = _visibleCount;
      } else if (op.leftType == kOpTypeTurns) {
         // DEBUG("lTurns: %d", _dataManager->turns);
         op.leftOperand = _dataManager->turns;
      } else if (op.leftType == kOpTypePsgVisits) {
         // TODO: Not implemented.
      }
   }

   // Special handling if operation is IF or ELSE.
   if (op.operationType == kOpIfStatement) {
      if (op.leftOperand <= 0) {
         DEBUG("IF - SETVAR == false");
         aSetVars = false;
      }
   } else if (op.operationType == kOpElseStatement) {
      if (!aAssigned) {
         DEBUG("ELSE - NOT ASSIGNED - SETVAR == true");
         aSetVars = true;
      }
   }

   if (op.rightType == kOpTypeOperation) {
      offset += parseOperation(offset, op.rightOperand, aSetVars, aAssigned);
   } else {
      _dataManager->readData(&op.rightOperand, offset, kOperationOperandSize);
      offset += kOperationOperandSize;
      if (op.rightType == kOpTypeVar) {
         // DEBUG("rVar[%d] == %d", op.rightOperand, _dataManager->varAtIndex(op.rightOperand));
         op.rightOperand = _dataManager->varAtIndex(op.rightOperand);
      } else if (op.rightType == kOpTypeRaw) {
         // DEBUG("rVal: %d", op.rightOperand);
      } else if (op.rightType == kOpTypeChoiceCount) {
         // DEBUG("lVis: %d", _visibleCount);
         op.rightOperand = _visibleCount;
      } else if (op.rightType == kOpTypeTurns) {
         // DEBUG("rTurns: %d", _dataManager->turns);
         op.rightOperand = _dataManager->turns;
      } else if (op.rightType == kOpTypePsgVisits) {
         // TODO: Not implemented.
      }
   }

   switch (op.operationType)
   {
      case kOpEqual:
         result = (op.leftOperand == op.rightOperand) ? 1 : 0;
         break;
      case kOpNotEquals:
         result = (op.leftOperand != op.rightOperand) ? 1 : 0;
         break;
      case kOpGreater:
         result = (op.leftOperand > op.rightOperand) ? 1 : 0;
         break;
      case kOpLess:
         result = (op.leftOperand < op.rightOperand) ? 1 : 0;
         break;
      case kOpGreaterThanOrEquals:
         result = (op.leftOperand >= op.rightOperand) ? 1 : 0;
         break;
      case kOpLessThanOrEquals:
         result = (op.leftOperand <= op.rightOperand) ? 1 : 0;
         break;
      case kOpAND:
         result = (op.leftOperand && op.rightOperand) ? 1 : 0;
         break;
      case kOpOR:
         result = (op.leftOperand || op.rightOperand) ? 1 : 0;
         break;
      case kOpXOR:
         result = (!op.leftOperand != !op.rightOperand) ? 1 : 0;
         break;
      case kOpNAND:
         result = (!op.leftOperand || !op.rightOperand) ? 1 : 0;
         break;
      case kOpNOR:
         result = !(op.leftOperand || op.rightOperand) ? 1 : 0;
         break;
      case kOpXNOR:
         result = !(op.leftOperand != op.rightOperand) ? 1 : 0;
         break;
      case kOpChoiceVisible:
         result = 0;
         if ((op.leftOperand > 0) &&
            (op.leftOperand <= _choiceIndex)) {
            if (_choices[op.leftOperand - 1].visible) {
               DEBUG("Choice Idx Vis[%d]: %d", op.leftOperand - 1, _choices[op.leftOperand - 1].visible);
               result = 1;
            }
         }
         break;
      case kOpMod:
         result = op.leftOperand % op.rightOperand;
         break;
      case kOpAssign:
         result = 1;
         if (aSetVars && !aAssigned) {
            if (!_dataManager->setVarAtIndex(leftIdx, op.rightOperand)) {
               result = 0;
            }
            // DEBUG("ASSIGNED: [%d] = %d", leftIdx, op.rightOperand);
            aAssigned = true;
         } else {
            result = 0;
            // DEBUG("NOT ASSIGNED - aSetVars[%d] aAssigned[%d]", aSetVars, aAssigned); 
         }
         break;
      case kOpAdd:
         result = op.leftOperand + op.rightOperand;
         break;
      case kOpSubtract:
         result = op.leftOperand - op.rightOperand;
         break;
      case kOpMultiply:
         result = op.leftOperand * op.rightOperand;
         break;
      case kOpDivide:
         result = op.leftOperand / op.rightOperand;
         break;
      case kOpRandom:
         // Add 1 to max to make it inclusive.
         result = random(op.leftOperand, op.rightOperand + 1);
         break;
      case kOpDice:
         for (int i = 0; i < op.leftOperand; ++i) {
            result += random(1, op.rightOperand + 1);
         }
         break;
      case kOpIfStatement:
         result = 0;
         if (op.leftOperand >= 1) {
            result = 1;
         }
         break;
      case kOpElseStatement:
         result = 0;
         if (op.leftOperand <= 0) {
            result = 1;
         }
         break;
      case kOpNegate:
         // Returns int16_t - positive val becomes negative, negative val becomes positive
         result = -op.leftOperand;
         break;
      case kOpNot:
         // Returns 0 or 1 - if val greater than 0, returns 0, if val is 0 or less, returns 1
         result = 1;
         if (op.leftOperand > 0) {
            result = 0;
         }
         break;
      case kOpMin:
         if (op.leftOperand < op.rightOperand) {
            result = op.leftOperand;
         } else {
            result = op.rightOperand;
         }
         break;
      case kOpMax:
         if (op.leftOperand > op.rightOperand) {
            result = op.leftOperand;
         } else {
            result = op.rightOperand;
         }
         break;
      case kOpPow:
         result = Utils::pow(op.leftOperand, op.rightOperand);
         break;
   }
   aResult = result;
   if (leftIdx == -1) {
      DEBUG("Formula: %d %s %d -> %d", op.leftOperand, kOpStr[op.operationType], op.rightOperand, aResult);
   } else {
      DEBUG("Formula: [%d] %s %d -> %d", leftIdx, kOpStr[op.operationType], op.rightOperand, aResult);
   }
   //DEBUG("Result: %d", aResult);
   return offset - aOffset;
}

}