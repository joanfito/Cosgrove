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
     char buff[100];
     int bytes = 0;

     //Send a connection ok frame
     sendFrame(conn.mcgruder[index].fd, (char)DISCONNECTION_FRAME_TYPE, "[CONOK]", 0, NULL);

     bytes = sprintf(buff, DISCONNECTION_MCGRUDER_PATTERN, conn.mcgruder[index].telescope_name);
     write(1, buff, bytes);

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
     char type, *header, *data;
     short length;
     int response, end = 0;

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
                    break;
          }
     }

     free(header);
     free(data);

     return (void *) arg;
}
