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

Connection conn;

void closeLionel() {
     write(1, SHUT_DOWN_MSG, strlen(SHUT_DOWN_MSG));
     safeClose();
}

void safeClose() {
     int i;

     //Close the file descriptors
     close(conn.socket_fd);

     for (i = 0; i < conn.num_mcgruder_processes; i++) {
          close(conn.mcgruder[i].fd);
     }

     for (i = 0; i < conn.num_mctavish_processes; i++) {
          close(conn.mctavish[i].fd);
     }

     //Free the memory
     free(conn.mcgruder);
     free(conn.mctavish);

     exit(0);
}

void copyConnection(Connection original) {
     conn = original;
}
