/*******************************************************************
*
* @File: Exit.c
* @Purpose: Handle the termination of McTavish
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#include "Exit.h"

extern Configuration config;
extern int socket_fd;

void closeMcTavish() {
     if (disconnectLionel(config, socket_fd) == DISCONNECTION_SUCCESSFUL) {
          safeClose();
     } else {
          write(1, DISCONNECTION_ERROR_MSG, strlen(DISCONNECTION_ERROR_MSG));
     }
}

void safeClose() {
     char *buff;
     int bytes = 0;

     //Show the disconnection message
     bytes = asprintf(&buff, DISCONNECTION_PATTERN, config.name);
     write(1, buff, bytes);
     free(buff);

     //Close the socket
     close(socket_fd);

     //Free the allocated memory
     free(config.name);
     free(config.ip);

     exit(0);
}
