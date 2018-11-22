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

extern Connection conn;
extern Configuration config;

void closeLionel() {
     write(1, SHUT_DOWN_MSG, strlen(SHUT_DOWN_MSG));
     safeClose();
}

void safeClose() {
     int i;

     //Close the file descriptors
     for (i = 0; i < conn.num_mcgruder_processes; i++) {
          if (conn.mcgruder[i].fd != SOCKET_CONNECTION_FAILED) {
               disconnectMcGruder(i);
               free(conn.mcgruder[i].telescope_name);
          }
     }

     for (i = 0; i < conn.num_mctavish_processes; i++) {
          close(conn.mctavish[i].fd);
     }

     close(conn.socket_fd);

     //Free the memory
     free(conn.mcgruder);
     free(conn.mctavish);
     free(config.ip);

     exit(0);
}
