/*******************************************************************
*
* @File: main.c
* @Purpose: McTavish process of Cosgrove system.
*           Interface for the scientists to obtain
*           Lionel's data.
*           To execute: ./mctavish <config_file>
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include "Configuration.h"
#include "Connection.h"
#include "Menu.h"
#include "Exit.h"

#define INVALID_ARGS_ERROR_MSG "McTavish needs a configuration file in order to work properly\n"
#define CONNECTION_READY_MSG "Connection ready.\n"
#define CONNECTION_ERROR_MSG "Something failed during the connection.\n"
#define CONNECTION_LIONEL_MSG "Connecting to Lionel...\n"
#define STARTING_PATTERN "Starting %s.\n"

Configuration config;
int socket_fd;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        write(1, INVALID_ARGS_ERROR_MSG, strlen(INVALID_ARGS_ERROR_MSG));
        return -1;
    } else {
        config = readConfiguration(argv[1]);
        if (!invalidConfig(config)) {
            char *buff;
            int bytes = 0;

            printConfig(config);
            bytes = asprintf(&buff, STARTING_PATTERN, config.name);
            write(1, buff, bytes);
            free(buff);

            //Start the connection with Lionel
            write(1, CONNECTION_LIONEL_MSG, strlen(CONNECTION_LIONEL_MSG));
            socket_fd = connectLionel(config);

            if (socket_fd != CONNECTION_FAILED) {
                int option = 0;

                write(1, CONNECTION_READY_MSG, strlen(CONNECTION_READY_MSG));

                signal(SIGINT, closeMcTavish);
                
                while (endMenu(option) == 0) {
                    //Ask the option
                    option = selectOption();

                    //Execute the selected option
                    executeOption(option);
                }
                closeMcTavish();
            } else {
                free(config.name);
                free(config.ip);
                write(1, CONNECTION_ERROR_MSG, strlen(CONNECTION_ERROR_MSG));
            }
        }
    }

    return 0;
}
