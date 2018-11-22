/*******************************************************************
*
* @File: Configuration.c
* @Purpose: Process the configuration file
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
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
                    config.ip = malloc(sizeof(char) * (strlen(output) + 1));
                    strcpy(config.ip, output);
                    break;
              case 1:
                    config.mcgruder_port = atoi(output);
                    break;
              case 2:
                    config.mctavish_port = atoi(output);
                    break;
              case 3:
                    config.seconds = atoi(output);
                    break;
         }
         line_counter++;
         free(output);
    }

    close(fd);
    return config;
}

Configuration configError() {
     Configuration error;
     error.ip = "0.0.0.0";
     error.mcgruder_port = CONFIG_ERROR;
     error.mctavish_port = CONFIG_ERROR;
     error.seconds = CONFIG_ERROR;
     return error;
}

int invalidConfig(Configuration config) {
     return config.mcgruder_port == CONFIG_ERROR;
}

void printConfig(Configuration config) {
     char buff[100];
     int bytes;

     write(1, PRINT_CONFIG_INI, strlen(PRINT_CONFIG_INI));
     bytes = sprintf(buff, "\t-> IP: %s\n", config.ip);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> Port for McGruder: %d\n", config.mcgruder_port);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> Port for McTavish: %d\n", config.mctavish_port);
     write(1, buff, bytes);
     bytes = sprintf(buff, "\t-> Seconds: %d\n", config.seconds);
     write(1, buff, bytes);
     write(1, PRINT_CONFIG_END, strlen(PRINT_CONFIG_END));
}
