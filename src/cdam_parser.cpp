#include "cdam_parser.h"
#include "cdam_manager.h"

namespace cdam
{

/* Public Methods */

void Parser::initialize() {
	_dataManager = Manager::getInstance().dataManager;
	_hardwareManager = Manager::getInstance().hardwareManager;
	_state = PARSE_IDLE;
}

void Parser::initStory(uint8_t aStoryIndex) {
	_dataManager->loadStory(aStoryIndex);
	_offset = _dataManager->startOffset;
	_state = PARSE_UPDATES;
	_appended = false;
	// The story number should then print out in front of the first passage.
	_choiceSelected = _dataManager->currentStory;
	_parsingChoices = false;
	_dataLength = 0;

	_hardwareManager->printer()->printStoryIntro(_dataManager->storyHeader.title, _dataManager->storyHeader.author);
	/*uint8_t authorLen = strlen(_dataManager->storyHeader.author);
	char *author = new char[authorLen + 1]();
	memcpy(author, _dataManager->storyHeader.author, authorLen);
	_hardwareManager->printer()->wrapText(author, kPrinterColumns);
	_hardwareManager->printer()->printAuthor(author);
	delete[] author;*/
}

ParseState Parser::parsePassage() {
	if (_state == PARSE_UPDATES) {
		uint32_t processedBytes = 0;
		if ((processedBytes = parseValueUpdates(_offset))) {
			_offset += processedBytes;
			// Only occurs if a choice has been selected.
			if (_parsingChoices) {
				// Choice - 1 for index, get actual index from _choiceLinks which
				// compensates for potentially invisible choices.
				//DEBUG("Choice selected: %d, link index: %d", _choiceSelected, _choiceLinks[_choiceSelected - 1]);
				Choice choice = _choices[_choiceLinks[_choiceSelected - 1]];
				_offset = _dataManager->startOffset + _dataManager->getPassageOffset(choice.passageIndex);
				//DEBUG("Jumping to passage %d at offset: %lu", choice.passageIndex, _offset);
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
			    (_parsingChoices && !_choices[_choiceIndex].append)) {
				_lastIndent = 0;
				// Setup padding for printing a number in front of the text.
				choiceData = _parsingChoices ? _visibleCount : _choiceSelected;
				if (choiceData > 0) {
					bufferPadding = (_choiceSelected == 10) ? 4 : 3;
				}
			} else {
				_appended = false;
			}
			// Get the length of the data bytes.
			if (!_dataManager->readData(&_dataLength, _offset, kDataLengthSize)) {
				_state = PARSE_ERROR;
			}
			_offset += kDataLengthSize;
		}
		// Allocate a buffer of data length, up to a maximum.
		uint16_t bufferSize = (_dataLength < (kPassageBufferReadSize + _lastIndent)) ? _dataLength : (kPassageBufferReadSize + _lastIndent);

		_buffer = new char[bufferSize + bufferPadding + 1](); // + 1 to null terminate if it is all text.
		// Insert the numbering.
		if (dataStart) {
			sprintf(_buffer, "%d. ", choiceData);
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
			DEBUG("%s", _buffer);
			_offset += processedBytes;
			_dataLength -= processedBytes;
		}

		if (_dataLength == 0) {
			delete[] _buffer;
			_buffer = NULL;
			if (_parsingChoices) { // Passage Index is next.
				if (_dataManager->readData(&_choices[_choiceIndex].passageIndex, _offset, kPassageIndexSize)) {
					//DEBUG("Passage Index: %d", _choices[_choiceIndex].passageIndex);
					_offset += kPassageIndexSize;
					_choices[_choiceIndex].updatesOffset = _offset;
					//DEBUG("Updates Offset for choice #%d: %lu", _choiceIndex, _choices[_choiceIndex].updatesOffset);
					// Jump past the value updates for the choice.
					uint8_t updateCount = _dataManager->readByte(_offset);
					_offset += (updateCount * kValueSetSize) + kValueSetCountSize;
					_state = PARSE_CHOICE;
					// Don't parse updates yet, we need user input first.
					//_state = PARSE_UPDATES;
					_choiceIndex++;
					if (!_choices[_choiceIndex - 1].append) {
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
			if (!_choices[_choiceIndex].append && (_choiceIndex == 0)) {
				_hardwareManager->printer()->feed(2);
				if (_choiceCount > 4) {
					_hardwareManager->printer()->printBigNumbers();
				}
			}
		} else {
			if ((_choiceCount == 1) && _choices[0].append) {
				//DEBUG("Append to next passage!");
				_appended = true;
				_offset = _dataManager->startOffset + _dataManager->getPassageOffset(_choices[0].passageIndex);
				//DEBUG("Jumping to passage %d at offset: %lu", _choices[0].passageIndex, _offset);
				cleanupAfterPassage();
				_hardwareManager->printer()->print(" ");
				_lastIndent++;
				_state = PARSE_UPDATES;
			} else {
				_state = PARSE_USER_INPUT;
			}
		}
	} else if (_state == PARSE_CONDITIONALS) {
		uint32_t processedBytes = 0;
		if ((processedBytes = parseConditions(_offset, _choices[_choiceIndex].visible))) {
			_offset += processedBytes;
			//DEBUG("Choice #%d %s visible.", (_choiceIndex + 1), (_choices[_choiceIndex].visible ? "is" : "not"));
			if (_choices[_choiceIndex].visible) {
				_choiceLinks[_visibleCount] = _choiceIndex;
				_visibleCount++;
				_state = PARSE_DATA;
			} else {
				_dataManager->readData(&_dataLength, _offset, kDataLengthSize);
				_offset += (kDataLengthSize + _dataLength + kPassageCountSize);
				_dataLength = 0;
				uint8_t updateCount = _dataManager->readByte(_offset);
				_offset += (updateCount * kValueSetSize) + kValueSetCountSize;
				_choiceIndex++;
				if (_choiceIndex == _choiceCount) {
					_hardwareManager->printer()->feed(2);
				}
				_state = PARSE_CHOICE;
			}
		}
	} else if (_state == PARSE_USER_INPUT) {
		// Once we have a valid choice, execute it's value updates and setup for the next passage.
		// Wait for multi button up event for story selection.
		_choiceSelected = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, _visibleCount);
		if (_choiceSelected) {
			// Choice has been selected.
			_offset = _choices[_choiceLinks[_choiceSelected - 1]].updatesOffset;
			_state = PARSE_UPDATES;
		}
	} else if (_state == PARSE_ENDING) {
		uint8_t endingAttr = _dataManager->readByte(_offset);
		_offset++;
		uint8_t endingQual = endingAttr & 0x07;
		DEBUG("Ending Quality: %d", endingQual);
		_hardwareManager->printer()->feed(1);
		_hardwareManager->printer()->printEnding(_dataManager->storyHeader.credits, _dataManager->storyHeader.contact);
		_state = PARSE_IDLE;
	} else if (_state == PARSE_ERROR) {
		Errors::setError(E_STORY_PARSE_FAIL);
		ERROR(Errors::errorString());
	}
	return _state;
}

/* Accessors */

/* Private Methods */

void Parser::cleanupAfterPassage() {
	_parsingChoices = false;

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
			ERROR(Errors::errorString());
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
	DEBUG("Parsing command of length: %d");
	return offset - aOffset;;
}

// aOffset: The byte offset to begin processing at.
// Returns: The number of bytes processed beyond the provided offset.
uint32_t Parser::parseValueUpdates(uint32_t aOffset) {
	//DEBUG("Updates Offset: %lu", aOffset);
	uint32_t offset = aOffset;
	uint8_t count = _dataManager->readByte(offset);
	//DEBUG("Update Count: %d", count);
	offset += kValueSetCountSize;
	for (int i = 0; i < count; ++i) {
		ValueSet valueSet;
		valueSet.varTypes = _dataManager->readByte(offset);
		_dataManager->readData(&valueSet.varOne, offset + 1, 2);
		_dataManager->readData(&valueSet.varTwo, offset + 3, 2);
		offset += kValueSetSize;
		int16_t valueOne = 0;
		int16_t valueTwo = 0;
		if (valueSet.varOneType == kValueTypeSmall) {
			DEBUG("Var One Small");
			valueOne = _dataManager->smallVarAtIndex(valueSet.varOne);
		} else if (valueSet.varOneType == kValueTypeBig) {
			DEBUG("Var One Big");
			valueOne = _dataManager->bigVarAtIndex(valueSet.varOne);
		}
		DEBUG("Var One Index: %d, Value: %d", valueSet.varOne, valueOne);

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
			DEBUG("Var Two Index: %d, Value: %d", valueSet.varTwo, valueTwo);
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
	}
	return offset - aOffset;
}

uint32_t Parser::parseConditions(uint32_t aOffset, bool &aResult) {
	DEBUG("Updates Offset: %lu", aOffset);
	uint32_t offset = aOffset;
	uint8_t count = _dataManager->readByte(offset);
	offset += kValueSetCountSize;
	DEBUG("Condition Count: %d, Offset: %lu", count, offset);
	aResult = true;
	bool result = true;
	for (int i = 0; i < count; ++i) {
		ValueSet valueSet;
		valueSet.varTypes = _dataManager->readByte(offset);
		_dataManager->readData(&valueSet.varOne, offset + 1, 2);
		_dataManager->readData(&valueSet.varTwo, offset + 3, 2);
		offset += kValueSetSize;
		int16_t valueOne = 0;
		int16_t valueTwo = 0;
		if (valueSet.varOneType == kValueTypeSmall) {
			DEBUG("Var One Small");
			valueOne = _dataManager->smallVarAtIndex(valueSet.varOne);
		} else if (valueSet.varOneType == kValueTypeBig) {
			DEBUG("Var One Big");
			valueOne = _dataManager->bigVarAtIndex(valueSet.varOne);
		}
		DEBUG("Var One Index: %d, Value: %d", valueSet.varOne, valueOne);

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
			DEBUG("Var Two Index: %d, Value: %d", valueSet.varTwo, valueTwo);
		}

		DEBUG("Condition: %s", (result ? "true" : "false"));
		// If we have logic gates, we would evalutate here.
		aResult = (aResult ? result : false);
	}
	return offset - aOffset;
}

/*bool Parser::parsePassage() {
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
}*/

/*uint32_t Parser::parseBody(uint32_t aOffset) {
	uint32_t bodyLength = 0;
	if (_dataManager->readData(&bodyLength, aOffset, kBodyLengthSize)) {
		aOffset += kBodyLengthSize;
		DEBUG("Body Length: %lu", bodyLength);
		aOffset = parseAndPrintData(aOffset, bodyLength);
	}
	return 0;
}*/


/*uint32_t Parser::parseChoice(uint32_t aOffset, Choice* aChoice, uint8_t aVisibleChoices) {
	uint32_t offset = aOffset;
	aChoice->attribute = _dataManager->readByte(offset);
	offset++;
	DEBUG("Psg #: %d, Append: %s", _dataManager->psgIndex, (aChoice->append ? "on" : "off"));
	offset += parseConditions(offset, aChoice->visible);
	DEBUG("Choice %s visible.", (aChoice->visible ? "is" : "not"));
	uint16_t choiceLength = 0;
	if (_dataManager->readData(&choiceLength, offset, kChoiceLengthSize)) {
		DEBUG("Choice Length: %lu", choiceLength);
		offset += kChoiceLengthSize;
		if (aChoice->visible) {
			offset = parseAndPrintData(aOffset, choiceLength);
		} else {
			offset += choiceLength;
		}
	}

	return offset - aOffset;
}*/

// aOffset: The byte offset to begin reading from.
// aLength: The length of the total body of data in bytes.
/*uint32_t Parser::parseData(uint32_t aOffset, uint16_t aBufferSize) {
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
						//if ((bytesToRead - i) > 0) {
						//	memcpy(buffer, buffer + i, bytesToRead - i);
						//}
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
}*/

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

}