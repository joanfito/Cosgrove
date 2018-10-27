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

#define INVALID_ARGS_ERROR_MSG "Lionel needs a configuration file in order to work properly\n"
#define STARTING_MSG "Starting Lionel.\n"
#define CONNECTION_MCGRUDER_ERROR_MSG "Something failed during the connection with McGruder.\n"
#define WAITING_MSG "Waiting...\n"

Configuration config;
Connection conn;

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
               conn.socket_fd = createSocketForMcGruder(config);

               if (conn.socket_fd == SOCKET_CONNECTION_FAILED) {
                    write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
               } else {
                    copyConnection(conn);
                    signal(SIGINT, closeLionel);

                    while (1) {
                         //Connect with the McGruder clients
                         write(1, WAITING_MSG, strlen(WAITING_MSG));
                         aux_fd = acceptMcGruder(conn.socket_fd);

                         if (aux_fd != MC_GRUDER_ACCEPT_FAILED) {
                              conn.mcgruder[conn.num_mcgruder_processes].fd = aux_fd;
                              conn.num_mcgruder_processes++;
                              conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
                         } else {
                              write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
                         }
                         if (connectMcGruder(&conn.mcgruder[conn.num_mcgruder_processes - 1]) == CONNECT_MCGRUDER_KO) {
                              //If something goes wrong, we close the socket and remove the process
                              close(conn.mcgruder[conn.num_mcgruder_processes].fd);
                              conn.num_mcgruder_processes--;
                              conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
                         }
                    }
               }

          }

     }

     return 0;
}
