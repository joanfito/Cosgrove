/*******************************************************************
*
* @File: Exit.c
* @Purpose: Handle the termination of McGruder
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Exit.h"

extern Connection conn;
extern Configuration config;
extern Files files;
extern int id_received_data, id_last_data;

void closeLionel() {
     write(1, SHUT_DOWN_MSG, strlen(SHUT_DOWN_MSG));
     safeClose();
}

void safeClose() {
     int i, files_saved;

     files_saved = saveReceivedFiles(files);
     if (files_saved == KALKUN_SAVED_OK) {
         //Close Lionel if the files are saved
         //Close the mcgruders
         for (i = 0; i < conn.num_mcgruder_processes; i++) {
              if (conn.mcgruder[i].fd != SOCKET_CONNECTION_FAILED) {
                   disconnectMcGruder(i);
                   free(conn.mcgruder[i].telescope_name);
              }
         }

         //Close the mctavishes
         for (i = 0; i < conn.num_mctavish_processes; i++) {
              if (conn.mctavish[i].fd != SOCKET_CONNECTION_FAILED) {
                   disconnectMcTavish(i);
                   free(conn.mctavish[i].scientist_name);
              }
         }

         close(conn.mcgruder_fd);
         close(conn.mctavish_fd);

         //Free the memory
         free(conn.mcgruder);
         free(conn.mctavish);
         free(config.ip);
         free(files.images);
         free(files.astronomical_data);

         //Erease the shared memory regions
         shmctl(id_received_data, IPC_RMID, NULL);
         shmctl(id_last_data, IPC_RMID, NULL);

         //Destroy the semaphores

         exit(0);
     } else {
         write(1, SHUT_DOWN_ERROR_MSG, strlen(SHUT_DOWN_ERROR_MSG));
     }
}
