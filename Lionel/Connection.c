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

Connection createConnection() {
     Connection conn;

     conn.socket_fd = 0;
     conn.mcgruder = (McGruder*) malloc(sizeof(McGruder));
     conn.mctavish = (McTavish*) malloc(sizeof(McTavish));
     conn.num_mcgruder_processes = 0;
     conn.num_mctavish_processes = 0;

     return conn;
}

int createSocketForMcGruder(Configuration config) {
     int socket_fd = 0;
     struct sockaddr_in lionel;

     //Get a IPv4 & TCP socket file descriptor
     socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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

int connectMcGruder(McGruder *mcgruder) {
     char buff[100];
     char type, *header, *data;
     short length;
     int response, bytes = 0;

     //Read the frame
     response = readFrame(mcgruder->fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) return CONNECT_MCGRUDER_KO;

     if (type == (char)CONNECTION_FRAME_TYPE) {
          //Send a connection ok frame
          response = sendFrame(mcgruder->fd, (char)CONNECTION_FRAME_TYPE, "[CONOK]", 0, NULL);
          if (response == SOCKET_CONNECTION_KO) return CONNECT_MCGRUDER_KO;

          mcgruder->telescope_name = data;
          bytes = sprintf(buff, CONNECTION_MCGRUDER_PATTERN, mcgruder->telescope_name);
          write(1, buff, bytes);
     } else {
          //Send a connection ko frame
          response = sendFrame(mcgruder->fd, (char)CONNECTION_FRAME_TYPE, "[CONKO]", 0, NULL);
          if (response == SOCKET_CONNECTION_KO) return CONNECT_MCGRUDER_KO;
     }

     //Throw a new thread to listen the mcgruder process
     pthread_t thread_mcgruder;
     response = pthread_create(&thread_mcgruder, NULL, mcgruderListener, mcgruder);
     if (response != 0) return CONNECT_MCGRUDER_KO;

     return CONNECT_MCGRUDER_OK;
}

int disconnectMcGruder(McGruder mcgruder) {
     char buff[100];
     int response, bytes = 0;

     //Send a connection ok frame
     response = sendFrame(mcgruder.fd, (char)DISCONNECTION_FRAME_TYPE, "[CONOK]", 0, NULL);
     if (response == SOCKET_CONNECTION_KO) return CONNECT_MCGRUDER_KO;

     bytes = sprintf(buff, DISCONNECTION_MCGRUDER_PATTERN, mcgruder.telescope_name);
     write(1, buff, bytes);

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
     McGruder *mcgruder = (McGruder*)arg;
     char type, *header, *data;
     short length;
     int response;

     //Read a frame
     response = readFrame(mcgruder->fd, &type, &header, &length, &data);
     if (response == SOCKET_CONNECTION_KO) {
          //If the socket is down, we can disconnect the mcgruder process
          disconnectMcGruder(*mcgruder);
     }

     switch (type) {
          case (char)DISCONNECTION_FRAME_TYPE:
               disconnectMcGruder(*mcgruder);
               break;
          case (char)FILE_FRAME_TYPE:
               break;
     }

     return (void *) arg;
}
