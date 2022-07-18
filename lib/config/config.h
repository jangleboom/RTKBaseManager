#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

/******************************************************************************/
//                       Default Serial settings
/******************************************************************************/
//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DEBUG_SERIAL \
  if (DEBUGGING) Serial

#define BAUD                          115200

#ifdef DEBUGGING
#include <tests.h>
#endif


#endif