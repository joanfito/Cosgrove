#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/*******************************************************************
*
* @File: Connection.h
* @Purpose: Manages the connection with the other processes
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

//Libraries
#include <stdio.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Configuration.h"
#include "Reader.h"

//Constants
#define FRAME_TYPE_SIZE 1
#define FRAME_LENGTH_SIZE 2
#define CONNECTION_MCGRUDER_PATTERN "Connection Lionel - %s ready.\n"
#define DISCONNECTION_MCGRUDER_PATTERN "Disconnecting from %s.\n"
#define SOCKET_CONNECTION_FAILED -1
#define MC_GRUDER_ACCEPT_FAILED -2
#define CONNECT_MCGRUDER_OK 1
#define CONNECT_MCGRUDER_KO 2
#define DISCONNECT_MCGRUDER_OK 3
#define DISCONNECT_MCGRUDER_KO 4
#define SOCKET_CONNECTION_OK 8
#define SOCKET_CONNECTION_KO 9
#define CONNECTION_FRAME_TYPE 0x01
#define DISCONNECTION_FRAME_TYPE 0x02
#define FILE_FRAME_TYPE 0x03
#define EMPTY_HEADER "[]"
#define CONNECTION_OK_HEADER "[CONOK]"
#define CONNECTION_KO_HEADER "[CONKO]"


//Type definitions
typedef struct {
     char *telescope_name;
     int fd;
} McGruder;

typedef struct {
     char *scientist_name;
     int fd;
} McTavish;

typedef struct {
     int socket_fd;
     McGruder *mcgruder;
     McTavish *mctavish;
     int num_mcgruder_processes;
     int num_mctavish_processes;
} Connection;

//Functions

/*******************************************************************
*
* @Name: createConnection
* @Purpose: Creates a connection object
* @Arguments: --
* @Return: the connection object
*
********************************************************************/
Connection createConnection();

/*******************************************************************
*
* @Name: createSocketForMcGruder
* @Purpose: Creates the socket for the connection with McGruder
* @Arguments: config (in) = Configuration object of Lionel
* @Return: the socket file descriptor, if the creation is okay,
*          otherwise, SOCKET_CONNECTION_FAIL
*
********************************************************************/
int createSocketForMcGruder(Configuration config);

/*******************************************************************
*
* @Name: acceptMcGruder
* @Purpose: Listen the socket in order to connect with McGruder
* @Arguments: socket_fd (in) = socket file descriptor
* @Return: the mcgruder file descriptor, if the connection is okay,
*          otherwise, MC_GRUDER_ACCEPT_FAILED
*
********************************************************************/
int acceptMcGruder(int socket_fd);

/*******************************************************************
*
* @Name: connectMcGruder
* @Purpose: Connects a McGruder process and gets its name
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
* @Arguments: index (in) = mcgruder process index
* @Return: CONNECT_MCGRUDER_OK, if the connection is okay,
*          otherwise, CONNECT_MCGRUDER_KO
*
********************************************************************/
int connectMcGruder(int index);

/*******************************************************************
*
* @Name: disconnectMcGruder
* @Purpose: Disconnect a McGruder process
*           Lionel sends the following frame:
*                   Type (1 byte): 0x02
*                   Header (2 bytes): [CONOK/CONKO]
*                   Length (2 bytes): 0
*                   Data (0 bytes): --
* @Arguments: index (in) = mcgruder process index
* @Return: DISCONNECT_MCGRUDER_OK, if the disconnection is okay,
*          otherwise, DISCONNECT_MCGRUDER_KO
*
********************************************************************/
int disconnectMcGruder(int index);

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

/*******************************************************************
*
* @Name: mcgruderListener
* @Purpose: Listens the socket of the mcgruder process
* @Arguments: arg (in) = index
* @Return: arg
*
********************************************************************/
void *mcgruderListener(void *arg);
#endif
