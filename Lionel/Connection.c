/*******************************************************************
*
* @File: Connection.c
* @Purpose: Manages the connection with the other processes
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Connection.h"

extern Connection conn;
extern Files files;

Connection createConnection() {
     Connection new;

     new.socket_fd = 0;
     new.mcgruder = (McGruder*)calloc(1, sizeof(McGruder));
     new.mctavish = (McTavish*)calloc(1, sizeof(McTavish));
     new.num_mcgruder_processes = 0;
     new.num_mctavish_processes = 0;

     return new;
}

int createSocketForMcGruder(Configuration config) {
     int socket_fd = 0;
     struct sockaddr_in lionel;

     //Get a IPv4 & TCP socket file descriptor
     socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (socket_fd < 0) {
          return SOCKET_CONNECTION_FAILED;
     }

     //Configure the server
     lionel.sin_family = AF_INET;
     lionel.sin_addr.s_addr = INADDR_ANY;
     lionel.sin_port = htons(config.mcgruder_port);

     if (bind(socket_fd, (struct sockaddr *)&lionel, sizeof(lionel)) < 0) {
          close(socket_fd);
          return SOCKET_CONNECTION_FAILED;
     }

     //Listen the port
     if (listen(socket_fd, 5) < 0) {
          close(socket_fd);
          return SOCKET_CONNECTION_FAILED;
     }

     return socket_fd;
}

int acceptMcGruder(int socket_fd) {
     struct sockaddr_in mcgruder;
     int mcgruder_fd;
     socklen_t mcgruder_len;

     //Accept the new connection
     mcgruder_len = sizeof(mcgruder);
     mcgruder_fd = accept(socket_fd, (struct sockaddr *)&mcgruder, &mcgruder_len);

     if (mcgruder_fd < 0) {
          return MC_GRUDER_ACCEPT_FAILED;
     }

     return mcgruder_fd;
}

int connectMcGruder(int index) {
     char buff[100];
     char type, *header, *data;
     short length;
     int response, bytes = 0;

     //Read the frame
     response = readFrame(conn.mcgruder[index].fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          free(header);
          free(data);
          return CONNECT_MCGRUDER_KO;
     }

     if (type == (char)CONNECTION_FRAME_TYPE) {
          conn.mcgruder[index].telescope_name = malloc(sizeof(char) * (strlen(data) + 1));
          strcpy(conn.mcgruder[index].telescope_name, data);

          //Send a connection ok frame
          response = sendFrame(conn.mcgruder[index].fd, (char)CONNECTION_FRAME_TYPE, "[CONOK]", 0, NULL);
          if (response == SOCKET_CONNECTION_KO) {
               free(header);
               free(data);
               return CONNECT_MCGRUDER_KO;
          }

          bytes = sprintf(buff, CONNECTION_MCGRUDER_PATTERN, conn.mcgruder[index].telescope_name);
          write(1, buff, bytes);
     } else {
          //Send a connection ko frame
          response = sendFrame(conn.mcgruder[index].fd, (char)CONNECTION_FRAME_TYPE, "[CONKO]", 0, NULL);
          if (response == SOCKET_CONNECTION_KO) {
               free(header);
               free(data);
               return CONNECT_MCGRUDER_KO;
          }
     }

     free(header);
     free(data);

     //Throw a new thread to listen the mcgruder process
     pthread_t thread_mcgruder;

     response = pthread_create(&thread_mcgruder, NULL, mcgruderListener, &index);
     if (response != 0) return CONNECT_MCGRUDER_KO;

     return CONNECT_MCGRUDER_OK;
}

int disconnectMcGruder(int index) {
     char *buff;
     int bytes = 0;

     //Send a connection ok frame
     sendFrame(conn.mcgruder[index].fd, (char)DISCONNECTION_FRAME_TYPE, CONNECTION_OK_HEADER, 0, NULL);

     bytes = asprintf(&buff, DISCONNECTION_MCGRUDER_PATTERN, conn.mcgruder[index].telescope_name);
     write(1, buff, bytes);
     free(buff);

     close(conn.mcgruder[index].fd);
     conn.mcgruder[index].fd = SOCKET_CONNECTION_FAILED;
     free(conn.mcgruder[index].telescope_name);

     return DISCONNECT_MCGRUDER_OK;
}

int sendFrame(int socket_fd, char type, char *header, short length, char *data) {
     int response;

     //Send the type
     response = write(socket_fd, &type, FRAME_TYPE_SIZE);
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Send the header
     response = write(socket_fd, header, strlen(header));
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Send the length
     response = write(socket_fd, &length, FRAME_LENGTH_SIZE);
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Send the data
     response = write(socket_fd, data, length);
     if (response < 0) return SOCKET_CONNECTION_KO;

     return SOCKET_CONNECTION_OK;
}

int readFrame(int socket_fd, char *type, char **header, short *length, char **data) {
     int response;

     //Read the type
     response = read(socket_fd, type, FRAME_TYPE_SIZE);
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Read the header
     *header = readHeader(socket_fd, &response);
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Read the length
     response = read(socket_fd, length, FRAME_LENGTH_SIZE);
     if (response < 0) return SOCKET_CONNECTION_KO;

     //Read the data
     *data = readData(socket_fd, *length, &response);
     if (response < 0) return SOCKET_CONNECTION_KO;

     return SOCKET_CONNECTION_OK;
}

void *mcgruderListener(void *arg) {
     char type, *header, *data, *buff;
     short length;
     int response, end = 0, file_type, receiving_file = 0, checksum_ok, bytes;

     int index = *((int*)arg);

     while (!end) {
          //Read a frame
          response = readFrame(conn.mcgruder[index].fd, &type, &header, &length, &data);

          if (response == SOCKET_CONNECTION_KO) {
               //If the socket is down, we can disconnect the mcgruder process
               disconnectMcGruder(index);
               end = 1;
          }

          switch (type) {
               case (char)DISCONNECTION_FRAME_TYPE:
                    disconnectMcGruder(index);
                    end = 1;
                    break;
               case (char)FILE_FRAME_TYPE:
                    if (strcmp(header, METADATA_HEADER) == 0) {
                        //We only process the metadata if we aren't receiving any file
                        if (receiving_file == 0) {
                            file_type = receiveMetadata(data);

                            if (file_type != ERROR_TYPE) {
                                //Lionel is ready to receive a file
                                receiving_file = 1;

                                response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, FILE_OK_HEADER, 0, NULL);
                                if (response == SOCKET_CONNECTION_KO) {
                                    receiving_file = 0;
                                    disconnectMcGruder(index);
                                }
                            } else {
                                response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, FILE_KO_HEADER, 0, NULL);
                                if (response == SOCKET_CONNECTION_KO) {
                                    disconnectMcGruder(index);
                                }
                            }
                        }
                    } else if (strcmp(header, EMPTY_HEADER) == 0) {
                        //We only process the frame if we are receiving a file
                        if (receiving_file == 1) {
                            receiveFrame(length, file_type, data);
                        }

                    } else if (strcmp(header, ENDFILE_HEADER) == 0) {
                        //We only process the checksum if we are receiving a file
                        if (receiving_file == 1) {
                            checksum_ok = receiveChecksum(index, file_type, data);
                            receiving_file = 0;

                            if (checksum_ok == CHECKSUM_KO) {
                                if (file_type == IMAGE_TYPE) {
                                    Date date = files.images[files.num_images - 1].date;
                                    Time time = files.images[files.num_images - 1].time;

                                    asprintf(&buff, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "jpg");
                                    bytes = asprintf(&buff, FILE_RECEIVED_KO_MSG, buff);

                                    removeLastImage(&files);
                                } else if (file_type == ASTRONOMICAL_DATA_TYPE) {
                                    Date date = files.astronomical_data[files.num_astronomical_data - 1].date;
                                    Time time = files.astronomical_data[files.num_astronomical_data - 1].time;

                                    asprintf(&buff, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "txt");
                                    bytes = asprintf(&buff, FILE_RECEIVED_KO_MSG, buff);

                                    removeLastAstronomicalData(&files);
                                }
                                write(1, buff, bytes);
                                free(buff);
                            }
                        }

                    }
                    break;
          }
     }

     free(header);
     free(data);

     return (void *) arg;
}

int receiveMetadata(char *data) {
     int i = 0, j, type = ERROR_TYPE, bytes;
     char *file_type, *file_size, *file_name, *buff;

     file_type = calloc(1, sizeof(char));
     file_size = calloc(1, sizeof(char));
     file_name = calloc(1, sizeof(char));

     //Get the file type
     j = 0;
     while (data[i] != '&') {
          file_type[j++] = data[i];
          file_type = realloc(file_type, sizeof(char) * j);
          i++;
     }
     file_type[j] = '\0';

     //Get the file size
     i++;
     j = 0;
     while (data[i] != '&') {
          file_size[j++] = data[i];
          file_size = realloc(file_size, sizeof(char) * j);
          i++;
     }
     file_size[j] = '\0';

     //Get the file name
     i++;
     j = 0;
     while (data[i] != '\0') {
          file_name[j++] = data[i];
          file_name = realloc(file_name, sizeof(char) * j);
          i++;
     }
     file_name[j] = '\0';

     //Check if it's an image or astronomical data
     if (isImage(file_type)) {
         type = IMAGE_TYPE;

         files.images[files.num_images].date = createDateFromName(file_name);
         files.images[files.num_images].time = createTimeFromName(file_name);
         files.images[files.num_images].size = atoi(file_size);
         files.images[files.num_images].percentage = 0;

         //Assign memory for the next image
         files.num_images++;
         files.images = realloc(files.images, sizeof(Image) * (files.num_images + 1));

     } else if (isAstronomicalData(file_type)) {
         type = ASTRONOMICAL_DATA_TYPE;

         files.astronomical_data[files.num_astronomical_data].date = createDateFromName(file_name);
         files.astronomical_data[files.num_astronomical_data].time = createTimeFromName(file_name);
         files.astronomical_data[files.num_astronomical_data].size = atoi(file_size);

         //Assign memory for the next astronomical data
         files.num_astronomical_data++;
         files.astronomical_data = realloc(files.astronomical_data, sizeof(AstronomicalData) * (files.num_astronomical_data + 1));
     }

     //Create the file with the type in lowercase
     file_name = typeToLowerCase(file_name);

     bytes = asprintf(&buff, RECEIVING_FILE_MSG, file_name);
     write(1, buff, bytes);
     free(buff);

     asprintf(&file_name, FILES_PATH, file_name);

     if (createFile(file_name) == FILE_CREATED_KO) type = ERROR_TYPE;

     free(file_type);
     free(file_size);
     free(file_name);

     return type;
}

void receiveFrame(short length, int type, char *data) {
    int file_fd;
    char *name;
    Date date;
    Time time;

    if (type == IMAGE_TYPE) {

    } else if (type == ASTRONOMICAL_DATA_TYPE) {
        date = files.astronomical_data[files.num_astronomical_data - 1].date;
        time = files.astronomical_data[files.num_astronomical_data - 1].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "txt");
        asprintf(&name, FILES_PATH, name);
        file_fd = open(name, O_WRONLY | O_APPEND);

        if (file_fd >= 0) {
            write(file_fd, data, length);
            close(file_fd);
        }

        free(name);
    }
}

int receiveChecksum(int index, int type, char *data) {
    char *checksum, *name, *path, *buff;
    int response, bytes;
    Date date;
    Time time;

    //Get the file name from the date, time and the type
    if (type == IMAGE_TYPE) {
        date = files.images[files.num_images - 1].date;
        time = files.images[files.num_images - 1].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "jpg");
    } else {
        date = files.astronomical_data[files.num_astronomical_data - 1].date;
        time = files.astronomical_data[files.num_astronomical_data - 1].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "txt");
    }

    asprintf(&path, FILES_PATH, name);
    checksum = calculateChecksum(path);

    if (strlen(checksum) < 1) {
            free(path);
            free(name);
            free(checksum);

            //If we can't calculate the checksum, we don't accept the file
            response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, CHECK_KO_HEADER, 0, NULL);
            if (response == SOCKET_CONNECTION_KO) {
                disconnectMcGruder(index);
            }
            return CHECKSUM_KO;
    }

    if (strcmp(checksum, data) == 0) {
        //Both checksums are equal
        response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, CHECK_OK_HEADER, 0, NULL);
        if (response == SOCKET_CONNECTION_KO) {
            free(path);
            free(name);
            free(checksum);
            disconnectMcGruder(index);
            return CHECKSUM_KO;
        }
    } else {
        response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, CHECK_KO_HEADER, 0, NULL);
        if (response == SOCKET_CONNECTION_KO) {
            free(path);
            free(name);
            free(checksum);
            disconnectMcGruder(index);
        }
        return CHECKSUM_KO;
    }

    bytes = asprintf(&buff, FILE_RECEIVED_OK_MSG, name);
    write(1, buff, bytes);

    free(buff);
    free(path);
    free(checksum);
    free(name);

    return CHECKSUM_OK;
}
