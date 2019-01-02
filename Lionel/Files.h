/*******************************************************************
*
* @File: Files.h
* @Purpose: Manage the files received
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
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
#include "Date.h"
#include "Time.h"

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define FILE_NAME_PATTERN "%.4d%.2d%.2d-%.2d%.2d%.2d.%s"
#define FILES_PATH "./files/%s"
#define KALKUN_PATH "./files/Kalkun.txt"
#define WATKIN_PATH "./files/Watkin.txt"
#define KALKUN_LINE_PATTERN "%s %s %d bytes\n"
#define FILE_CREATED_OK 1
#define FILE_CREATED_KO 2
#define KALKUN_SAVED_OK 3
#define KALKUN_SAVED_KO 4

//Type definitions
typedef struct {
     Date date;
     Time time;
     int size;
     int bytes_readed;
     int percentage;
     int entered;
} Image;

typedef struct {
     Date date;
     Time time;
     int size;
} AstronomicalData;

typedef struct {
     Image *images;
     AstronomicalData *astronomical_data;
     int num_images;
     int num_astronomical_data;
} Files;

//Functions

/*******************************************************************
*
* @Name: createFiles
* @Purpose: Creates a file object
* @Arguments: --
* @Return: the file object
*
********************************************************************/
Files createFiles();

/*******************************************************************
*
* @Name: isImage
* @Purpose: Verify if a given file is an image
* @Arguments: type (in) = type of the file
* @Return: 1 if is an image, otherwise 0
*
********************************************************************/
int isImage(char *type);

/*******************************************************************
*
* @Name: isAstronomicalData
* @Purpose: Verify if a given file is astronomical data
* @Arguments: type (in) = type of the file
* @Return: 1 if is astronomical data, otherwise 0
*
********************************************************************/
int isAstronomicalData(char *type);

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

/*******************************************************************
*
* @Name: createFile
* @Purpose: Create a file
* @Arguments: filename (in) = name of the file
* @Return: FILE_CREATED_OK if the file is created successfuly,
*          otherwise, FILE_CREATED_KO
*
********************************************************************/
int createFile(char *filename);

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
* @Name: typeToLowerCase
* @Purpose: Override the type with the same type in lower case
* @Arguments: filename (in) = name of the file
* @Return: the new name
*
********************************************************************/
char *typeToLowerCase(char *filename);

/*******************************************************************
*
* @Name: removeLastImage
* @Purpose: Remove the last image received
* @Arguments: files (in/out) = received files
* @Return: --
*
********************************************************************/
void removeLastImage(Files *files);

/*******************************************************************
*
* @Name: removeLastAstronomicalData
* @Purpose: Remove the last image received
* @Arguments: files (in/out) = received files
* @Return: --
*
********************************************************************/
void removeLastAstronomicalData(Files *files);

/*******************************************************************
*
* @Name: saveReceivedFiles
* @Purpose: Saves the received files into Kalkun.txt
* @Arguments: files (in) = received files
* @Return: KALKUN_SAVED_OK if the files have been saved successfuly,
*          otherwise, KALKUN_SAVED_KO
*
********************************************************************/
int saveReceivedFiles(Files files);

/*******************************************************************
*
* @Name: printProgressbar
* @Purpose: Prints the progessbar of the current progress
* @Arguments: percentage (in) = current percentage
*             progress_completed (in/out) = number of bars reached
*                                           (from 0 to 10)
*             name (in) = file name
*             entered (in/out) = flag to print the progress just once
* @Return: --
*
********************************************************************/
void printProgressbar(float percentage, int *progress_completed, char *name, int *entered);

/*******************************************************************
*
* @Name: readAstronomicalData
* @Purpose: Read the data of the astronomical data file and save it
*           in shared memory
* @Arguments: name (in) = name of the file
* @Return: --
*
********************************************************************/
void readAstronomicalData(char *name);

#endif
