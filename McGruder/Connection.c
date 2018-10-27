/*******************************************************************
*
* @File: Connection.c
* @Purpose: Manages the connection with Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Connection.h"

int openLionel(Configuration config) {
     struct sockaddr_in servidor;
     int socket_fd;
     socket_fd = socket(AF_INET, SOCK_STREAM, 0);

     if (socket_fd < 0) {
         return CONNECTION_FAILED;
     }

     memset(&servidor, 0, sizeof(servidor));
     servidor.sin_family = AF_INET;
     servidor.sin_port = htons(config.port);

     //Check if we have an IP or a host name
     if (config.is_ip == 1) {
          servidor.sin_addr.s_addr = inet_addr(config.ip);
     } else {
          struct hostent *host = gethostbyname(config.ip);
          servidor.sin_addr.s_addr = inet_addr(host->h_name);
     }

     if (connect(socket_fd, (void *)&servidor, sizeof(servidor)) < 0) {
          close(socket_fd);
          return CONNECTION_FAILED;
     }

     return socket_fd;
}

int connectLionel(Configuration config) {
     int socket_fd, frame_ok;
     char type, *header, *data;
     short length;
     socket_fd = openLionel(config);
     if (socket_fd == CONNECTION_FAILED) {
          return CONNECTION_FAILED;
     }
     
     //Send the connection frame
     frame_ok = sendFrame(socket_fd, (char)CONNECTION_FRAME_TYPE, "[]", (short)strlen(config.name), config.name);
     if (frame_ok == SOCKET_CONNECTION_KO) {
          return CONNECTION_FAILED;
     }

     //Read the Lionel answer
     frame_ok = readFrame(socket_fd, &type, &header, &length, &data);
     if (frame_ok == SOCKET_CONNECTION_KO) {
          return CONNECTION_FAILED;
     }

     return strcmp(header, "[CONOK]") == 0 ? socket_fd : CONNECTION_FAILED;
}

int disconnectLionel(Configuration config, int socket_fd) {
     int frame_ok;
     char type, *header, *data;
     short length;

     if (socket_fd != CONNECTION_FAILED) {
          //Send the connection frame
          frame_ok = sendFrame(socket_fd, (char)DISCONNECTION_FRAME_TYPE, "[]", (short)strlen(config.name), config.name);
          if (frame_ok == SOCKET_CONNECTION_KO) {
               return DISCONNECTION_FAILED;
          }

          //Read the Lionel answer
          frame_ok = readFrame(socket_fd, &type, &header, &length, &data);
          if (frame_ok == SOCKET_CONNECTION_KO) {
               return DISCONNECTION_FAILED;
          }
     }

     return strcmp(header, "[CONOK]") == 0 ? DISCONNECTION_SUCCESSFUL : DISCONNECTION_FAILED;
}

int sendFile() {
     //return FILE_SENDING_FAILED;
     return FILE_SENDING_SUCCESSFUL;
}

int sendChecksum() {
     //return CHECKSUM_KO;
     return CHECKSUM_OK;
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
