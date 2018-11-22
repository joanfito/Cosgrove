/*******************************************************************
*
* @File: Files.h
* @Purpose: Handle the termination of McGruder
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Exit.h"

extern Configuration config;
extern int socket_fd;

void closeMcGruder() {
     if (disconnectLionel(config, socket_fd) == DISCONNECTION_SUCCESSFUL) {
          safeClose();
     } else {
          write(1, DISCONNECTION_ERROR_MSG, strlen(DISCONNECTION_ERROR_MSG));
     }
}

void safeClose() {
     char buff[100];
     int bytes = 0;
     
     bytes = sprintf(buff, DISCONNECTION_PATTERN, config.name);
     write(1, buff, bytes);

     close(socket_fd);
     free(config.name);
     free(config.ip);

     exit(0);
}
