/*******************************************************************
*
* @File: Exit.c
* @Purpose: Handle the termination of Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Exit.h"

extern Connection conn;
extern Configuration config;
extern Files files;
extern int id_received_data, id_last_data, id_last_file, id_file;
extern semaphore sem_sync_paquita, sem_file, sem_received;

void closeLionel() {
     write(1, SHUT_DOWN_MSG, strlen(SHUT_DOWN_MSG));
     safeClose();
}

void safeClose() {
     int i, files_saved;

     //Dump the received files into the log files
     files_saved = saveReceivedFiles(files);

     if (files_saved == KALKUN_SAVED_OK) {
         //Close Lionel if the files are saved
         //Close the mcgruders
         for (i = 0; i < conn.num_mcgruder_processes; i++) {
              if (conn.mcgruder[i].fd != SOCKET_CONNECTION_FAILED) {
                   disconnectMcGruder(i);
              }
         }

         //Close the mctavishes
         for (i = 0; i < conn.num_mctavish_processes; i++) {
              if (conn.mctavish[i].fd != SOCKET_CONNECTION_FAILED) {
                   disconnectMcTavish(i);
              }
         }

         //Close the sockets
         close(conn.mcgruder_fd);
         close(conn.mctavish_fd);

         //Free the allocated memory
         free(conn.mcgruder);
         free(conn.mctavish);
         free(config.ip);
         free(files.images);
         free(files.astronomical_data);

         //Erease the shared memory regions
         shmctl(id_received_data, IPC_RMID, NULL);
         shmctl(id_last_data, IPC_RMID, NULL);
         shmctl(id_last_file, IPC_RMID, NULL);
         shmctl(id_file, IPC_RMID, NULL);

         //Destroy the semaphores
         SEM_destructor(&sem_sync_paquita);
         SEM_destructor(&sem_file);
         SEM_destructor(&sem_received);

         exit(0);
     } else {
         write(1, SHUT_DOWN_ERROR_MSG, strlen(SHUT_DOWN_ERROR_MSG));
     }
}
