#include "cdam_parser.h"
#include "cdam_manager.h"

namespace cdam
{

/* Public Methods */

void Parser::initialize() {
	_dataManager = Manager::getInstance().dataManager;
	_hardwareManager = Manager::getInstance().hardwareManager;
	_state = PARSE_NONE;
}

void Parser::initStory(uint8_t aStoryIndex) {
	_dataManager->loadStory(aStoryIndex);
	_offset = _dataManager->startOffset;
	_state = PARSE_PSG_UPDATES;
}

/*void Parser::parsePassage() {
	if (_state == PARSE_PSG_UPDATES) {

	} else if (_state == PARSE_BODY_TEXT) {

	} else if (_state == PARSE_BODY_CMD) {

	}
}*/

bool Parser::parsePassage() {
	// First process any passage body value updates.
	if ((_offset = parseValueUpdates(_offset))) {
		// Get the length of the body in bytes.
		uint32_t bodyLength = 0;
		if (_dataManager->readData(&bodyLength, _offset, kBodyLengthSize)) {
			_offset += kBodyLengthSize;
			DEBUG("Body Length: %lu", bodyLength);
			// Parse and print the body from the provided byte offset with the given length.
			if ((_offset = parseAndPrintData(_offset, bodyLength))) {
				// Get the number of choices, 0 meaning an ending.
				uint8_t choiceCount = _dataManager->readByte(_offset);
				_offset++;
				DEBUG("Choice Count: %d", choiceCount);
				if (choiceCount == 0) {
					uint8_t endingAttr = _dataManager->readByte(_offset);
					_offset++;
					uint8_t endingQual = endingAttr & 0x07;
					DEBUG("Ending Quality: %d", endingQual);
				} else {
					_choices = new Choice[choiceCount];
					_choiceIndex = new uint8_t[choiceCount];
					_visibleChoices = 0;
					for (int i = 0; i < choiceCount; ++i) {
						//Choice* choice = new Choice;
						_offset = parseChoice(_offset, &_choices[i], _visibleChoices);
						// If NULL, choice isn't visible so we don't need it.
						if (_choices[i].visible) {
							_choiceIndex[_visibleChoices] = i;
							_visibleChoices++;
							//_choices.push_back(*choice);
						}
					}
				}
			}
		}
	}

	if (_offset == 0) {
		Errors::setError(E_STORY_PARSE_FAIL);
		ERROR(Errors::errorString());
		return false;
	}
	return true;
}

uint32_t Parser::parseValueUpdates(uint32_t aOffset) {
	uint8_t count = 0;
	if (_dataManager->readData(&count, aOffset, kValueSetCountSize)) {
		DEBUG("Update Count: %d", count);
		aOffset += kValueSetCountSize;
		for (int i = 0; i < count; ++i) {
			ValueSet valueSet;
			if (_dataManager->readData(&valueSet, aOffset, kValueSetSize)) {
				aOffset += kValueSetSize;
				int16_t valueOne = 0;
				int16_t valueTwo = 0;
				if (valueSet.varOneType == kValueTypeSmall) {
					DEBUG("Var One Small");
					valueOne = _dataManager->smallVarAtIndex(valueSet.varOne);
				} else if (valueSet.varOneType == kValueTypeBig) {
					DEBUG("Var One Big");
					valueOne = _dataManager->bigVarAtIndex(valueSet.varOne);
				}
				DEBUG("Var One Index: %d", valueSet.varOne);

				if (valueSet.varTwoType == kValueTypeRaw) {
					DEBUG("Var Two Raw");
					valueTwo = (int16_t)valueSet.varTwo;
				} else if (valueSet.varTwoType == kValueTypeSmall) {
					DEBUG("Var Two Small");
					valueTwo = _dataManager->smallVarAtIndex(valueSet.varTwo);
				} else if (valueSet.varTwoType == kValueTypeBig) {
					DEBUG("Var Two Big");
					valueTwo = _dataManager->bigVarAtIndex(valueSet.varTwo);
				}

				if (valueSet.operatorType == kOperatorEquals) {
					DEBUG("Operator: Equals");
					valueOne = valueTwo;
				} else if (valueSet.operatorType == kOperatorPlus) {
					DEBUG("Operator: Plus");
					valueOne += valueTwo;
				} else if (valueSet.operatorType == kOperatorMinus) {
					DEBUG("Operator: Minus");
					valueOne -= valueTwo;
				} else if (valueSet.operatorType == kOperatorMultiply) {
					DEBUG("Operator: Multiply");
					valueOne *= valueTwo;
				} else if (valueSet.operatorType == kOperatorDivide) {
					DEBUG("Operator: Divide");
					valueOne /= valueTwo;
				} else if (valueSet.operatorType == kOperatorModulus) {
					DEBUG("Operator: Modulus");
					valueOne %= valueTwo;
				}
				
				if (valueSet.varTwoType == kValueTypeRaw) {
					DEBUG("Var Two Value: %d", (int16_t)valueSet.varTwo);
				} else {
					DEBUG("Var Two Index: %d", valueSet.varTwo);
				}

				if (valueSet.varOneType == kValueTypeSmall) {
					DEBUG("Var Two Small");
					if (!_dataManager->setSmallVarAtIndex(valueSet.varOne, (int8_t)valueOne)) {
						return 0;
					}
				} else if (valueSet.varOneType == kValueTypeBig) {
					DEBUG("Var Two Big");
					if (!_dataManager->setBigVarAtIndex(valueSet.varOne, valueOne)) {
						return 0;
					}
				}
			} else {
				return 0;
			}
		}
	} else {
		return 0;
	}
	return aOffset;
}

uint32_t Parser::parseConditions(uint32_t aOffset, bool aResult) {
	uint8_t count = 0;
	if (_dataManager->readData(&count, aOffset, kValueSetCountSize)) {
		DEBUG("Update Count: %d", count);
		aOffset += kValueSetCountSize;
		aResult = true;
		bool result = true;
		for (int i = 0; i < count; ++i) {
			ValueSet valueSet;
			if (_dataManager->readData(&valueSet, aOffset, kValueSetSize)) {
				aOffset += kValueSetSize;
				int16_t valueOne = 0;
				int16_t valueTwo = 0;
				if (valueSet.varOneType == kValueTypeSmall) {
					DEBUG("Var One Small");
					valueOne = _dataManager->smallVarAtIndex(valueSet.varOne);
				} else if (valueSet.varOneType == kValueTypeBig) {
					DEBUG("Var One Big");
					valueOne = _dataManager->bigVarAtIndex(valueSet.varOne);
				}
				DEBUG("Var One Index: %d", valueSet.varOne);

				if (valueSet.varTwoType == kValueTypeRaw) {
					DEBUG("Var Two Raw");
					valueTwo = (int16_t)valueSet.varTwo;
				} else if (valueSet.varTwoType == kValueTypeSmall) {
					DEBUG("Var Two Small");
					valueTwo = _dataManager->smallVarAtIndex(valueSet.varTwo);
				} else if (valueSet.varTwoType == kValueTypeBig) {
					DEBUG("Var Two Big");
					valueTwo = _dataManager->bigVarAtIndex(valueSet.varTwo);
				}

				if (valueSet.operatorType == kCompareEqual) {
					DEBUG("Operator: Equals");
					result = (valueOne == valueTwo) ? true : false;
				} else if (valueSet.operatorType == kCompareGreater) {
					DEBUG("Operator: Greater Than");
					result = (valueOne > valueTwo) ? true : false;
				} else if (valueSet.operatorType == kCompareLess) {
					DEBUG("Operator: Less Than");
					result = (valueOne < valueTwo) ? true : false;
				} else if (valueSet.operatorType == kCompareEqualGreater) {
					DEBUG("Operator: Equal or Greater");
					result = (valueOne >= valueTwo) ? true : false;
				} else if (valueSet.operatorType == kCompareEqualLess) {
					DEBUG("Operator: Equal or Less");
					result = (valueOne <= valueTwo) ? true : false;
				} else if (valueSet.operatorType == kCompareModulus) {
					DEBUG("Operator: Modulus (has remainder)");
					result = (valueOne %= valueTwo) ? true : false;
				}
				
				if (valueSet.varTwoType == kValueTypeRaw) {
					DEBUG("Var Two Value: %d", (int16_t)valueSet.varTwo);
				} else {
					DEBUG("Var Two Index: %d", valueSet.varTwo);
				}

				DEBUG("Condition: %s", (result ? "true" : "false"));
				// If we have logic gates, we would evalutate here.
				aResult = (aResult ? result : false);
			} else {
				return 0;
			}
		}
	} else {
		return 0;
	}
	return aOffset;
}

uint32_t Parser::parseBody(uint32_t aOffset) {
	uint32_t bodyLength = 0;
	if (_dataManager->readData(&bodyLength, aOffset, kBodyLengthSize)) {
		aOffset += kBodyLengthSize;
		DEBUG("Body Length: %lu", bodyLength);
		aOffset = parseAndPrintData(aOffset, bodyLength);
	}
	return 0;
}


uint32_t Parser::parseChoice(uint32_t aOffset, Choice* aChoice, uint8_t aVisibleChoices) {
	aChoice->attribute = _dataManager->readByte(aOffset);
	aOffset++;
	DEBUG("Psg #: %d, Append: %s", _dataManager->psgIndex, (aChoice->append ? "on" : "off"));
	aOffset = parseConditions(aOffset, aChoice->visible);
	DEBUG("Choice %s visible.", (aChoice->visible ? "is" : "not"));
	uint16_t choiceLength = 0;
	if (_dataManager->readData(&choiceLength, aOffset, kChoiceLengthSize)) {
		DEBUG("Choice Length: %lu", choiceLength);
		aOffset += kChoiceLengthSize;
		if (aChoice->visible) {
			aOffset = parseAndPrintData(aOffset, choiceLength);
		} else {
			aOffset += choiceLength;
		}
	}

	return aOffset;
}

// aOffset: The byte offset to begin reading from.
// aLength: The length of the total body of data in bytes.
uint32_t Parser::parseAndPrintData(uint32_t aOffset, uint32_t aLength) {
	uint32_t bytesProcessed = 0;
	DEBUG("Offset: %lu, Length: %lu", aOffset, aLength);
	while (bytesProcessed < aLength) {
		uint16_t bytesToRead = ((aLength - bytesProcessed) < kPassageBufferReadSize) ? (aLength - bytesProcessed) : kPassageBufferReadSize;
		char buffer[bytesToRead + 1];
		memset(&buffer[0], 0, sizeof(buffer));
		if (_dataManager->readData(&buffer, aOffset, bytesToRead)) {
			//bytesProcessed += bytesToRead;

			uint16_t i = 0;
			bool commandFound = false;
			for (; i < bytesToRead; ++i) {
				if (buffer[i] == kAsciiCommandByte) {
					commandFound = true;
					// Overwrite the command byte with string ending.
					buffer[i] = '\0';
					i++; // Move to the next index, the command length.
					// Wrap the text up to the command.
					wrapText(buffer, kPrinterColumns);
					// Print the text up to the command.
					DEBUG("PRINT TO CMD: %s", buffer);
					_hardwareManager->printer()->print(buffer);

					uint16_t commandLength = 0;
					// Enough bytes remaining in buffer to get command length?
					if ((i + 1) < bytesToRead) {
						// Get command length as little endian 2 byte uint.
						commandLength = (((uint16_t)buffer[i + 1] << 8) +
					                          (uint16_t)buffer[i]);
						// Add length bytes.
						i += 2;
					} else {
						if (!_dataManager->readData(&commandLength, aOffset + i, 2)) {
							return 0;
						}
						// Add length bytes.
						i += 2;
						// Whatever 'i' is now, we are done with the current buffer.
						bytesToRead = i;
					}
					DEBUG("Command Length: %d", commandLength);
					if (commandLength > kPassageBufferReadSize) {
						Errors::setError(E_DATA_TOO_LARGE_FAIL);
						ERROR(Errors::errorString());
						return 0;
					}
					// If command is longer than remaining bytes...
					if (commandLength > (bytesToRead - i)) {
						memset(&buffer[0], 0, sizeof(buffer));
						_dataManager->readData(&buffer, aOffset + i, commandLength);
						/*if ((bytesToRead - i) > 0) {
							memcpy(buffer, buffer + i, bytesToRead - i);
						}*/
					}
					if (!parseCommand(buffer, commandLength)) {
						return 0;
					}
					memset(&buffer[0], 0, sizeof(buffer));
					i += commandLength;
				}
			}
			if (!commandFound) {
				// Wrap the text up to the command.
				wrapText(buffer, kPrinterColumns);
				// Print the text up to the command.
				DEBUG("PRINT: %s", buffer);
				_hardwareManager->printer()->print(buffer);
			}
			bytesProcessed += i;
			aOffset += i;
		} else {
			return 0;
		}
	}
	_hardwareManager->printer()->feed(1);
	return aOffset;
}

/*void Parser::parseData(char* aBuffer, uint16_t aBufferSize, uint32_t aLength) {
	DEBUG("String: %s", aBuffer);
	//for (uint16_t i = 0; aBuffer[i] != '\0'; ++i) {

	uint16_t printIndex = 0;
	for (uint16_t i = 0; i < aLength; ++i) {
		DEBUG("Char: %c", aBuffer[i]);
		if (aBuffer[i] == kAsciiCommandByte) {
			// Overwrite the command byte with string ending.
			aBuffer[i] = '\0';
			// Wrap the text up to the command.
			wrapText(aBuffer, kPrinterColumns);
			// Print the text up to the command.
			_hardwareManager->printer()->println(aBuffer + printIndex);
			
			// Get command length as little endian 2 byte uint.
			uint16_t commandLength = (((uint16_t)aBuffer[i + 2] << 8) +
			                          (uint16_t)aBuffer[i + 1]);
			DEBUG("Command Length: %d", commandLength);
			// Send a pointer to the byte after the
			// command byte, the command type.
			

			//printIndex = parseCommand(aBuffer + i + 1)
		}
	}
}*/

bool Parser::parseCommand(char* aBuffer, uint16_t aLength) {
	DEBUG("Parsing command of length: %d");
	return true;
}

uint8_t Parser::wrapText(char* aBuffer, uint8_t aColumns) {
	//DEBUG("Text: %s, Columns: %d", aBuffer, aColumns);
	uint16_t length = strlen(aBuffer);
	//DEBUG("Length: %d", length);

	// Should always represent the first char index of a line.
	uint16_t startIndex = 0;
	bool foundBreak = false;
	// Wrap until we have less than one full line.
	while ((length - startIndex) > aColumns) {
		DEBUG("Start wrapping...");
		foundBreak = false;
		uint8_t i;
  		// Search for a newline to break on.
		for (i = 0; i < aColumns; ++i) {
			if (aBuffer[startIndex + i] == '\t') {
				aBuffer[startIndex + i] = ' ';
      			WARN("Found tab.");
			} else if (aBuffer[startIndex + i] == '\r') {
      			WARN("Found carriage return.");
			} else if (aBuffer[startIndex + i] == '\n') {
				DEBUG("Found newline.");
				foundBreak = true;
				startIndex += i + 1;
				break;
			}
		}

  		// Search for a space to break on.
		if (foundBreak == false) {
			DEBUG("Looking for a space...");
			for (i = aColumns; i > 0; --i) {
				if (aBuffer[startIndex + i] == ' ') {
					DEBUG("Found space at: %d", i);
					foundBreak = true;
					aBuffer[startIndex + i] = '\n';
					startIndex += i + 1;
					break;
				}
			}
			if (foundBreak == false) {
				startIndex += aColumns;
			}
		}
	}
	DEBUG("Last bit length: %d", length - startIndex);
	return length - startIndex;
}

/* Accessors */

/* Private Methods */

}