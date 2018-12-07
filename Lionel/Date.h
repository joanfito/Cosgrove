/*******************************************************************
*
* @File: Date.h
* @Purpose: Manage dates
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
*
********************************************************************/

#ifndef _DATE_H_
#define _DATE_H_

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define DATE_PATTERN "%.4d-%.2d-%.2d"
#define DATE_ERROR_VALUE -1

//Type definitions
typedef struct {
  short day;
  short month;
  short year;
} Date;


//Functions

/*******************************************************************
*
* @Name: getFormattedDate
* @Purpose: Create the date with the pattern yyyy-mm-dd
* @Arguments: date (in) = date information
* @Return: the formatted date
*
********************************************************************/
char *getFormattedDate(Date date);

/*******************************************************************
*
* @Name: createDateFromName
* @Purpose: Create a date object from a mcgruder file name
            the file name follows the pattern: yyyymmdd-hhmmss
* @Arguments: name (in) = file name
* @Return: the date
*
********************************************************************/
Date createDateFromName(char *name);

#endif
