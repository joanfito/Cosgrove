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

Configuration config;
Connection conn;
Files files;

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
                    signal(SIGINT, closeLionel);
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

                    //Initialize and run Paquita
                    response = initPaquita();
                    if (response != PAQUITA_CREATED_OK) {
                        write(1, PAQUITA_CREATION_ERROR_MSG, strlen(PAQUITA_CREATION_ERROR_MSG));
                        closeLionel();
                    }
                    
                    startPaquita();
                    pause();
                }
            }
        }
    }

    return 0;
}
