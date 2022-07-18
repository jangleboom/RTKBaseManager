#include <AUnit.h>
#include <RTKBaseManager.h>
#include <SPIFFS.h>
using namespace aunit;
using namespace RTKBaseManager;

test(correct) {
    int x = 1;
    assertEqual(x, 1);
}

test(incorrect) {
    int x = 2;
    assertNotEqual(x, 1);
}

test(getLowerPrecisionPartFromDouble) {
    double input = 12.345678999;
    uint8_t lowerPrecPart = getHighPrecisionPartFromDouble(input);
    assertTrue(lowerPrecPart == 123456789);
}

test(getHighPrecisionPartFromDouble) {
    double input = 12.345678999;
    uint8_t higherPrecPart = getHighPrecisionPartFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getDoubleFromIntegerParts) {
    int32_t lowerPrecPart = 123456789;
    uint8_t higherPrecPart= 99;
    double double_var = getDoubleFromIntegerParts(lowerPrecPart, higherPrecPart);
    assertLess(double_var - 12.345678999, 0.000000001);
}

test(getReconstructedValStringFromCSV) {
    String csvStr = "111234567,89";
    String doubleStr = "11.123456789";
    String testStr = getReconstructedValStringFromCSV(csvStr);
    assertTrue (testStr.equals(doubleStr));
}