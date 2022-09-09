#ifndef TESTS_RTK_BASE_MANAGER_H
#define TESTS_RTK_BASE_MANAGER_H

#include <AUnit.h>
#include <RTKBaseManager.h>

using namespace aunit;
using namespace RTKBaseManager;

test(getLowerPrecisionPartFromDouble_Pos) {
    double input = 12.345678999;
    int32_t lowerPrecPart = getLowerPrecisionPartFromDouble(input);
    assertTrue(lowerPrecPart == 123456789);
}

test(getLowerPrecisionPartFromDouble_Neg) {
    double input = -12.345678999;
    int32_t lowerPrecPart = getLowerPrecisionPartFromDouble(input);
    assertTrue(lowerPrecPart == -123456789);
}

test(getHighPrecisionPartFromDouble_Pos) {
    double input = 12.345678999;
    int8_t higherPrecPart = getHighPrecisionPartFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getHighPrecisionPartFromDouble_Neg) {
    double input = -12.345678999;
    int8_t higherPrecPart = getHighPrecisionPartFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getDoubleFromIntegerParts_Pos) {
    int32_t lowerPrecPart = 123456789;
    int8_t higherPrecPart= 99;
    double d_val_test = 12.345678999;
    double double_var = getDoubleFromIntegerParts(lowerPrecPart, higherPrecPart);
    assertLess(abs(double_var) - d_val_test, 0.000000001);
}

test(getDoubleFromIntegerParts_Neg) {
    int32_t val = -123456789;
    int8_t valHp = 99;      // high precision extension
    double d_val_test = -12.345678999;
    double d_val = getDoubleFromIntegerParts(val, valHp);
    assertLess(abs(d_val) - abs(d_val_test), 0.000000001);
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

test(getIntLocationFromSPIFFS) {
    bool success = true;
    location_int_t location;
    const int32_t lowerPrec = 123456789;
    const int8_t highPrec = 99;
    String doubleValStr = "12.345678999";
    const char* testPathLat = "/testPathLat";
    const char* testPathLon = "/testPathLon";
    const char* testPathAlt = "/testPathAlt";

    if (SPIFFS.exists(testPathLat)) SPIFFS.remove(testPathLat);
    if (SPIFFS.exists(testPathLon)) SPIFFS.remove(testPathLon);
    if (SPIFFS.exists(testPathAlt)) SPIFFS.remove(testPathAlt);
   
    String deconstructedValAsCSV = getDeconstructedValAsCSV(doubleValStr);
    success &= writeFile(SPIFFS, testPathLat, deconstructedValAsCSV.c_str());
    success &= writeFile(SPIFFS, testPathLon, deconstructedValAsCSV.c_str());
    success &= writeFile(SPIFFS, testPathAlt, String(lowerPrec).c_str());

    success &= getIntLocationFromSPIFFS(&location, testPathLat, testPathLon, testPathAlt);
    success &= location.lat == lowerPrec;
    success &= location.lat_hp == highPrec;
    success &= location.lon == lowerPrec;
    success &= location.lon_hp == highPrec;
    success &= location.ellips == lowerPrec;

    assertTrue(success);
}

test(getLowerPrecisionIntAltitudeFromFloat) {
    float alt = 12.3456;
    int32_t result = getLowerPrecisionIntAltitudeFromFloat(alt);
    assertEqual( result, 12345);
}

test(getHigherPrecisionIntAltitudeFromFloat) {
    float alt = 12.3456;
    int8_t result = getHigherPrecisionIntAltitudeFromFloat(alt);
    assertEqual( result, 6);
}

test(getDigitsCount) {
    int32_t num = 12345;
    int8_t digits = getDigitsCount(num);
    assertEqual(digits, 5);
}

test(getFloatAltitudeFromInt) {
    int32_t alt = 12345;
    int8_t altHp = 6;
    float result = getFloatAltitudeFromInt(alt, altHp) - 12.3456;
    assertLess(abs(result), 0.0001);
}

#endif /*** TESTS_RTK_BASE_MANAGER_H ***/