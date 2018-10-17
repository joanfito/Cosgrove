/*******************************************************************
*
* @File: Configuration.c
* @Purpose: Process the configuration file
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 16/10/2018
*
********************************************************************/

#include "Configuration.h"

Configuration readConfiguration(char *filename) {
     Configuration config;
     int endOfFile = 0, line_counter = 0;
     char *output;

     //Open the configuration file
     int fd = open(filename, O_RDONLY);
     if (fd < 0) {
        write(1, OPEN_FILE_ERROR, strlen(OPEN_FILE_ERROR));
        return configError();
    }

    //Read the file
    while (!endOfFile) {
         output = readLine(fd, '\n', &endOfFile);

         //If the file has more than 4 lines, we ignore them
         switch (line_counter) {
              case 0:
                    config.name = output;
                    break;
              case 1:
                    config.search_time = atoi(output);
                    break;
              case 2:
                    config.ip = output;
                    break;
              case 3:
                    config.port = atoi(output);
                    break;
         }
         line_counter++;
    }

    //Create the configuration

    close(fd);
    return config;
}

Configuration configError() {
     Configuration error;
     error.name = "ERROR";
     error.search_time = CONFIG_ERROR;
     error.ip = "0.0.0.0";
     error.port = CONFIG_ERROR;
     return error;
}

int invalidConfig(Configuration config) {
     return config.port == CONFIG_ERROR;
}

void printConfig(Configuration config) {
     char buff[100];
     int bytes;

     write(1, PRINT_CONFIG_INI, strlen(PRINT_CONFIG_INI));
     bytes = sprintf(buff, "\t-> Name: %s\n", config.name);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> Search time: %d\n", config.search_time);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> IP: %s\n", config.ip);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> Port: %d\n", config.port);
     write(1, buff, bytes);
     write(1, PRINT_CONFIG_END, strlen(PRINT_CONFIG_END));
}
