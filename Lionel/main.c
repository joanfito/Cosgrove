/*******************************************************************
*
* @File: main.c
* @Purpose: Lionel process of Cosgrove system.
*           Central process of the system where all the data is stored
*           To execute: ./lionel <config_file>
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include <string.h>
#include <stdlib.h>
#include "Configuration.h"
#include "Connection.h"
#include "Exit.h"
#include "Files.h"

#define INVALID_ARGS_ERROR_MSG "Lionel needs a configuration file in order to work properly\n"
#define STARTING_MSG "Starting Lionel.\n"
#define ERROR_SHARED_MEM_MSG "An error occur while creating the shared memory regions\n"

Configuration config;
Connection conn;
Files files;
semaphore sem_sync_paquita, sem_file, sem_received;
int id_received_data, id_last_data, id_last_file, id_file;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
        return -1;
    } else {
        config = readConfiguration(argv[1]);
        if (!invalidConfig(config)) {
            printConfig(config);
            write(1, STARTING_MSG, strlen(STARTING_MSG));
            conn = createConnection();
            conn.mcgruder_fd = createSocketForMcGruder(config);
            conn.mctavish_fd = createSocketForMcTavish(config);

            if (conn.mcgruder_fd == SOCKET_CONNECTION_FAILED) {
                write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
                free(conn.mcgruder);
                free(conn.mctavish);
                free(config.ip);
            } else {
                if (conn.mctavish_fd == SOCKET_CONNECTION_FAILED) {
                    close(conn.mcgruder_fd);
                    write(1, CONNECTION_MCTAVISH_ERROR_MSG, strlen(CONNECTION_MCTAVISH_ERROR_MSG));
                    free(conn.mcgruder);
                    free(conn.mctavish);
                    free(config.ip);
                } else {
                    files = createFiles();

                    //Create the threads to listen both McGruder socket and McTavish socket
                    pthread_t server_mcgruder, server_mvtavish;
                    int response;

                    response = pthread_create(&server_mcgruder, NULL, mcgruderServer, NULL);
                    if (response != 0) {
                        write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
                        closeLionel();
                    }

                    response = pthread_create(&server_mvtavish, NULL, mctavishServer, NULL);
                    if (response != 0) {
                        write(1, CONNECTION_MCTAVISH_ERROR_MSG, strlen(CONNECTION_MCTAVISH_ERROR_MSG));
                        closeLionel();
                    }

                    //Create the shared memory region for the received data
                    id_received_data = shmget(IPC_PRIVATE, sizeof(ReceivedData), IPC_CREAT | IPC_EXCL | 0600);

                    if (id_received_data == -1) {
                         write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
                         closeLionel();
                    }

                    //Create the shared memory region for the last astronomical data
                    id_last_data = shmget(IPC_PRIVATE, sizeof(ReceivedAstronomicalData), IPC_CREAT | IPC_EXCL | 0600);

                    if (id_last_data == -1) {
                        shmctl(id_received_data, IPC_RMID, NULL);
                        write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
                        closeLionel();
                    }

                    //Create the shared memory region for the name of the file
                    id_last_file = shmget(IPC_PRIVATE, sizeof(LastFile), IPC_CREAT | IPC_EXCL | 0600);

                    if (id_last_file == -1) {
                        shmctl(id_received_data, IPC_RMID, NULL);
                        shmctl(id_last_data, IPC_RMID, NULL);
                        write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
                        closeLionel();
                    }

                    //Create the shared memory region for data of the astronomical data file
                    id_file = shmget(IPC_PRIVATE, sizeof(char) * 1000, IPC_CREAT | IPC_EXCL | 0600);

                    if (id_file == -1) {
                        shmctl(id_received_data, IPC_RMID, NULL);
                        shmctl(id_last_data, IPC_RMID, NULL);
                        shmctl(id_last_file, IPC_RMID, NULL);
                        write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
                        closeLionel();
                    }

                    int paquita;

                    paquita = fork();

                    switch (paquita) {
                         case 0:
                              //Initialize and run Paquita
                              response = initPaquita();
                              if (response != PAQUITA_CREATED_OK) {
                                  write(1, PAQUITA_CREATION_ERROR_MSG, strlen(PAQUITA_CREATION_ERROR_MSG));
                                  closeLionel();
                              }

                              startPaquita();
                              break;
                         default:
                              //Create the semaphores
                              SEM_constructor_with_name(&sem_received, ftok("Paquita.c", 29));
                              SEM_constructor_with_name(&sem_file, ftok("Paquita.c", 33));
                              SEM_constructor_with_name(&sem_sync_paquita, ftok("Paquita.c", 74));

                              //Reprogram the signal
                              signal(SIGINT, closeLionel);

                              wait(0);
                              break;
                    }
                }
            }
        }
    }

    return 0;
}
