/*******************************************************************
*
* @File: Paquita.h
* @Purpose: Manage the calculations of the received data
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 12/12/2018
*
********************************************************************/

#ifndef _PAQUITA_H_
#define _PAQUITA_H_

//Libraries
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "Reader.h"
#include "Files.h"
#include "semaphore_v2.h"
//Constants
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#define FILE_NAME_LENGHT 20
#define IMAGE_TYPE 14
#define ASTRONOMICAL_DATA_TYPE 15
#define ERROR_TYPE 16
#define PAQUITA_CREATED_OK 1
#define PAQUITA_CREATED_KO 2
#define PAQUITA_CREATION_ERROR_MSG "Something failed while creating Paquita.\n"

//Type definitions
typedef struct {
    int count_images;
    float images_size_kb;
    int count_astronomical_data;
    int acum_constellations;
} ReceivedData;

typedef struct {
    int count_constellations;
    float acum_density;
    float min_size;
    float max_size;
} ReceivedAstronomicalData;

typedef struct {
    int type;
    char name[FILE_NAME_LENGHT];
} LastFile;

//Functions

/*******************************************************************
*
* @Name: initPaquita
* @Purpose: Initialize the Paquita process
* @Arguments: --
* @Return: PAQUITA_CREATED_OK if the initialization is successful,
*          otherwise, PAQUITA_CREATED_KO
*
********************************************************************/
int initPaquita();

/*******************************************************************
*
* @Name: startPaquita
* @Purpose: Starts the Paquita process
* @Arguments: --
* @Return: --
*
********************************************************************/
void startPaquita();

/*******************************************************************
*
* @Name: resetLastAtronomicalData
* @Purpose: Reset the last astronomical data object to its default
*           values
* @Arguments: --
* @Return: --
*
********************************************************************/
void resetLastAtronomicalData();

/*******************************************************************
*
* @Name: imageReceived
* @Purpose: name (in) = name of the file
* @Arguments: --
* @Return: --
*
********************************************************************/
void imageReceived(char *name);

/*******************************************************************
*
* @Name: astronomicalDataReceived
* @Purpose: name (in) = name of the file
* @Arguments: --
* @Return: --
*
********************************************************************/
void astronomicalDataReceived(char *name);

/*******************************************************************
*
* @Name: processAstronomicalData
* @Purpose: Read and process the data of the astronomical data file
* @Arguments: name (in) = name of the file
*             count_constellations (in/out) = number of constellations
*             acum_density (in/out) = accumulation of the the density
*             min_size (in/out) = minimum constellation size
*             max_size (in/out) = maximum constellation size
* @Return: --
*
********************************************************************/
void processAstronomicalData(char *name, int *count_constellations, float *acum_density, float *min_size, float *max_size);

/*******************************************************************
*
* @Name: shareReceivedData
* @Purpose: Save the received data into the shared memory region
* @Arguments: --
* @Return: --
*
********************************************************************/
void shareReceivedData();

/*******************************************************************
*
* @Name: shareLastAtronomicalData
* @Purpose: Save the last astronoical data into the shared memory
*           region
* @Arguments: --
* @Return: --
*
********************************************************************/
void shareLastAtronomicalData();


#endif
