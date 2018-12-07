/*******************************************************************
*
* @File: Files.h
* @Purpose: Search and send the files of the telescope
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

#ifndef _FILES_H_
#define _FILES_H_

//Libraries
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "Connection.h"
#include "Reader.h"

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define FILES_NOT_FOUND_MSG "No files found.\n"
#define FILE_FOUND_PATTERN "File: %s\n"
#define SEND_FILE_PATTERN "Sending %s....\n"
#define FILE_SENT_MSG "File sent\n"
#define FILE_NOT_SENT_MSG "File not sent successfully\n"
#define FILES_PATH "./files/%s"
#define FILES_NAMES_PATH "./files/names"
#define SCAN_ERROR_MSG "An error occurred while scaning the files\n"
#define FRAME_SIZE 100
#define METADATA_PATTERN "%s&%d&%s"

//Type definitions

//Functions

/*******************************************************************
*
* @Name: scanDirectory
* @Purpose: Scan the files directory to send its files
* @Arguments: socket_fd (in) = socket file descriptor
* @Return: --
*
********************************************************************/
void scanDirectory(int socket_fd);

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

/*******************************************************************
*
* @Name: sendImage
* @Purpose: Send an image to Lionel
* @Arguments: filename (in) = name of the file
* @Return: SEND_IMAGE_OK, if the image is sended okay,
*          otherwise, SEND_IMAGE_KO
*
********************************************************************/
int sendImage(char *filename, int socket_fd);

/*******************************************************************
*
* @Name: sendAstronomicalData
* @Purpose: Send an astronomical data file
* @Arguments: filename (in) = name of the file
* @Return: SEND_TXT_OK, if the text file is sended okay,
*          otherwise, SEND_TXT_KO
*
********************************************************************/
int sendAstronomicalData(char *filename, int socket_fd);

/*******************************************************************
*
* @Name: removeFile
* @Purpose: Remove a file
* @Arguments: filename (in) = name of the file
* @Return: --
*
********************************************************************/
void removeFile(char *filename);

/*******************************************************************
*
* @Name: calculateChecksum
* @Purpose: Calculate a file checksum
* @Arguments: filename (in) = name of the file
* @Return: the checksum
*
********************************************************************/
char *calculateChecksum(char *filename);

/*******************************************************************
*
* @Name: getFileSize
* @Purpose: Calculate the size of a file
* @Arguments: filename (in) = name of the file
* @Return: the size
*
********************************************************************/
int getFileSize(char *filename);


#endif
