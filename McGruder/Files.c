/*******************************************************************
*
* @File: Files.c
* @Purpose: Search and send the files of the telescope
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

#include "Files.h"

void scanDirectory(int socket_fd) {
     int endOfFile = 0, fd = 0;
     int file_found = 0, bytes;
     char buff[100];
     char *output, *file;
     int script;

     //Bash script to get all the names
     script = fork();

     switch (script) {
          case -1:
               write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               break;
          case 0:
               execl("/bin/bash","bash","./getnames.sh",NULL);
               break;
          default:
               //Wait until the script is executed
               wait(0);

               //Open and process the file
               fd = open(FILES_NAMES_PATH, O_RDONLY);
               if (fd < 0) {
                    write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               } else {

                    while (!endOfFile) {
                        output = readLine(fd, '\n', &endOfFile);

                        if (isImage(output) || isAstronomicalData(output)) {
                             file = output;
                             bytes = sprintf(buff, FILE_FOUND_PATTERN, file);
                             write(1, buff, bytes);
                             file_found = 1;
                             break;
                        }
                   }
                   close(fd);
                   free(output);

                   if (!file_found) {
                       write(1, FILES_NOT_FOUND_MSG, strlen(FILES_NOT_FOUND_MSG));
                   } else {
                        //Send the file
                        if (isImage(file)) {
                             sendImage(file, socket_fd);
                        } else if (isAstronomicalData(file)) {
                             sendAstronomicalData(file, socket_fd);
                        }
                   }
               }
               break;
     }
}

int isImage(char *filename) {
     int i = 0, j = 0;
     char type[4];

     //Read the name
     while (filename[i] != '.' && filename[i] != '\0') {
          i++;
     }

     //The name has a dot
     if (filename[i] == '.') {
          //Skip the dot
          i++;

          //Read the type
          while (filename[i] != '\0') {
               type[j] = filename[i];
               j++;
               i++;
          }
          type[j] = '\0';
          return (strcmp(type, "JPG") == 0 || strcmp(type, "jpg") == 0);
     }
     return 0;
}

int isAstronomicalData(char *filename) {
     int i = 0, j = 0;
     char type[4];

     //Read the name
     while (filename[i] != '.' && filename[i] != '\0') {
          i++;
     }

     //The name has a dot
     if (filename[i] != '\0') {
          //Skip the dot
          i++;

          //Read the type
          while (filename[i] != '\0') {
               type[j] = filename[i];
               j++;
               i++;
          }
          type[j] = '\0';

          return (strcmp(type, "TXT") == 0 || strcmp(type, "txt") == 0);
     }
     return 0;
}

void sendImage(char *filename, int socket_fd) {
     int bytes;
     char buff[100];

     bytes = sprintf(buff, SEND_FILE_PATTERN, filename);
     write(1, buff, bytes);
     sendFile(socket_fd);
     removeFile(filename);
     write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));
}

void sendAstronomicalData(char *filename, int socket_fd) {
     int bytes;
     char buff[100];

     bytes = sprintf(buff, SEND_FILE_PATTERN, filename);
     write(1, buff, bytes);
     sendFile(socket_fd);
     removeFile(filename);
     write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));
}

void removeFile(char *filename) {
     char path[100];
     int script;

     script = fork();

     switch (script) {
          case -1:
               write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               break;
          case 0:
               sprintf(path, FILES_PATH, filename);
               execl("/bin/rm","rm", "-rf", path, NULL);
               break;
          default:
               wait(0);
               break;
     }
}
