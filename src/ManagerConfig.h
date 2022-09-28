#ifndef RTK_BASE_CONFIG_H
#ifndef MANAGER_CONFIG_H
#define MANAGER_CONFIG_H
#include <Arduino.h>

/*
=================================================================================
                              Serial settings
=================================================================================
*/
//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DBG \
  if (DEBUGGING) Serial

#define BAUD                          115200

/*
=================================================================================
                              RTK settings
=================================================================================
*/
#define COORD_PRECISION               9
#define ALT_PRECISION                 4



#endif  /*** MANAGER_CONFIG_H ***/
#endif