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
#define CONNECTION_MCGRUDER_ERROR_MSG "Something failed during the connection with McGruder.\n"
#define CONNECTION_MCTAVISH_ERROR_MSG "Something failed during the connection with McTavish.\n"
#define WAITING_MSG "Waiting...\n"

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
            int aux_fd;

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

                    while (1) {
                        write(1, WAITING_MSG, strlen(WAITING_MSG));

                        //Connect with the McGruder clients
                        aux_fd = acceptMcGruder(conn.mcgruder_fd);

                        if (aux_fd != MC_GRUDER_ACCEPT_FAILED) {
                            conn.mcgruder[conn.num_mcgruder_processes].fd = aux_fd;
                            conn.num_mcgruder_processes++;
                            conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
                        } else {
                            write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
                        }

                        if (connectMcGruder(conn.num_mcgruder_processes - 1) == CONNECT_MCGRUDER_KO) {
                            //If something goes wrong, we close the socket and remove the process
                            close(conn.mcgruder[conn.num_mcgruder_processes].fd);
                            conn.num_mcgruder_processes--;
                            conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
                        }
                        /*
                        //Connect with the McTavish clients
                        aux_fd = acceptMcTavish(conn.mctavish_fd);

                        if (aux_fd != MC_TAVISH_ACCEPT_FAILED) {
                            conn.mctavish[conn.num_mctavish_processes].fd = aux_fd;
                            conn.num_mctavish_processes++;
                            conn.mctavish = (McTavish*) realloc(conn.mctavish, sizeof(McTavish) * (conn.num_mctavish_processes + 1));
                        } else {
                            write(1, CONNECTION_MCTAVISH_ERROR_MSG, strlen(CONNECTION_MCTAVISH_ERROR_MSG));
                        }

                        if (connectMcTavish(conn.num_mctavish_processes - 1) == CONNECT_MCTAVISH_KO) {
                            //If something goes wrong, we close the socket and remove the process
                            close(conn.mctavish[conn.num_mctavish_processes].fd);
                            conn.num_mctavish_processes--;
                            conn.mctavish = (McTavish*) realloc(conn.mctavish, sizeof(McTavish) * (conn.num_mctavish_processes + 1));
                        }*/
                    }
                }
            }
        }
    }

    return 0;
}
