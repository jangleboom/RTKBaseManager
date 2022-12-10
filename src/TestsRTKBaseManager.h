#ifndef TESTS_RTK_BASE_MANAGER_H
#define TESTS_RTK_BASE_MANAGER_H

#include <AUnit.h>
#include <RTKBaseManager.h>

using namespace aunit;
using namespace RTKBaseManager;

test(getLowerPrecisionCoordFromDouble_Pos) 
{
    double input = 12.345678999;
    int32_t lowerPrecPart = getLowerPrecisionCoordFromDouble(input);
    assertTrue(lowerPrecPart == 123456789);
}

test(getLowerPrecisionCoordFromDouble_Neg) 
{
    double input = -12.345678999;
    int32_t lowerPrecPart = getLowerPrecisionCoordFromDouble(input);
    assertTrue(lowerPrecPart == -123456789);
}

test(getHighPrecisionCoordFromDouble_Pos) 
{
    double input = 12.345678999;
    int8_t higherPrecPart = getHighPrecisionCoordFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getHighPrecisionCoordFromDouble_Neg) 
{
    double input = -12.345678999;
    int8_t higherPrecPart = getHighPrecisionCoordFromDouble(input);
    assertTrue(higherPrecPart == 99);
}

test(getDoubleCoordFromIntegerParts_Pos) 
{
    int32_t lowerPrecPart = 123456789;
    int8_t higherPrecPart= 99;
    double d_val_test = 12.345678999;
    double double_var = getDoubleCoordFromIntegerParts(lowerPrecPart, higherPrecPart);
    assertLess(abs(double_var) - d_val_test, 0.000000001);
}

test(getDoubleCoordFromIntegerParts_Neg) 
{
    int32_t val = -123456789;
    int8_t valHp = 99;      // high precision extension
    double d_val_test = -12.345678999;
    double d_val = getDoubleCoordFromIntegerParts(val, valHp);
    assertLess(abs(d_val) - abs(d_val_test), 0.000000001);
}

test(getFloatingPointStringFromCSV_Coord) 
{
    String csvStr = "123456789,99";
    String doubleStr = "12.345678999";
    String testStr = getFloatingPointStringFromCSV(csvStr, COORD_PRECISION);
    assertTrue (testStr.equals(doubleStr));
}

test(getFloatingPointStringFromCSV_Alt) 
{
    String csvStr = "12345,6";
    String floatStr = "12.3456";
    String testStr = getFloatingPointStringFromCSV(csvStr, ALT_PRECISION);
    assertTrue (testStr.equals(floatStr));
}

test(getValueAsStringFromCSV) 
{
    bool result = true;
    String csv = "123456789,99";
    const char SEP = ',';
    const int8_t LOWER_VAL_IDX = 0;
    const int8_t HIGHER_VAL_IDX = 1;
    result &= getValueAsStringFromCSV(csv, SEP, LOWER_VAL_IDX).equals("123456789");
    result &= getValueAsStringFromCSV(csv, SEP, HIGHER_VAL_IDX).equals("99");
    assertTrue(result);
}

test(getDeconstructedCoordAsCSV) 
{
    String doubleStr= "12.345678999";
    double dVal = doubleStr.toDouble();
    int32_t lowerPrec = getLowerPrecisionCoordFromDouble(dVal);
    int8_t highPrec = getHighPrecisionCoordFromDouble(dVal);
    String deconstructedCSV = String(lowerPrec) + SEP + String(highPrec);
    String testString = getDeconstructedCoordAsCSV(doubleStr);

    assertTrue(deconstructedCSV.equals(testString));
}

test(getDeconstructedAltAsCSV) 
{
    String floatStr= "12.3456";
    float fVal = floatStr.toDouble();
    int32_t lowerPrec = getLowerPrecisionIntAltitudeFromFloat(fVal);
    int8_t highPrec = getHigherPrecisionIntAltitudeFromFloat(fVal);
    String deconstructedCSV = String(lowerPrec) + SEP + String(highPrec);
    String testString = getDeconstructedAltAsCSV(floatStr);
    assertTrue(deconstructedCSV.equals(testString));
}

test(getLocationFromLittleFS) 
{
    bool success = true;
    location_t location;
    const int32_t lowerPrecCoord = 123456789;
    const int8_t highPrecCoord = 99;
    const int32_t lowerPrecAlt= 12345;
    const int8_t highPrecAlt = 6;
    const float accuracy = 0.05;
    String doubleCoordStr = "12.345678999";
    String floatAltStr = "12.3456";
    String floatAccStr = "0.05";
    const char* kTestLatPath = "/testLat.txt";
    const char* kTestLonPath = "/testLon.txt";
    const char* kTestAltPath = "/testAlt.txt";
    const char* kTestAccPath = "/testAcc.txt";

    if (LittleFS.exists(kTestLatPath)) LittleFS.remove(kTestLatPath);
    if (LittleFS.exists(kTestLonPath)) LittleFS.remove(kTestLonPath);
    if (LittleFS.exists(kTestAltPath)) LittleFS.remove(kTestAltPath);
    if (LittleFS.exists(kTestAccPath)) LittleFS.remove(kTestAccPath);
   
    String deconstructedCoordAsCSV = getDeconstructedCoordAsCSV(doubleCoordStr);
    String deconstructedAltAsCSV = getDeconstructedAltAsCSV(floatAltStr);
    String accuracyString = String(accuracy);
    success &= writeFile(LittleFS, kTestLatPath, deconstructedCoordAsCSV.c_str());
    success &= writeFile(LittleFS, kTestLonPath, deconstructedCoordAsCSV.c_str());
    success &= writeFile(LittleFS, kTestAltPath, deconstructedAltAsCSV.c_str());
    success &= writeFile(LittleFS, kTestAccPath, accuracyString.c_str());

    success &= getLocationFromLittleFS(&location, kTestLatPath, kTestLonPath, kTestAltPath, kTestAccPath);
    success &= location.lat == lowerPrecCoord;
    success &= location.lat_hp == highPrecCoord;
    success &= location.lon == lowerPrecCoord;
    success &= location.lon_hp == highPrecCoord;
    success &= location.alt == lowerPrecAlt;
    success &= location.alt_hp == highPrecAlt;
    success &= abs(location.acc - accuracy) < 0.0001;

    assertTrue(success);
}

test(getLowerPrecisionIntAltitudeFromFloat) 
{
    float alt = 12.3456;
    int32_t result = getLowerPrecisionIntAltitudeFromFloat(alt);
    assertEqual( result, 12345);
}

test(getHigherPrecisionIntAltitudeFromFloat) 
{
    float alt = 12.3456;
    int8_t result = getHigherPrecisionIntAltitudeFromFloat(alt);
    assertEqual( result, 6);
}

test(getDigitsCount) 
{
    int32_t num = 12345;
    int8_t digits = getDigitsCount(num);
    assertEqual(digits, 5);
}

test(getFloatAltitudeFromInt) 
{
    int32_t alt = 12345;
    int8_t altHp = 6;
    float result = getFloatAltitudeFromInt(alt, altHp) - 12.3456;
    assertLess(abs(result), 0.0001);
}

test(processorWriteToLittleFS) 
{
    String testValue = "TestValue";
    const char* kTestValuePath = "/testValue.txt";
    if (LittleFS.exists(kTestValuePath)) LittleFS.remove(kTestValuePath);
    writeFile(LittleFS, kTestValuePath, testValue.c_str());
    delay(100);
    String savedValue = readFile(LittleFS, kTestValuePath);
    assertTrue(savedValue.equals(testValue));
}

test(getPath)
{
    const char testValue[]  = "testValue";
    String testValuePath    = "/testValue.txt";
    String result           = getPath(testValue);

    assertTrue(result.equals(testValuePath));
}
#endif /*** TESTS_RTK_BASE_MANAGER_H ***/