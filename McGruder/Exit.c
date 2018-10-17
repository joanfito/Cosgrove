/*******************************************************************
*
* @File: Files.h
* @Purpose: Handle the termination of McGruder
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

#include "Exit.h"

Configuration config;

void closeMcGruder() {
     char buff[100];
     int bytes = 0;
     bytes = sprintf(buff, DISCONNECTION_PATTERN, config.name);
     write(1, buff, bytes);

     if (disconnect() == DISCONNECTION_SUCCESSFUL) {
          safeClose();
     } else {
          write(1, DISCONNECTION_ERROR_MSG, strlen(DISCONNECTION_ERROR_MSG));
     }
}

void safeClose() {
     exit(0);
}

void copyConfig(Configuration original) {
     config = original;
}
