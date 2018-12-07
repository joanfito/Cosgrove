/*******************************************************************
*
* @File: Reader.h
* @Purpose: Manages the reading from files descriptors
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#ifndef _READER_H_
#define _READER_H_

//Libraries
#include <stdlib.h>
#include <unistd.h>

//Constants

//Type definitions


//Functions

/*******************************************************************
*
* @Name: readLine
* @Purpose: Read a sequence of characters until finding the delimiter
* @Arguments: fd (in) = file descriptor from where it is read
*             delimiter (in) = the delimiter
*             endOfFile (in/out) = flag to indicate if the EOF
*                                  is reached
* @Return: The sequence of characters readed
*
********************************************************************/
char *readLine(int fd, char delimiter, int* endOfFile);

/*******************************************************************
*
* @Name: readHeader
* @Purpose: Read a frame header
* @Arguments: fd (in) = file descriptor from where it is read
*             endOfFile (in/out) = flag to indicate if the EOF
*                                  is reached
* @Return: The header readed
*
********************************************************************/
char *readHeader(int fd, int *endOfFile);

/*******************************************************************
*
* @Name: readData
* @Purpose: Read a frame data
* @Arguments: fd (in) = file descriptor from where it is read
*             length (in) = length of the data
*             endOfFile (in/out) = flag to indicate if the EOF
*                                  is reached
* @Return: The data readed
*
********************************************************************/
char *readData(int fd, int length, int *endOfFile);

#endif
