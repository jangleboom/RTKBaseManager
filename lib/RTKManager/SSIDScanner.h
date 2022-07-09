#ifndef SSID_SCANNER_H
#define SSID_SCANNER_H
#include <Arduino.h>

class SSIDScanner
{
private:
    String _foundSSIDs[10];
    void _scan();
public:
    SSIDScanner();
    ~SSIDScanner();
    
    bool ssidAvailable(const String& ssid);
};




#endif /*** SSID_SCANNER_H ***/