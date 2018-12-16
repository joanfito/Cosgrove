/*******************************************************************
*
* @File: Menu.h
* @Purpose: Manages the menu of the application
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

//Libraries
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "Connection.h"
#include "Reader.h"
#include "Exit.h"

//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define MENU_HEADER "\n\n#############################################\n"
#define MENU_INPUT "\nWhat data do you need?\n"
#define MENU_INPUT_ERROR "This isn't a valid option.\n"
#define MENU_OPTION_1_MSG "\t1. All received data\n"
#define MENU_OPTION_2_MSG "\t2. Last astronomical data received\n"
#define MENU_OPTION_3_MSG "\t3. Exit\n"
#define DATA_ERROR_MSG "Sorry, there was an error while accessing the data\n"
#define RECEIVED_IMAGES_PATTERN "\t-> Number of received images: %d\n"
#define RECEIVED_ASTRONOMICAL_DATA_PATTERN "\t-> Number of received astronomical data files: %d\n"
#define IMAGES_SIZE_PATTERN "\t-> Total size of received images: %.2f KB\n"
#define AVG_CONSTELLATIONS_PATTERN "\t-> Average of constellations: %.2f\n"
#define NUM_CONSTELLATIONS_PATTERN "\t-> There are %d constellations\n"
#define AVG_DENSITY_PATTERN "\t-> Average density: %.2f\n"
#define MAX_SIZE_PATTERN "\t-> Maximum size: %.2f\n"
#define MIN_SIZE_PATTERN "\t-> Minimum size: %.2f\n"


//Type definitions

//Functions

/*******************************************************************
*
* @Name: showMenu
* @Purpose: Print the menu
* @Arguments: --
* @Return: --
*
********************************************************************/
void showMenu();

/*******************************************************************
*
* @Name: endMenu
* @Purpose: Stop showing the menu
* @Arguments: option (in) = selected option
* @Return: 1 if the menu hasn't to be shown, otherwise, 0
*
********************************************************************/
int endMenu(int option);

/*******************************************************************
*
* @Name: selectOption
* @Purpose: Select one option of the menu
* @Arguments: --
* @Return: the selected option
*
********************************************************************/
int selectOption();

/*******************************************************************
*
* @Name: executeOption
* @Purpose: Execute an option of the menu
* @Arguments: option (in) = selected option
* @Return: --
*
********************************************************************/
void executeOption(int option);

/*******************************************************************
*
* @Name: executeOption1
* @Purpose: Execute the first option of the menu
* @Arguments: --
* @Return: --
*
********************************************************************/
void executeOption1();

/*******************************************************************
*
* @Name: executeOption2
* @Purpose: Execute the second option of the menu
* @Arguments: --
* @Return: --
*
********************************************************************/
void executeOption2();

#endif
