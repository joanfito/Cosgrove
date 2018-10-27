#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/*******************************************************************
*
* @File: Connection.h
* @Purpose: Manages the connection with Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

//Libraries
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Reader.h"
#include "Configuration.h"

//Constants
#define FRAME_TYPE_SIZE 1
#define FRAME_LENGTH_SIZE 2
#define CONNECTION_SUCCESSFUL 0
#define CONNECTION_FAILED -1
#define DISCONNECTION_SUCCESSFUL 2
#define DISCONNECTION_FAILED 3
#define FILE_SENDING_SUCCESSFUL 4
#define FILE_SENDING_FAILED 5
#define CHECKSUM_OK 6
#define CHECKSUM_KO 7
#define SOCKET_CONNECTION_OK 8
#define SOCKET_CONNECTION_KO 9
#define CONNECTION_FRAME_TYPE 0x01
#define DISCONNECTION_FRAME_TYPE 0x02

//Type definitions

//Functions

/*******************************************************************
*
* @Name: openLionel
* @Purpose: Create the socket for the connection with Lionel
* @Arguments: config (in) = Configuration object of McGruder
* @Return: the socket file descriptor, if the creation is okay,
*          otherwise, CONNECTION_FAILED
*
********************************************************************/
int openLionel(Configuration config);

/*******************************************************************
*
* @Name: connectLionel
* @Purpose: Connect with Lionel
*           The McGruder sends the following connection frame:
*                   Type (1 byte): 0x01
*                   Header (2 bytes): []
*                   Length (2 bytes): length of telescope name
*                   Data (<length> bytes): telescope name
*           Lionel answer with:
*                   Type (1 byte): 0x01
*                   Header (2 bytes): [CONOK/CONKO]
*                   Length (2 bytes): 0
*                   Data (0 bytes): --
* @Arguments: config (in) = Configuration object of McGruder
* @Return: the socket file descriptor, if the creation is okay,
*          otherwise, CONNECTION_FAILED
*
********************************************************************/
int connectLionel(Configuration config);

/*******************************************************************
*
* @Name: disconnectLionel
* @Purpose: Disconnect the process with Lionel
* @Arguments: config (in) = Configuration object of McGruder
*             socket_fd (in) = Socket file descriptor
* @Return: DISCONNECTION_SUCCESSFUL, if the disconnection is okay,
*          otherwise, DISCONNECTION_FAILED
*
********************************************************************/
int disconnectLionel(Configuration config, int socket_fd);

/*******************************************************************
*
* @Name: sendFile
* @Purpose: Send a file to Lionel
* @Arguments: socket_fd (in) = Socket file descriptor
* @Return: FILE_SENDING_SUCCESSFUL, if the disconnection is okay,
*          otherwise, FILE_SENDING_FAILED
*
********************************************************************/
int sendFile(int socket_fd);

/*******************************************************************
*
* @Name: sendChecksum
* @Purpose: Send the checksum to Lionel
* @Arguments: --
* @Return: CHECKSUM_OK, if the disconnection is okay,
*          otherwise, CHECKSUM_KO
*
********************************************************************/
int sendChecksum();

/*******************************************************************
*
* @Name: sendFrame
* @Purpose: Send a frame to Lionel
* @Arguments: socket_fd (in) = socket file descriptor
*             type (in) = frame type
*             header (in) = frame header
*             length (in) = length of the frame data
*             data (in) = frame data
* @Return: SOCKET_CONNECTION_OK, if the frame is okay,
*          otherwise, SOCKET_CONNECTION_KO
*
********************************************************************/
int sendFrame(int socket_fd, char type, char *header, short length, char *data);

/*******************************************************************
*
* @Name: readFrame
* @Purpose: Read a frame from Lionel
* @Arguments: socket_fd (in) = socket file descriptor
*             type (in/out) = frame type
*             header (in/out) = frame header
*             length (in/out) = length of the frame data
*             data (in/out) = frame data
* @Return: SOCKET_CONNECTION_OK, if the frame is okay,
*          otherwise, SOCKET_CONNECTION_KO
*
********************************************************************/
int readFrame(int socket_fd, char *type, char **header, short *length, char **data);

#endif
