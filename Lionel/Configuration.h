#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/*******************************************************************
*
* @File: Configuration.h
* @Purpose: Process the configuration file
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

//Libraries
#include "Reader.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

//Constants
#define OPEN_FILE_ERROR "An error occurred while opening the configuration file\n"
#define PRINT_CONFIG_INI "\n*********** Configuration ***********\n"
#define PRINT_CONFIG_END "*************************************\n\n\n"
#define CONFIG_ERROR -1

//Type definitions
typedef struct {
     char *ip;
     int mcgruder_port;
     int mctavish_port;
     int seconds;
} Configuration;

//Functions

/*******************************************************************
*
* @Name: readConfiguration
* @Purpose: Read the configuration file and creates a Configuration
*           object with its information.
*           If the configuration file doesn't exist, it creates a
*           Configuration object with error values.
* @Arguments: filename (in) = name of the configuration file
* @Return: A Configuration object with the information.
*
********************************************************************/
Configuration readConfiguration(char *filename);

/*******************************************************************
*
* @Name: configError
* @Purpose: Create an Configuration object with error values.
* @Arguments: --
* @Return: A Configuration object with the error values.
*
********************************************************************/
Configuration configError();

/*******************************************************************
*
* @Name: invalidConfig
* @Purpose: Check if config has error values or not
* @Arguments: config (in) = Configuration object to check
* @Return: 1 if has error values, 0 if the values are valid
*
********************************************************************/
int invalidConfig(Configuration config);

/*******************************************************************
*
* @Name: printConfig
* @Purpose: Print in the screen the configuration values
* @Arguments: config (in) = Configuration object to print
* @Return: --
*
********************************************************************/
void printConfig(Configuration config);

#endif
