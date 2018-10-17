#ifndef _FILES_H_
#define _FILES_H_

/*******************************************************************
*
* @File: Files.h
* @Purpose: Search and sends the files of the telescope
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

//Libraries
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "Connection.h"

//Constants
#define FILES_NOT_FOUND_MSG "No files found.\n"
#define FILE_FOUND_PATTERN "File: %s\n"
#define SEND_FILE_PATTERN "Sending %s....\n"

//Type definitions

//Functions

/*******************************************************************
*
* @Name: scanDirectory
* @Purpose: Scan the files directory to send its files
* @Arguments: config (in) = Configuration object of McGruder
* @Return: --
*
********************************************************************/
void scanDirectory(Configuration config);

#endif
