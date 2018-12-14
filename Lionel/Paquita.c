/*******************************************************************
*
* @File: Paquita.c
* @Purpose: Manage the calculations of the received data
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 12/12/2018
*
********************************************************************/

#include "Paquita.h"

ReceivedData received_data;
ReceivedAstronomicalData last_astronomical_data;
int id_received_data, id_last_data;

int initPaquita() {
    //Inicialize the variables
    received_data.count_images = 0;
    received_data.images_size_kb = 0;
    received_data.count_astronomical_data = 0;
    received_data.acum_constellations = 0;

    resetLastAtronomicalData();

    //Create the semaphore for both received data and last astronomical data


    //Create the shared memory region for the received data
    id_received_data = shmget(IPC_PRIVATE, sizeof(ReceivedData), IPC_CREAT | IPC_EXCL | 0600);

    if (id_received_data == -1) {
         return PAQUITA_CREATED_KO;
    }

    shareReceivedData();

    //Create the shared memory region for the last astronomical data
    id_last_data = shmget(IPC_PRIVATE, sizeof(ReceivedAstronomicalData), IPC_CREAT | IPC_EXCL | 0600);

    if (id_last_data == -1) {
        shmctl(id_received_data, IPC_RMID, NULL);
        return PAQUITA_CREATED_KO;
    }
    shareLastAtronomicalData();

    return PAQUITA_CREATED_OK;
}

void resetLastAtronomicalData() {
    last_astronomical_data.count_constellations = 0;
    last_astronomical_data.acum_density = 0;
    last_astronomical_data.min_size = INT_MAX;
    last_astronomical_data.max_size = INT_MIN;
}

void imageReceived() {

}

void astronomicalDataReceived() {

}

void readAstronomicalData() {

}

void shareReceivedData() {
    //TODO semafors
    //Save the values
    ReceivedData *shared_received_data = shmat(id_received_data, NULL, 0);

    shared_received_data->count_images = received_data.count_images;
    shared_received_data->images_size_kb = received_data.images_size_kb;
    shared_received_data->count_astronomical_data = received_data.count_astronomical_data;
    shared_received_data->acum_constellations = received_data.acum_constellations;

    //Unlink from the shared memory reion
    shmdt(shared_received_data);
}

void shareLastAtronomicalData() {
    //TODO semafors
    //Save the values
    ReceivedAstronomicalData *shared_last_data = shmat(id_last_data, NULL, 0);

    shared_last_data->count_constellations = last_astronomical_data.count_constellations;
    shared_last_data->acum_density = last_astronomical_data.acum_density;
    shared_last_data->min_size = last_astronomical_data.min_size;
    shared_last_data->max_size = last_astronomical_data.max_size;

    //Unlink from the shared memory reion
    shmdt(shared_last_data);
}
