#ifndef TESTS_H
#define TESTS_H

#include <AUnit.h>

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
    int32_t lowerPrecPart = getLowerPrecisionPartFromDouble(input);
    assertTrue(lowerPrecPart == 123456789);
}

test(getHighPrecisionPartFromDouble) {
    double input = 12.345678999;
    int8_t higherPrecPart = getHighPrecisionPartFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getDoubleFromIntegerParts) {
    int32_t lowerPrecPart = 123456789;
    int8_t higherPrecPart= 99;
    double double_var = getDoubleFromIntegerParts(lowerPrecPart, higherPrecPart);
    assertLess(double_var - 12.345678999, 0.000000001);
}

test(getDoubleStringFromCSV) {
    String csvStr = "123456789,99";
    String doubleStr = "12.345678999";
    String testStr = getDoubleStringFromCSV(csvStr);
    assertTrue (testStr.equals(doubleStr));
}

test(getValueAsStringFromCSV) {
    bool result = true;
    String csv = "123456789,99";
    result &= getValueAsStringFromCSV(csv, ',',0).equals("123456789");
    result &= getValueAsStringFromCSV(csv, ',',1).equals("99");
    assertTrue(result);
}

#endif /*** TESTS_H ***/