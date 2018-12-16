/*******************************************************************
*
* @File: Exit.h
* @Purpose: Handle the termination of Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#ifndef _EXIT_H_
#define _EXIT_H_

//Libraries
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "Connection.h"

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

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

#endif
