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
#include <limits.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

//Constants
#define PAQUITA_CREATED_OK 1
#define PAQUITA_CREATED_KO 2

//Type definitions
typedef struct {
    int count_images;
    int images_size_kb;
    int count_astronomical_data;
    int acum_constellations;
} ReceivedData;

typedef struct {
    int count_constellations;
    int acum_density;
    int min_size;
    int max_size;
} ReceivedAstronomicalData;

//Functions

int initPaquita();
void resetLastAtronomicalData();
void imageReceived();
void astronomicalDataReceived();
void readAstronomicalData();
void shareReceivedData();
void shareLastAtronomicalData();


#endif
