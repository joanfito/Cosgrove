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

Configuration config;
int socket_fd;

void closeMcGruder() {
     char buff[100];
     int bytes = 0;
     bytes = sprintf(buff, DISCONNECTION_PATTERN, config.name);
     write(1, buff, bytes);

     if (disconnectLionel(config, socket_fd) == DISCONNECTION_SUCCESSFUL) {
          safeClose();
     } else {
          write(1, DISCONNECTION_ERROR_MSG, strlen(DISCONNECTION_ERROR_MSG));
     }
}

void safeClose() {
     close(socket_fd);
     exit(0);
}

void copyConfigAndFd(Configuration original_conf, int original_socket) {
     config = original_conf;
     socket_fd = original_socket;
}
