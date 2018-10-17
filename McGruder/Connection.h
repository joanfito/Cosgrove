#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/*******************************************************************
*
* @File: Connection.h
* @Purpose: Manages the connection with Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

//Libraries
#include "Configuration.h"

//Constants
#define CONNECTION_SUCCESSFUL 0
#define CONNECTION_FAILED 1
#define DISCONNECTION_SUCCESSFUL 2
#define DISCONNECTION_FAILED 3
#define FILE_SENDING_SUCCESSFUL 4
#define FILE_SENDING_FAILED 5
#define CHECKSUM_OK 6
#define CHECKSUM_KO 7

//Type definitions

//Functions

/*******************************************************************
*
* @Name: connect
* @Purpose: Connect the process with Lionel
* @Arguments: config (in) = Configuration object of McGruder
* @Return: CONNECTION_SUCCESSFUL, if the connection is okay,
*          otherwise, CONNECTION_FAILED
*
********************************************************************/
int connect(Configuration config);

/*******************************************************************
*
* @Name: disconnect
* @Purpose: Disconnect the process with Lionel
* @Arguments: --
* @Return: DISCONNECTION_SUCCESSFUL, if the disconnection is okay,
*          otherwise, DISCONNECTION_FAILED
*
********************************************************************/
int disconnect();

/*******************************************************************
*
* @Name: sendFile
* @Purpose: Send a file to Lionel
* @Arguments: --
* @Return: FILE_SENDING_SUCCESSFUL, if the disconnection is okay,
*          otherwise, FILE_SENDING_FAILED
*
********************************************************************/
int sendFile();

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

#endif
