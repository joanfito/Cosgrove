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

void scanDirectory(Configuration config) {
     int endOfFile = 0, fd = 0, i;
     int img_size = 0, data_size = 0, bytes;
     char buff[100];
     char *output, **images, **data;
     int script;

     //Bash script to get all the names
     script = fork();

     switch (script) {
          case -1:
               write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               break;
          case 0:
               execl("/bin/bash","bash","./files/getnames.sh",NULL);
               break;
          default:
               //Wait until the script is executed
               wait(0);

               //Open and process the file
               fd = open(FILES_NAMES_PATH, O_RDONLY);
               if (fd < 0) {
                    write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               } else {

                    images = malloc(sizeof(char *));
                    data = malloc(sizeof(char *));
                    while (!endOfFile) {
                        output = readLine(fd, '\n', &endOfFile);

                        if (isImage(output)) {
                             images[img_size] = output;
                             bytes = sprintf(buff, FILE_FOUND_PATTERN, images[img_size]);
                             write(1, buff, bytes);

                             images = realloc(images, sizeof(char *) * (img_size + 1));
                             img_size++;
                        }

                        if (isAstronomicalData(output)) {
                             data[data_size] = output;
                             bytes = sprintf(buff, FILE_FOUND_PATTERN, data[data_size]);
                             write(1, buff, bytes);

                             data = realloc(data, sizeof(char *) * (data_size + 1));
                             data_size++;
                        }
                   }
                   close(fd);

                   if (data_size == 0 && img_size == 0) {
                       write(1, FILES_NOT_FOUND_MSG, strlen(FILES_NOT_FOUND_MSG));
                   } else {
                        //Send the files
                        for (i = 0; i < img_size; i++) {
                             bytes = sprintf(buff, SEND_FILE_PATTERN, images[i]);
                             write(1, buff, bytes);
                             sendFile();
                             write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));
                        }

                        for (i = 0; i < data_size; i++) {
                             bytes = sprintf(buff, SEND_FILE_PATTERN, data[i]);
                             write(1, buff, bytes);
                             sendFile();
                             write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));
                        }
                   }

                   free(images);
                   free(data);
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
