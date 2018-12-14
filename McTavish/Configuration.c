/*******************************************************************
*
* @File: Configuration.c
* @Purpose: Process the configuration file
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
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

         //If the file has more than 3 lines, we ignore them
         switch (line_counter) {
              case 0:
                    config.name = malloc(sizeof(char) * (strlen(output) + 1));
                    strcpy(config.name, output);
                    break;
              case 1:
                    config.ip = malloc(sizeof(char) * (strlen(output) + 1));
                    strcpy(config.ip, output);

                    config.is_ip = isIp(config.ip);
                    break;
              case 2:
                    config.port = atoi(output);
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
     error.name = "ERROR";
     error.ip = "0.0.0.0";
     error.is_ip = CONFIG_ERROR;
     error.port = CONFIG_ERROR;
     return error;
}

int invalidConfig(Configuration config) {
     return config.port == CONFIG_ERROR;
}

void printConfig(Configuration config) {
     char *buff;
     int bytes;

     write(1, PRINT_CONFIG_INI, strlen(PRINT_CONFIG_INI));
     bytes = asprintf(&buff, "\t-> Scientist name: %s\n", config.name);
     write(1, buff, bytes);
     free(buff);

     if (config.is_ip == 1) {
          bytes = asprintf(&buff, "\t-> IP: %s\n", config.ip);
          write(1, buff, bytes);
          free(buff);
     } else {
          bytes = asprintf(&buff, "\t-> Host name: %s\n", config.ip);
          write(1, buff, bytes);
          free(buff);
     }

     bytes = asprintf(&buff, "\t-> Port: %d\n", config.port);
     write(1, buff, bytes);
     free(buff);
     write(1, PRINT_CONFIG_END, strlen(PRINT_CONFIG_END));
}

int isIp(char *ip) {
    int i = 0, j = 0, k = 0;
    char *aux;

    aux = calloc(1, sizeof(char));

    //If there is any char (a/A->z/Z), we consider it to be a host name
    while (ip[i] != '\0') {
        j = 0;

        while (ip[i] != '.' && ip[i] != '\0') {
            aux[j++] = ip[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }

        if (ip[i] == '.') i++;
        aux[j] = '\0';

        for (k = 0; k < j; k++) {
            if ((aux[k] >= 'a' && aux[k] <= 'z') || (aux[k] >= 'A' && aux[k] <= 'Z')) {
                free(aux);
                return 0;
            }
        }
    }

    free(aux);
    return 1;
}
