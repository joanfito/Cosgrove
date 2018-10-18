#ifndef _FILES_H_
#define _FILES_H_

/*******************************************************************
*
* @File: Files.h
* @Purpose: Search and send the files of the telescope
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
#include <sys/wait.h>
#include "Connection.h"
#include "Reader.h"

//Constants
#define FILES_NOT_FOUND_MSG "No files found.\n"
#define FILE_FOUND_PATTERN "File: %s\n"
#define SEND_FILE_PATTERN "Sending %s....\n"
#define FILE_SENT_MSG "File sent\n"
#define FILES_NAMES_PATH "./files/names"
#define SCAN_ERROR_MSG "An error occurred while scaning the files\n"

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

/*******************************************************************
*
* @Name: isImage
* @Purpose: Verify if a given file is an image
* @Arguments: filename (in) = name of the file
* @Return: 1 if is an image, otherwise 0
*
********************************************************************/
int isImage(char *filename);

/*******************************************************************
*
* @Name: isAstronomicalData
* @Purpose: Verify if a given file is astronomical data
* @Arguments: filename (in) = name of the file
* @Return: 1 if is astronomical data, otherwise 0
*
********************************************************************/
int isAstronomicalData(char *filename);


#endif
