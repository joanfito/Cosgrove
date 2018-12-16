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
int id_received_data, id_last_data, id_last_file;
semaphore sem_sync_paquita, sem_file, sem_received;

int initPaquita() {
    //Initialize the variables
    received_data.count_images = 0;
    received_data.images_size_kb = 0;
    received_data.count_astronomical_data = 0;
    received_data.acum_constellations = 0;

    resetLastAtronomicalData();

    //Create the semaphore for both received data and last astronomical data
    SEM_constructor(&sem_received);
    SEM_init(&sem_received, 1);

    //Create the semaphore for the last file
    SEM_constructor(&sem_file);
    SEM_init(&sem_file, 1);

    //Create the synchronization semaphore
    SEM_constructor(&sem_sync_paquita);
    SEM_init(&sem_sync_paquita, 0);


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

    //Create the shared memory region for the name of the file
    id_last_file = shmget(IPC_PRIVATE, sizeof(LastFile), IPC_CREAT | IPC_EXCL | 0600);

    if (id_last_file == -1) {
        shmctl(id_received_data, IPC_RMID, NULL);
        shmctl(id_last_data, IPC_RMID, NULL);
        return PAQUITA_CREATED_KO;
    }

    return PAQUITA_CREATED_OK;
}

void startPaquita() {
    LastFile *shared_last_file;

    while (1) {
        //Wait until Lionel has received a new file
        SEM_wait(&sem_sync_paquita);

        //Mutual exclusion for the file
        SEM_wait(&sem_file);

        //Get the file name
        shared_last_file = shmat(id_last_file, NULL, 0);

        if (shared_last_file->type == IMAGE_TYPE) {
            imageReceived(shared_last_file->name);
        } else if (shared_last_file->type == ASTRONOMICAL_DATA_TYPE) {
            resetLastAtronomicalData();
            astronomicalDataReceived(shared_last_file->name);
        }

        shareReceivedData();
        shareLastAtronomicalData();

        //Unlink from the shared memory reion
        shmdt(shared_last_file);

        SEM_signal(&sem_file);
    }
}

void resetLastAtronomicalData() {
    last_astronomical_data.count_constellations = 0;
    last_astronomical_data.acum_density = 0;
    last_astronomical_data.min_size = FLT_MAX;
    last_astronomical_data.max_size = FLT_MIN;
}

void imageReceived(char *name) {
    char *full_name;
    asprintf(&full_name, FILES_PATH, name);

    received_data.count_images++;
    received_data.images_size_kb += (float)getFileSize(full_name) / 1000.0;

    free(full_name);
}

void astronomicalDataReceived(char *name) {
    int count_constellations;
    float acum_density, min_size, max_size;
    count_constellations = acum_density = min_size = max_size = 0;

    received_data.count_astronomical_data++;

    min_size = last_astronomical_data.min_size;
    max_size = last_astronomical_data.max_size;
    readAstronomicalData(name, &count_constellations, &acum_density, &min_size, &max_size);

    //Update the values
    last_astronomical_data.count_constellations = count_constellations;
    last_astronomical_data.acum_density = acum_density;
    last_astronomical_data.min_size = min_size;
    last_astronomical_data.max_size = max_size;

    received_data.acum_constellations += last_astronomical_data.count_constellations;
}

void readAstronomicalData(char *name, int *count_constellations, float *acum_density, float *min_size, float *max_size) {
    int fd, endOfFile = 0, i = 0, j = 0;
    char *full_name, *line, *density, *size;
    asprintf(&full_name, FILES_PATH, name);

    fd = open(full_name, O_RDONLY);
    density = calloc(1, sizeof(char));
    size = calloc(1, sizeof(char));

    if (fd > 0) {
        //Parse the astronomical data file
        while (endOfFile == 0) {
            i = j = 0;
            line = readLine(fd, '\n', &endOfFile);

            if (endOfFile == 1) {
                free(line);
                break;
            }

            //Parse the line (<constellation_code> <density> <size>)
            while (line[i] != ' ') {
                i++;
            }
            i++;

            while (line[i] != ' ') {
                density[j++] = line[i++];
                density = realloc(density, sizeof(char) * (j + 1));
            }
            density[j] = '\0';
            i++;

            j = 0;
            while (line[i] != '\0') {
                size[j++] = line[i++];
                size = realloc(size, sizeof(char) * (j + 1));
            }
            size[j] = '\0';
            i++;

            free(line);

            *count_constellations = *count_constellations + 1;
            *acum_density = *acum_density + atof(density);

            //Update the minimum or maximum size, if required
            if (atof(size) > *max_size) {
                *max_size = atof(size);
            }

            if (atof(size) < *min_size) {
                *min_size = atof(size);
            }
        }

        close(fd);
    }

    free(full_name);
    free(density);
    free(size);
}

void shareReceivedData() {
    SEM_wait(&sem_received);

    //Save the values
    ReceivedData *shared_received_data = shmat(id_received_data, NULL, 0);

    shared_received_data->count_images = received_data.count_images;
    shared_received_data->images_size_kb = received_data.images_size_kb;
    shared_received_data->count_astronomical_data = received_data.count_astronomical_data;
    shared_received_data->acum_constellations = received_data.acum_constellations;

    //Unlink from the shared memory reion
    shmdt(shared_received_data);

    SEM_signal(&sem_received);
}

void shareLastAtronomicalData() {
    SEM_wait(&sem_received);

    //Save the values
    ReceivedAstronomicalData *shared_last_data = shmat(id_last_data, NULL, 0);

    shared_last_data->count_constellations = last_astronomical_data.count_constellations;
    shared_last_data->acum_density = last_astronomical_data.acum_density;
    shared_last_data->min_size = last_astronomical_data.min_size;
    shared_last_data->max_size = last_astronomical_data.max_size;

    //Unlink from the shared memory reion
    shmdt(shared_last_data);

    SEM_signal(&sem_received);
}
