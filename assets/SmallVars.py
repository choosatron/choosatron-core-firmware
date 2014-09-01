# custom data type script

def parseByteRange(element, byteView, bitPos, bitLength, results):
    # this method parses data starting at bitPos, bitLength bits are remaining
    """parseByteRange method"""

    # how many bytes were processed?
    processedBytes = 0
    iteration = 0
    # we are only working with bytes
    startPos = bytePos = bitPos / 8

    # Find the number of vars to read.
    result = results.getResultByName("SmallVars")
    if result is not None:
        varCountPos = result.getStartBytePos()
        varCount = result.getByteView().readUnsignedInt(varCountPos, 1, ENDIAN_LITTLE)

        defaultResult = results.addStructureStart(element.getEnclosingStructure(), startPos, 0, "Small Defaults", True)

        index = 0
        for index in range(0, varCount):
            value = NumberValue()
            value.setUnsigned(byteView.readUnsignedInt(bytePos, 1, ENDIAN_LITTLE)
            processedBytes += 1
            bytePos += 1
            results.addElement(element, processedBytes, iteration, value)

    results.addStructureEnd(bytePos)

    # return number of processed bytes
    return processedBytes

def fillByteRange(value, byteArray, bitPos, bitLength):
    # this method translates edited values back to the file
    """fillByteRange method"""

    # write an integer back to file
    # byteArray.writeUnsignedIntBits(highWord, bitPos, bitLength, ENDIAN_LITTLE)
