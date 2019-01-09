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
     char *buff, *output, *file;
     int script;

     //Bash script to get all the names
     script = fork();

     switch (script) {
          case -1:
               write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               break;
          case 0:
               //Execute the getnames script
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

                        //If it's one of the valid file types we attemp to send it
                        if (isImage(output) || isAstronomicalData(output)) {
                             file = calloc(1, sizeof(char) * (strlen(output) + 1));
                             strcpy(file, output);

                             //Show the founded file message
                             bytes = asprintf(&buff, FILE_FOUND_PATTERN, file);
                             write(1, buff, bytes);
                             free(buff);
                             file_found = 1;
                             free(output);
                             break;
                        }
                        free(output);
                   }
                   close(fd);

                   if (!file_found) {
                       write(1, FILES_NOT_FOUND_MSG, strlen(FILES_NOT_FOUND_MSG));
                   } else {
                        //Send the file
                        if (isImage(file)) {
                             sendImage(file, socket_fd);
                        } else if (isAstronomicalData(file)) {
                             sendAstronomicalData(file, socket_fd);
                        }
                        free(file);
                   }
               }
               break;
     }
}

int isImage(char *filename) {
     int i = 0, j = 0;
     char type[4] = { 0 };

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
     }

     return (strcmp(type, "jpg") == 0 || strcmp(type, "JPG") == 0);
}

int isAstronomicalData(char *filename) {
     int i = 0, j = 0;
     char type[4] = { 0 };

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
     }
     return (strcmp(type, "txt") == 0 || strcmp(type, "TXT") == 0);
}

int sendImage(char *filename, int socket_fd) {
     int bytes, frame_ok, size, fd, response;
     char *buff, *metadata, *fullname, *frame, *checksum;
     char *header, *data, type;
     short length;

     bytes = asprintf(&fullname, FILES_PATH, filename);

     //Open the image in read only mode
     fd = open(fullname, O_RDONLY);

     if (fd < 0) {
          free(fullname);
          return SEND_IMAGE_KO;
     }

     bytes = asprintf(&buff, SEND_FILE_PATTERN, filename);
     write(1, buff, bytes);
     free(buff);

     //Calculate the image checksum
     checksum = calculateChecksum(fullname);
     if (strlen(checksum) < 1) {
          free(fullname);
          free(checksum);
          close(fd);
          return SEND_IMAGE_KO;
     }

     //Get the image length
     size = getFileSize(fullname);

     //Create the metadata
     asprintf(&metadata, METADATA_PATTERN, "jpg", size, filename);

     //Send the metadata
     frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, METADATA_HEADER, (short)strlen(metadata), metadata);

     if (frame_ok == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(checksum);
          free(metadata);
          close(fd);
          safeClose();
     }

     free(metadata);

     //Read if lionel is gonna listen us
     response = readFrame(socket_fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(header);
          free(data);
          close(fd);
          safeClose();
     }

     if (strcmp(header, SEND_OK_HEADER) != 0) {
         free(fullname);
         free(header);
         free(data);
         close(fd);
         return SEND_TXT_KO;
     }

     free(header);
     free(data);

     frame = calloc(1, sizeof(char) * FRAME_SIZE);

     //Decompose the image into smaller frames and sent it
     int i, num_trames, bytes_readed = 0, current_frame_size, progress_completed = 0;
     num_trames = (size / FRAME_SIZE) + (size % FRAME_SIZE == 0 ? 0 : 1);

     write(1, "\n", 1);

     for (i = 0; i < num_trames; i++) {
          current_frame_size = FRAME_SIZE;

          //Check if there are FRAME_SIZE bytes to read
          if (size - bytes_readed < FRAME_SIZE) {
               current_frame_size = size - bytes_readed;
          }

          //Read FRAME_SIZE bytes
          read(fd, frame, current_frame_size);

          //Send the frame
          frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, EMPTY_HEADER, (short)current_frame_size, frame);
          if (frame_ok == SOCKET_CONNECTION_KO) {
               free(frame);
               close(fd);
               safeClose();
          }

          //Wait until lionel is ready the receive another frame
          frame_ok = readFrame(socket_fd, &type, &header, &length, &data);
          if (frame_ok == SOCKET_CONNECTION_KO) {
               free(header);
               free(data);
               free(frame);
               close(fd);
               safeClose();
          }

          if (strcmp(header, SEND_OK_HEADER) != 0) {
              free(header);
              free(data);
              break;
          }

          free(header);
          free(data);

          bytes_readed += current_frame_size;
          printProgressbar(((float)bytes_readed/(float)size) * 100, &progress_completed);
     }
     write(1, "\n\n", 2);
     free(frame);
     close(fd);

     //Send the checksum
     frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, ENDFILE_HEADER, (short)strlen(checksum), checksum);
     if (frame_ok == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(checksum);
          safeClose();
     }
     free(checksum);

     //Lionel response
     response = readFrame(socket_fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(header);
          free(data);
          safeClose();
     }

     if (strcmp(header, CHECK_OK_HEADER) != 0) {
         write(1, FILE_NOT_SENT_MSG, strlen(FILE_NOT_SENT_MSG));

         free(fullname);
         free(header);
         free(data);

         return SEND_IMAGE_KO;
     }

     //Once the image is sended, we remove it from McGruder
     removeFile(fullname);
     write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));

     free(fullname);
     free(header);
     free(data);

     return SEND_IMAGE_OK;
}

int sendAstronomicalData(char *filename, int socket_fd) {
     int bytes, size, response, frame_ok, fd;
     char *buff, *fullname, *metadata, *frame;
     char *header, *data, type;
     short length;

     bytes = asprintf(&fullname, FILES_PATH, filename);

     //Open the astronomical data file in read only mode
     fd = open(fullname, O_RDONLY);

     if (fd < 0) {
          free(fullname);
          return SEND_TXT_KO;
     }

     bytes = asprintf(&buff, SEND_FILE_PATTERN, filename);
     write(1, buff, bytes);
     free(buff);

     //Get the image length
     size = getFileSize(fullname);

     //Create the metadata
     asprintf(&metadata, METADATA_PATTERN, "txt", size, filename);

     //Send the metadata
     frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, METADATA_HEADER, (short)strlen(metadata), metadata);

     if (frame_ok == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(metadata);
          close(fd);
          safeClose();
     }

     free(metadata);

     //Read if lionel is gonna listen us
     response = readFrame(socket_fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(header);
          free(data);
          close(fd);
          safeClose();
     }

     if (strcmp(header, SEND_OK_HEADER) != 0) {
         free(fullname);
         free(header);
         free(data);
         close(fd);
         return SEND_TXT_KO;
     }

     free(header);
     free(data);

     frame = calloc(1, sizeof(char) * (size + 1));
     read(fd, frame, size);
     frame[size] = '\0';

     frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, EMPTY_HEADER, (short)strlen(frame), frame);
     if (frame_ok == SOCKET_CONNECTION_KO) {
          free(fullname);
          close(fd);
          safeClose();
     }

     free(frame);
     close(fd);

     //Send the enfile frame
     frame_ok = sendFrame(socket_fd, (char)FILE_FRAME_TYPE, ENDFILE_HEADER, 0, NULL);
     if (frame_ok == SOCKET_CONNECTION_KO) {
          free(fullname);
          safeClose();
     }

     //Lionel response
     response = readFrame(socket_fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          free(fullname);
          free(header);
          free(data);
          safeClose();
     }

     if (strcmp(header, FILE_OK_HEADER) != 0) {
         write(1, FILE_NOT_SENT_MSG, strlen(FILE_NOT_SENT_MSG));

         free(fullname);
         free(header);
         free(data);

         return SEND_TXT_KO;
     }

     //Once the file is sended, we remove it from McGruder
     removeFile(fullname);
     write(1, FILE_SENT_MSG, strlen(FILE_SENT_MSG));

     free(fullname);
     free(header);
     free(data);

     return SEND_TXT_OK;
}

void removeFile(char *filename) {
     int script;

     script = fork();

     switch (script) {
          case -1:
               write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
               break;
          case 0:
               //Remove the image from the disk
               execl("/bin/rm","rm", "-rf", filename, NULL);
               break;
          default:
               wait(0);
               break;
     }
}

char *calculateChecksum(char *filename) {
     int md5sum, pipe_fd[2], endOfFile = 0;
     char *checksum, *output;

     checksum = calloc(1, sizeof(char));

     if(pipe(pipe_fd) != -1) {
          md5sum = fork();

          switch (md5sum) {
               case -1:
                    write(1, SCAN_ERROR_MSG, strlen(SCAN_ERROR_MSG));
                    break;
               case 0:
                    //Close the reading and the stdout pipe
                    close(STDOUT_FILENO);
                    close(pipe_fd[0]);

                    //Redirect the stdout pipe to the writing pipe
                    dup(pipe_fd[1]);

                    //Calculate the md5sum
                    execl("/usr/bin/md5sum","md5sum", filename, NULL);

                    close(pipe_fd[1]);
                    break;
               default:
                    //Close the writing pipe
                    close(pipe_fd[1]);
                    output = readLine(pipe_fd[0], ' ', &endOfFile);

                    checksum = realloc(checksum, sizeof(char) * (strlen(output) + 1));
                    strcpy(checksum, output);
                    free(output);

                    //Wait until the checksum is calculated
                    wait(0);
                    close(pipe_fd[0]);
                    break;
          }
     }

     return checksum;
}

int getFileSize(char *filename) {
     int fd, compt = 0;
     char aux;

     //Open the file in read only mode
     fd = open(filename, O_RDONLY);

     if (fd < 0) {
         close(fd);
         return -1;
     } else {
         //Read the file one by one character (1 byte each)
         while (read(fd, &aux, 1) > 0) compt++;
     }

     close(fd);
     return compt;
}

void printProgressbar(float percentage, int *progress_completed) {
     char *bar;
     unsigned char current_progress[11];
     int bytes, i;

     //If its a multiple of 10, we increase the progressbar
     if ((int)percentage % 10 == 0) {
          *progress_completed = (int)percentage / 10;
     }

     //Print as many '#' as progress (from 0 to 10) has been completed
     for (i = 0; i < *progress_completed; i++) {
          current_progress[i] = '#';
     }

     //Print as many ' ' as progress left to complete
     for (i = *progress_completed; i < 10; i++) {
          current_progress[i] = ' ';
     }
     current_progress[i] = '\0';

     //Print the progressbar
     bytes = asprintf(&bar, PROGRESSBAR_PATTERN, current_progress, percentage);
     write(1, bar, bytes);
     free(bar);
}
