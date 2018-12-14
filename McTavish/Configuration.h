/*******************************************************************
*
* @File: Configuration.h
* @Purpose: Process the configuration file
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

//Libraries
#include "Reader.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define OPEN_FILE_ERROR "An error occurred while opening the configuration file\n"
#define PRINT_CONFIG_INI "\n*********** Configuration ***********\n"
#define PRINT_CONFIG_END "*************************************\n\n\n"
#define CONFIG_ERROR -1

//Type definitions
typedef struct {
     char *name;
     char *ip;
     int is_ip;
     int port;
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

/*******************************************************************
*
* @Name: isIp
* @Purpose: Check if the given ip is an ip or a host name
* @Arguments: ip (in) = ip to check
* @Return: 1 if is an ip, 0 if it is a host name
*
********************************************************************/
int isIp(char *ip);

#endif
