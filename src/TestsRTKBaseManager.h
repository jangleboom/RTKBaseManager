#ifndef TESTS_RTK_BASE_MANAGER_H
#define TESTS_RTK_BASE_MANAGER_H

#include <AUnit.h>
#include <RTKBaseManager.h>

using namespace aunit;
using namespace RTKBaseManager;

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
    success &= writeFile(SPIFFS, testPathAlt, deconstructedValAsCSV.c_str());

    success &= getIntLocationFromSPIFFS(&location, testPathLat, testPathLon, testPathAlt);
    success &= location.lat == lowerPrec;
    success &= location.lon == lowerPrec;
    success &= location.alt == lowerPrec;
    success &= location.lat_hp == highPrec;
    success &= location.lon_hp == highPrec;
    success &= location.alt_hp == highPrec;

    assertTrue(success);
}

#endif /*** TESTS_RTK_BASE_MANAGER_H ***/