#ifndef RTK_CONFIG_H
#define RTK_CONFIG_H
#include <Arduino.h>

/******************************************************************************/
//                       Default Serial settings
/******************************************************************************/
//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DEBUG_SERIAL \
  if (DEBUGGING) Serial

#define BAUD                          115200




#endif  /*** RTK_CONFIG_H ***/