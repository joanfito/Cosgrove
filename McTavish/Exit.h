/*******************************************************************
*
* @File: Exit.h
* @Purpose: Handle the termination of McTavish
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#ifndef _EXIT_H_
#define _EXIT_H_

//Libraries
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "Connection.h"
#include "Configuration.h"

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define DISCONNECTION_PATTERN "\nDisconnecting %s.\n"
#define DISCONNECTION_ERROR_MSG "Something failed during the disconnection. Try again.\n"

//Type definitions

//Functions

/*******************************************************************
*
* @Name: closeMcTavish
* @Purpose: Close safely McTavish
* @Arguments: --
* @Return: --
*
********************************************************************/
void closeMcTavish();

/*******************************************************************
*
* @Name: safeClose
* @Purpose: Close the resources used by McTavish
* @Arguments: --
* @Return: --
*
********************************************************************/
void safeClose();

#endif
