/*******************************************************************
*
* @File: Time.h
* @Purpose: Manage the time
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
*
********************************************************************/

#ifndef _TIME_H_
#define _TIME_H_

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define TIME_PATTERN "%.2d:%.2d"
#define TIME_ERROR_VALUE -1

//Type definitions
typedef struct {
  short hour;
  short minute;
  short second;
} Time;


//Functions

/*******************************************************************
*
* @Name: getFormattedTime
* @Purpose: Create the time with the pattern hh:mm
* @Arguments: time (in) = time information
* @Return: the formatted time
*
********************************************************************/
char *getFormattedTime(Time time);

/*******************************************************************
*
* @Name: createTimeFromName
* @Purpose: Create a time object from a mcgruder file name
            the file name follows the pattern: yyyymmdd-hhmmss
* @Arguments: name (in) = file name
* @Return: the time
*
********************************************************************/
Time createTimeFromName(char *name);

#endif
