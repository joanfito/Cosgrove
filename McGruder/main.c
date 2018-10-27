/*******************************************************************
*
* @File: main.c
* @Purpose: McGruder process of Cosgrove system.
*           Read data from the telescopes and send it to Lionel
*           To execute: ./mcgruder <config_file>
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include "Configuration.h"
#include "Connection.h"
#include "Files.h"
#include "Exit.h"

#define INVALID_ARGS_ERROR_MSG "McGruder needs a configuration file in order to work properly\n"
#define CONNECTION_READY_MSG "Connection ready.\n"
#define CONNECTION_ERROR_MSG "Something failed during the connection.\n"
#define WAITING_MSG "Waiting...\n"
#define TESTING_FILES_MSG "Testing files...\n"
#define CONNECTION_LIONEL_MSG "Connecting to Lionel...\n"
#define STARTING_PATTERN "Starting %s.\n"

Configuration config;

void searchAlarm() {
     write(1, TESTING_FILES_MSG, strlen(TESTING_FILES_MSG));
     scanDirectory(config);
     alarm(config.search_time);
}

int main(int argc, char *argv[]) {
     if (argc < 2) {
          write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
          return -1;
     } else {
          config = readConfiguration(argv[1]);
          if (!invalidConfig(config)) {
               char buff[100];
               int bytes = 0, socket_fd;

               printConfig(config);
               bytes = sprintf(buff, STARTING_PATTERN, config.name);
               write(1, buff, bytes);

               //Start the connection with Lionel
               write(1, CONNECTION_LIONEL_MSG, strlen(CONNECTION_LIONEL_MSG));
               socket_fd = connectLionel(config);

               if (socket_fd != CONNECTION_FAILED) {
                    write(1, CONNECTION_READY_MSG, strlen(CONNECTION_READY_MSG));
                    copyConfigAndFd(config, socket_fd);
                    signal(SIGINT, closeMcGruder);
                    signal(SIGALRM, searchAlarm);
                    alarm(config.search_time);

                    while (1) {
                         //Scan the files folder of the telescope
                         write(1, WAITING_MSG, strlen(WAITING_MSG));
                         pause();
                    }
               } else {
                    write(1, CONNECTION_ERROR_MSG, strlen(CONNECTION_ERROR_MSG));
               }
          }

     }

     return 0;
}
