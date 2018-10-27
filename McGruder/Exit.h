#ifndef _EXIT_H_
#define _EXIT_H_

/*******************************************************************
*
* @File: Files.h
* @Purpose: Handle the termination of McGruder
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

//Libraries
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "Connection.h"
#include "Configuration.h"

//Constants
#define DISCONNECTION_PATTERN "\nDisconnecting %s.\n"
#define DISCONNECTION_ERROR_MSG "Something failed during the disconnection. Try again.\n"

//Type definitions

//Functions

/*******************************************************************
*
* @Name: closeMcGruder
* @Purpose: Close safely McGruder
* @Arguments: --
* @Return: --
*
********************************************************************/
void closeMcGruder();

/*******************************************************************
*
* @Name: safeClose
* @Purpose: Close the resources used by McGruder
* @Arguments: --
* @Return: --
*
********************************************************************/
void safeClose();

/*******************************************************************
*
* @Name: copyConfig
* @Purpose: Make a local copy of a Configuration object
* @Arguments: original_conf (in) = Configuration object to copy
*             original_socket (in) = socket file descriptor to copy
* @Return: --
*
********************************************************************/
void copyConfigAndFd(Configuration original_conf, int original_socket);

#endif
