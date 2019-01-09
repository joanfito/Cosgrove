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
extern int id_received_data, id_last_data, id_last_file;
extern semaphore sem_sync_paquita, sem_file, sem_received;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

Connection createConnection() {
     Connection new;

     //Initialize the values
     new.mcgruder_fd = 0;
     new.mctavish_fd = 0;
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
     int mcgruder_fd = 0;
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
     char type = 0, *header, *data;
     short length;
     int response = 0, bytes = 0;

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
          response = sendFrame(conn.mcgruder[index].fd, (char)CONNECTION_FRAME_TYPE, CONNECTION_OK_HEADER, 0, NULL);
          if (response == SOCKET_CONNECTION_KO) {
               free(header);
               free(data);
               return CONNECT_MCGRUDER_KO;
          }

          bytes = sprintf(buff, CONNECTION_MCGRUDER_PATTERN, conn.mcgruder[index].telescope_name);
          write(1, buff, bytes);
     } else {
          //Send a connection ko frame
          response = sendFrame(conn.mcgruder[index].fd, (char)CONNECTION_FRAME_TYPE, CONNECTION_KO_HEADER, 0, NULL);
          if (response == SOCKET_CONNECTION_KO) {
               free(header);
               free(data);
               return CONNECT_MCGRUDER_KO;
          }
     }

     free(header);
     free(data);

     //Throw a new thread to listen the mcgruder process
     response = pthread_create(&conn.mcgruder[index].id_thread, NULL, mcgruderListener, &index);
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

     //Close the socket and free the allocated memory
     close(conn.mcgruder[index].fd);
     conn.mcgruder[index].fd = SOCKET_CONNECTION_FAILED;
     free(conn.mcgruder[index].telescope_name);

     //Wait until the thread ends
     pthread_detach(conn.mcgruder[index].id_thread);

     return DISCONNECT_MCGRUDER_OK;
}

int createSocketForMcTavish(Configuration config) {
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
     lionel.sin_port = htons(config.mctavish_port);

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

int acceptMcTavish(int socket_fd) {
    struct sockaddr_in mctavish;
    int mctavish_fd = 0;
    socklen_t mctavish_len;

    //Accept the new connection
    mctavish_len = sizeof(mctavish);
    mctavish_fd = accept(socket_fd, (struct sockaddr *)&mctavish, &mctavish_len);

    if (mctavish_fd < 0) {
        return MC_TAVISH_ACCEPT_FAILED;
    }

    return mctavish_fd;
}

int connectMcTavish(int index) {
    char buff[100];
    char type = 0, *header, *data;
    short length;
    int response = 0, bytes = 0;

    //Read the frame
    response = readFrame(conn.mctavish[index].fd, &type, &header, &length, &data);
    if (response == SOCKET_CONNECTION_KO) {
        free(header);
        free(data);
        return CONNECT_MCTAVISH_KO;
    }

    if (type == (char)CONNECTION_FRAME_TYPE) {
        conn.mctavish[index].scientist_name = malloc(sizeof(char) * (strlen(data) + 1));
        strcpy(conn.mctavish[index].scientist_name, data);

        //Send a connection ok frame
        response = sendFrame(conn.mctavish[index].fd, (char)CONNECTION_FRAME_TYPE, CONNECTION_OK_HEADER, 0, NULL);
        if (response == SOCKET_CONNECTION_KO) {
            free(header);
            free(data);
            return CONNECT_MCTAVISH_KO;
        }

        bytes = sprintf(buff, CONNECTION_MCTAVISH_PATTERN, conn.mctavish[index].scientist_name);
        write(1, buff, bytes);
    } else {
        //Send a connection ko frame
        response = sendFrame(conn.mctavish[index].fd, (char)CONNECTION_FRAME_TYPE, CONNECTION_KO_HEADER, 0, NULL);
        if (response == SOCKET_CONNECTION_KO) {
            free(header);
            free(data);
            return CONNECT_MCTAVISH_KO;
        }
    }

    free(header);
    free(data);

    //Throw a new thread to listen the mcgruder process
    response = pthread_create(&conn.mctavish[index].id_thread, NULL, mctavishListener, &index);
    if (response != 0) return CONNECT_MCTAVISH_KO;

    return CONNECT_MCTAVISH_OK;
}

int disconnectMcTavish(int index) {
     char *buff;
     int bytes = 0;

     //Send a connection ok frame
     sendFrame(conn.mctavish[index].fd, (char)DISCONNECTION_FRAME_TYPE, CONNECTION_OK_HEADER, 0, NULL);

     bytes = asprintf(&buff, DISCONNECTION_MCGRUDER_PATTERN, conn.mctavish[index].scientist_name);
     write(1, buff, bytes);
     free(buff);

     close(conn.mctavish[index].fd);
     conn.mctavish[index].fd = SOCKET_CONNECTION_FAILED;
     free(conn.mctavish[index].scientist_name);

     //Wait until the thread ends
     pthread_detach(conn.mctavish[index].id_thread);

     return DISCONNECT_MCTAVISH_OK;
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
     char type = 0, *header, *data, *file_name, *error_msg;
     LastFile *shared_last_file;
     short length;
     int response = 0, end = 0, file_type = ERROR_TYPE, receiving_file = 0, checksum_ok, bytes;

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
                                //Save the index of the receiving file
                                if (file_type == IMAGE_TYPE) {
                                     conn.mcgruder[index].receivingFile = files.num_images - 1;
                                } else if (file_type == ASTRONOMICAL_DATA_TYPE) {
                                     conn.mcgruder[index].receivingFile = files.num_astronomical_data - 1;
                                }

                                //Once the index is saved, unlock the files variable
                                pthread_mutex_unlock(&mtx);

                                //Lionel is ready to receive a file
                                receiving_file = 1;
                                response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, SEND_OK_HEADER, 0, NULL);

                                if (response == SOCKET_CONNECTION_KO) {
                                    receiving_file = 0;
                                    disconnectMcGruder(index);
                                }
                            } else {
                                pthread_mutex_unlock(&mtx);
                                response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, SEND_KO_HEADER, 0, NULL);

                                if (response == SOCKET_CONNECTION_KO) {
                                        disconnectMcGruder(index);
                                }
                            }
                        }
                    } else if (strcmp(header, EMPTY_HEADER) == 0) {
                        //We only process the frame if we are receiving a file
                        if (receiving_file == 1) {
                            receiveFrame(index, length, file_type, data);
                        }

                    } else if (strcmp(header, ENDFILE_HEADER) == 0) {
                        //We only process the checksum if we are receiving a file
                        if (receiving_file == 1 && file_type == IMAGE_TYPE) {
                            checksum_ok = receiveChecksum(index, file_type, data);
                            receiving_file = 0;

                            Date date = files.images[conn.mcgruder[index].receivingFile].date;
                            Time time = files.images[conn.mcgruder[index].receivingFile].time;

                            asprintf(&file_name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "jpg");

                            if (checksum_ok == CHECKSUM_KO) {
                                bytes = asprintf(&error_msg, FILE_RECEIVED_KO_MSG, file_name);

                                removeLastImage(&files);

                                write(1, error_msg, bytes);
                                free(error_msg);
                            } else {
                                SEM_wait(&sem_file);

                                //Save the type and the name in the shared memory region
                                shared_last_file = shmat(id_last_file, NULL, 0);

                                shared_last_file->type = IMAGE_TYPE;
                                strcpy(shared_last_file->name, file_name);

                                //Unlink from the shared memory reion
                                shmdt(shared_last_file);

                                SEM_signal(&sem_file);

                                //Notify Paquita that a new file has arrived
                                SEM_signal(&sem_sync_paquita);
                            }

                            free(file_name);
                       } else if (receiving_file == 1 && file_type == ASTRONOMICAL_DATA_TYPE) {
                            receiving_file = 0;

                            //The file will always been received successfully
                            Date date = files.astronomical_data[conn.mcgruder[index].receivingFile].date;
                            Time time = files.astronomical_data[conn.mcgruder[index].receivingFile].time;

                            response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, FILE_OK_HEADER, 0, NULL);

                            if (response == SOCKET_CONNECTION_KO) {
                                 disconnectMcGruder(index);
                            } else {
                                 asprintf(&file_name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "txt");

                                 SEM_wait(&sem_file);

                                 //Save the type and the name in the shared memory region
                                 shared_last_file = shmat(id_last_file, NULL, 0);

                                 shared_last_file->type = ASTRONOMICAL_DATA_TYPE;
                                 strcpy(shared_last_file->name, file_name);

                                 //Unlink from the shared memory reion
                                 shmdt(shared_last_file);

                                 SEM_signal(&sem_file);

                                 //Read the data
                                 readAstronomicalData(file_name);

                                 //Notify Paquita that a new file has arrived
                                 SEM_signal(&sem_sync_paquita);

                                 bytes = asprintf(&error_msg, FILE_RECEIVED_OK_MSG, file_name);
                                 write(1, error_msg, bytes);

                                 free(error_msg);
                                 free(file_name);
                            }
                       }
                    }
                    break;
          }
          free(header);
          free(data);
     }

     return (void *) arg;
}

void *mctavishListener(void *arg) {
    char type = 0, *header, *data;
    short length;
    int response = 0, end = 0;
    ReceivedData *shared_received_data;
    ReceivedAstronomicalData *shared_last_data;
    char *send_data;
    float avg_const = 0, avg_density = 0;

    int index = *((int*)arg);

    while (!end) {

        //Read a frame
        response = readFrame(conn.mctavish[index].fd, &type, &header, &length, &data);

        if (response == SOCKET_CONNECTION_KO) {
            //If the socket is down, we can disconnect the mcgruder process
            disconnectMcTavish(index);
            end = 1;
        }

        switch (type) {
            case (char)DISCONNECTION_FRAME_TYPE:
                disconnectMcTavish(index);
                end = 1;
                break;
            case (char)RECEIVED_DATA_FRAME_TYPE:
                SEM_wait(&sem_received);

                //Get the data from Paquita
                shared_received_data = shmat(id_received_data, NULL, 0);
                avg_const = 0;

                if (shared_received_data->count_astronomical_data > 0) {
                    avg_const = (float)shared_received_data->acum_constellations / (float)shared_received_data->count_astronomical_data;
                }

                asprintf(&send_data, RECEIVED_DATA_PATTERN, shared_received_data->count_images, shared_received_data->images_size_kb, shared_received_data->count_astronomical_data, avg_const);

                //Unlink from the shared memory reion
                shmdt(shared_received_data);

                SEM_signal(&sem_received);

                response = sendFrame(conn.mctavish[index].fd, (char)RECEIVED_DATA_FRAME_TYPE, EMPTY_HEADER, (short)strlen(send_data), send_data);
                free(send_data);

                if (response == SOCKET_CONNECTION_KO) {
                    disconnectMcTavish(index);
                }
                break;
            case (char)LAST_DATA_FRAME_TYPE:
                SEM_wait(&sem_received);

                //Get the data from Paquita
                shared_last_data = shmat(id_last_data, NULL, 0);
                avg_density = 0;

                if (shared_last_data->count_constellations > 0) {
                    avg_density = (float)shared_last_data->acum_density/(float)shared_last_data->count_constellations;
                }

                //If the minimum size equals to float maximum value, Lionel hasn't received a astronoical data file yet
                if (shared_last_data->min_size == FLT_MAX) {
                    asprintf(&send_data, LAST_DATA_PATTERN, shared_last_data->count_constellations, avg_density, shared_last_data->max_size, 0.0);
                } else {
                    asprintf(&send_data, LAST_DATA_PATTERN, shared_last_data->count_constellations, avg_density, shared_last_data->max_size, shared_last_data->min_size);
                }

                //Unlink from the shared memory reion
                shmdt(shared_last_data);

                SEM_signal(&sem_received);

                response = sendFrame(conn.mctavish[index].fd, (char)LAST_DATA_FRAME_TYPE, EMPTY_HEADER, strlen(send_data), send_data);
                free(send_data);

                if (response == SOCKET_CONNECTION_KO) {
                    disconnectMcTavish(index);
                }
                break;
        }

        free(header);
        free(data);
    }

    return (void *) arg;
}

int receiveMetadata(char *data) {
     int i = 0, j, type = ERROR_TYPE, bytes;
     char *file_type, *file_size, *file_name, *buff, *full_name;

     file_type = calloc(1, sizeof(char));
     file_size = calloc(1, sizeof(char));
     file_name = calloc(1, sizeof(char));

     //Get the file type
     j = 0;
     while (data[i] != '&') {
          file_type[j++] = data[i];
          file_type = realloc(file_type, sizeof(char) * (j + 1));
          i++;
     }
     file_type[j] = '\0';

     //Get the file size
     i++;
     j = 0;
     while (data[i] != '&') {
          file_size[j++] = data[i];
          file_size = realloc(file_size, sizeof(char) * (j + 1));
          i++;
     }
     file_size[j] = '\0';

     //Get the file name
     i++;
     j = 0;
     while (data[i] != '\0') {
          file_name[j++] = data[i];
          file_name = realloc(file_name, sizeof(char) * (j + 1));
          i++;
     }
     file_name[j] = '\0';

     //Guarantee mutual exclusion to files variable
     pthread_mutex_lock(&mtx);

     //Check if it's an image or astronomical data
     if (isImage(file_type)) {
         type = IMAGE_TYPE;

         files.images[files.num_images].date = createDateFromName(file_name);
         files.images[files.num_images].time = createTimeFromName(file_name);
         files.images[files.num_images].size = atoi(file_size);
         files.images[files.num_images].bytes_readed = 0;
         files.images[files.num_images].percentage = 0;
         files.images[files.num_images].entered = 0;

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

     asprintf(&full_name, FILES_PATH, file_name);

     if (createFile(full_name) == FILE_CREATED_KO) type = ERROR_TYPE;
     free(file_type);
     free(file_size);
     free(file_name);
     free(full_name);

     return type;
}

void receiveFrame(int index, short length, int type, char *data) {
    int file_fd, response;
    char *name, *full_name;
    Date date;
    Time time;

    if (type == IMAGE_TYPE) {
        //Write the received data into the file
        date = files.images[conn.mcgruder[index].receivingFile].date;
        time = files.images[conn.mcgruder[index].receivingFile].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "jpg");
        asprintf(&full_name, FILES_PATH, name);

        //Open the file in order to write and append the new data into the existing one
        file_fd = open(full_name, O_WRONLY | O_APPEND);

        if (file_fd >= 0) {
            //Write the received data
            write(file_fd, data, length);
            files.images[conn.mcgruder[index].receivingFile].bytes_readed += length;
            printProgressbar(((float)files.images[conn.mcgruder[index].receivingFile].bytes_readed/(float)files.images[conn.mcgruder[index].receivingFile].size) * 100, &files.images[conn.mcgruder[index].receivingFile].percentage, name, &files.images[conn.mcgruder[index].receivingFile].entered);
            close(file_fd);
        }

        //Notify McGruder that Lionel is ready to receive another frame
        response = sendFrame(conn.mcgruder[index].fd, (char)FILE_FRAME_TYPE, SEND_OK_HEADER, 0, NULL);
        if (response == SOCKET_CONNECTION_KO) {
            disconnectMcGruder(index);
        }

        free(name);
        free(full_name);

    } else if (type == ASTRONOMICAL_DATA_TYPE) {
        //Write the received data into the file
        date = files.astronomical_data[conn.mcgruder[index].receivingFile].date;
        time = files.astronomical_data[conn.mcgruder[index].receivingFile].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "txt");
        asprintf(&full_name, FILES_PATH, name);

        //Open the file in order to write and append the new data into the existing one
        file_fd = open(full_name, O_WRONLY | O_APPEND);

        if (file_fd >= 0) {
            //Write the received data
            write(file_fd, data, length);
            close(file_fd);
        }

        free(name);
        free(full_name);
    }
}

int receiveChecksum(int index, int type, char *data) {
    char *checksum, *name, *path, *buff;
    int response, bytes;
    Date date;
    Time time;

    //Get the file name from the date, time and the type
    if (type == IMAGE_TYPE) {
        date = files.images[conn.mcgruder[index].receivingFile].date;
        time = files.images[conn.mcgruder[index].receivingFile].time;

        asprintf(&name, FILE_NAME_PATTERN, date.year, date.month, date.day, time.hour, time.minute, time.second, "jpg");
    } else {
        date = files.astronomical_data[conn.mcgruder[index].receivingFile].date;
        time = files.astronomical_data[conn.mcgruder[index].receivingFile].time;

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

void *mcgruderServer(void *arg) {
    int aux_fd = MC_GRUDER_ACCEPT_FAILED;
    while (1) {
        write(1, WAITING_MCGRUDER_MSG, strlen(WAITING_MCGRUDER_MSG));

        //Connect with the McGruder clients
        aux_fd = acceptMcGruder(conn.mcgruder_fd);

        if (aux_fd != MC_GRUDER_ACCEPT_FAILED) {
            conn.mcgruder[conn.num_mcgruder_processes].fd = aux_fd;
            conn.num_mcgruder_processes++;
            conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
        } else {
            write(1, CONNECTION_MCGRUDER_ERROR_MSG, strlen(CONNECTION_MCGRUDER_ERROR_MSG));
        }

        if (connectMcGruder(conn.num_mcgruder_processes - 1) == CONNECT_MCGRUDER_KO) {
            //If something goes wrong, we close the socket and remove the process
            close(conn.mcgruder[conn.num_mcgruder_processes].fd);
            conn.num_mcgruder_processes--;
            conn.mcgruder = (McGruder*) realloc(conn.mcgruder, sizeof(McGruder) * (conn.num_mcgruder_processes + 1));
        }
    }

    return (void *) arg;
}

void *mctavishServer(void *arg) {
    int aux_fd = MC_TAVISH_ACCEPT_FAILED;

    while (1) {
        write(1, WAITING_MCTAVISH_MSG, strlen(WAITING_MCTAVISH_MSG));

        //Connect with the McTavish clients
        aux_fd = acceptMcTavish(conn.mctavish_fd);

        if (aux_fd != MC_TAVISH_ACCEPT_FAILED) {
            conn.mctavish[conn.num_mctavish_processes].fd = aux_fd;
            conn.num_mctavish_processes++;
            conn.mctavish = (McTavish*) realloc(conn.mctavish, sizeof(McTavish) * (conn.num_mctavish_processes + 1));
        } else {
            write(1, CONNECTION_MCTAVISH_ERROR_MSG, strlen(CONNECTION_MCTAVISH_ERROR_MSG));
        }

        if (connectMcTavish(conn.num_mctavish_processes - 1) == CONNECT_MCTAVISH_KO) {
            //If something goes wrong, we close the socket and remove the process
            close(conn.mctavish[conn.num_mctavish_processes].fd);
            conn.num_mctavish_processes--;
            conn.mctavish = (McTavish*) realloc(conn.mctavish, sizeof(McTavish) * (conn.num_mctavish_processes + 1));
        }
    }

    return (void *) arg;
}
