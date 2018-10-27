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
#include <string.h>
#include "Connection.h"

//Constants
#define SHUT_DOWN_MSG "\nShutting down Lionel.\n"
#define DISCONNECTION_PATTERN "\nDisconnecting %s.\n"
#define SHUT_DOWN_ERROR_MSG "Something failed during the shut down. Try again.\n"

//Type definitions

//Functions

/*******************************************************************
*
* @Name: closeMcGruder
* @Purpose: Close safely Lionel
* @Arguments: --
* @Return: --
*
********************************************************************/
void closeLionel();

/*******************************************************************
*
* @Name: safeClose
* @Purpose: Close the resources used by Lionel
* @Arguments: --
* @Return: --
*
********************************************************************/
void safeClose();

/*******************************************************************
*
* @Name: copyConnection
* @Purpose: Make a local copy of a Connection object
* @Arguments: original (in) = Connection object to copy
* @Return: --
*
********************************************************************/
void copyConnection(Connection original);

#endif
