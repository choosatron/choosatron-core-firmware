# Text Body - Used for passage body text and choice text.

def parseByteRange(element, byteView, bitPos, bitLength, results):
    # this method parses data starting at bitPos, bitLength bits are remaining
    """parseByteRange method"""

    #lengthResult = results.getPrevResult()
    #print lengthResult.getName()
    #lengthBytes = 4
    #if lengthResult == NULL:
    #   lengthResult = results.getResultByName("ChoiceLength")
    #   lengthBytes = 2
    #lengthPos = lengthResult.getStartBytePos()
    #length = lengthResult.getByteView().readUnsignedInt(lengthPos, lengthBytes, ENDIAN_LITTLE)
    #print "Length: " + str(length)

    textStr = ""
    # how many bytes were processed?
    processedBytes = 0
    iteration = 0
    # we are only working with bytes
    startPos = bytePos = bitPos / 8
    
    processedBytes = 0
    byteLength = bitLength / 8

    bodyResult = results.addStructureStart(element.getEnclosingStructure(), startPos, 0, "Body", True)

    while (bytePos - startPos) < byteLength:
        byteRead = byteView.readByte(bytePos)
        print("Byte: " + chr(byteRead))
        bytePos += 1
        processedBytes += 1
        print "Position: " + str(bytePos - startPos) + ", Total Length: " + str(byteLength)
        if byteRead == 0x1A or byteRead == 0x05 or (bytePos - startPos) == byteLength:
            if len(textStr) > 0:
                if (bytePos - startPos) == byteLength:
                    textStr += chr(byteRead)
                    processedBytes += 1
                textValue = StringValue()
                textValue.setString(textStr)
                print(textStr)
                textStr = ""
                textElement = Element(1, "Text", True)
                results.addElement(textElement, processedBytes - 1, iteration, textValue)
                # The currently read byte is part of a SUB or CMD
                processedBytes = 1
                if (bytePos - startPos) == byteLength:
                    print("End of Text")
                    bodyResult = results.addStructureEnd(bytePos)

        # SUB or Substitute byte means a word lookup, 3 byte address
        if byteRead == 0x1A:
            print("Starting SUB")
            subByteElement = Element(1, "Start Substitute", True)
            subValue = StringValue()
            subValue.setString("SUB: 1A")
            results.addElement(subByteElement, 1, iteration, subValue)
            address = byteView.readUnsignedInt(bytePos, 3, ENDIAN_LITTLE)
            addressVal = NumberValue()
            addressVal.setName("Lookup Address")
            addressVal.setUnsigned(address)
            bytePos += 3
            subElement = Element(2, "Substitution", True)
            results.addElement(subElement, 3, iteration, addressVal)
            processedBytes = 0
        elif byteRead == 0x05:
            print("Starting CMD")
            cmdStartElement = Element(1, "Start Command", True)
            cmdStartValue = StringValue()
            cmdStartValue.setString("ENQ: 05")
            results.addElement(cmdStartElement, 1, iteration, cmdStartValue)
            commandType = byteView.readByte(bytePos)
            bytePos += 1
            cmdTypeElement = Element(2, "Command Type", True)
            cmdTypeValue = NumberValue()
            cmdTypeValue.setUnsigned(commandType)
            results.addElement(cmdTypeElement, 1, iteration, cmdTypeValue)
            cmdBody = ""
            processedBytes = 0
            while byteRead != 0x06:
                byteRead = byteView.readByte(bytePos)
                if byteRead != 0x06:
                    bytePos += 1
                    processedBytes += 1
                    cmdBody += chr(byteRead)
            if len(cmdBody) > 0:
                commandVal = StringValue()
                commandVal.setName("Command Body")
                commandVal.setString(cmdBody)
                cmdElement = Element(1, "Command", True)
                results.addElement(cmdElement, processedBytes, iteration, commandVal)
            bytePos += 1
            cmdEndElement = Element(1, "End Command", True)
            cmdEndValue = StringValue()
            cmdEndValue.setString("ACK: 06")
            results.addElement(cmdEndElement, 1, iteration, cmdEndValue)
            processedBytes = 0
        else:
            textStr += chr(byteRead)

    #if (bytePos - startPos) == length:
    #   results.addStructureEnd(bytePos)
    #   print "Processed Bytez: " + str(bytePos - startPos)
    return bytePos - startPos


def fillByteRange(value, byteArray, bitPos, bitLength):
    # this method translates edited values back to the file
    """fillByteRange method"""