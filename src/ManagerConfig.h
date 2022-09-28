#ifndef RTKBASE_CONFIG_H
#ifndef MANAGER_CONFIG_H
#define MANAGER_CONFIG_H
#include <Arduino.h>

/*******************************************************************************
 *                         WiFi settings
 * ****************************************************************************/
const uint8_t MAX_SSIDS = 10; // Space to scan and remember SSIDs

/******************************************************************************/
//                       Default Serial settings
/******************************************************************************/
//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DBG \
  if (DEBUGGING) Serial

#define BAUD                          115200
#define COORD_PRECISION               9
#define ALT_PRECISION                 4



#endif  /*** MANAGER_CONFIG_H ***/
#endif