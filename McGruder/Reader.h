#ifndef _READER_H_
#define _READER_H_

/*******************************************************************
*
* @File: Reader.h
* @Purpose: Manages the reading from files descriptors
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 16/10/2018
*
********************************************************************/

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

#endif
