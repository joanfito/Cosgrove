/*******************************************************************
*
* @File: Menu.c
* @Purpose: Manages the menu of the application
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 13/12/2018
*
********************************************************************/

#include "Menu.h"

extern int socket_fd;

void showMenu() {
    write(1, MENU_HEADER, strlen(MENU_HEADER));
    write(1, MENU_OPTION_1_MSG, strlen(MENU_OPTION_1_MSG));
    write(1, MENU_OPTION_2_MSG, strlen(MENU_OPTION_2_MSG));
    write(1, MENU_OPTION_3_MSG, strlen(MENU_OPTION_3_MSG));
    write(1, MENU_INPUT, strlen(MENU_INPUT));
}

int endMenu(int option) {
    return option == 3;
}

int selectOption() {
    int endOfFile = 0, option;
    char *input;

    do {
        showMenu();

        //Read the user's input
        input = readLine(0, '\n', &endOfFile);

        option = atoi(input);
        free(input);

        if (option < 1 || option > 3) write(1, MENU_INPUT_ERROR, strlen(MENU_INPUT_ERROR));
    } while(option < 1 || option > 3);

    return option;
}

void executeOption(int option) {
    switch (option) {
        case 1:
            executeOption1();
            break;
        case 2:
            executeOption2();
            break;
    }
}

void executeOption1() {
    int response;
    short length;
    char type, *header, *data;

    //Send the petition to Lionel
    response = sendFrame(socket_fd, (char)RECEIVED_DATA_FRAME_TYPE, EMPTY_HEADER, 0, NULL);
    if (response == SOCKET_CONNECTION_KO) {
        closeMcTavish();
    }

    //Read the Lionel answer
    response = readFrame(socket_fd, &type, &header, &length, &data);
    if (response == SOCKET_CONNECTION_KO) {
        closeMcTavish();
    }

    //Check if it is the asked frame
    if (type == (char)RECEIVED_DATA_FRAME_TYPE && strcmp(header, EMPTY_HEADER) == 0) {
        int i = 0, j, count_images, images_size, count_astronomical;
        float avg_const;
        char *aux;

        aux = calloc(1, sizeof(char));

        //Read the number of images
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        count_images = atoi(aux);
        i++;

        //Read the total size of images
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        images_size = atoi(aux);
        i++;

        //Read the number of astronomical data files
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        count_astronomical = atoi(aux);
        i++;

        //Read the average of constellations
        j = 0;
        while (data[i] != '\0') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        avg_const = atof(aux);

        free(aux);

        //Print the results
        char *buff;
        int bytes;

        bytes = asprintf(&buff, RECEIVED_IMAGES_PATTERN, count_images);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, IMAGES_SIZE_PATTERN, images_size);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, RECEIVED_ASTRONOMICAL_DATA_PATTERN, count_astronomical);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, AVG_CONSTELLATIONS_PATTERN, avg_const);
        write(1, buff, bytes);
        free(buff);

    } else {
        write(1, DATA_ERROR_MSG, strlen(DATA_ERROR_MSG));
    }

    free(data);
    free(header);
}

void executeOption2() {
    int response;
    short length;
    char type, *header, *data;

    //Send the petition to Lionel
    response = sendFrame(socket_fd, (char)LAST_DATA_FRAME_TYPE, EMPTY_HEADER, 0, NULL);
    if (response == SOCKET_CONNECTION_KO) {
        closeMcTavish();
    }

    //Read the Lionel answer
    response = readFrame(socket_fd, &type, &header, &length, &data);
    if (response == SOCKET_CONNECTION_KO) {
        closeMcTavish();
    }

    //Check if it is the asked frame
    if (type == (char)LAST_DATA_FRAME_TYPE && strcmp(header, EMPTY_HEADER) == 0) {
        int i = 0, j, count_const, min_size, max_size;
        float avg_density;
        char *aux;

        aux = calloc(1, sizeof(char));

        //Read the number of images
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        count_const = atoi(aux);
        i++;

        //Read the total size of images
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        avg_density = atof(aux);
        i++;

        //Read the number of astronomical data files
        j = 0;
        while (data[i] != '&') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        min_size = atoi(aux);
        i++;

        //Read the average of constellations
        j = 0;
        while (data[i] != '\0') {
            aux[j++] = data[i++];
            aux = realloc(aux, sizeof(char) * (j + 1));
        }
        aux[j] = '\0';
        max_size = atoi(aux);

        free(aux);

        //Print the results
        char *buff;
        int bytes;

        bytes = asprintf(&buff, NUM_CONSTELLATIONS_PATTERN, count_const);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, AVG_DENSITY_PATTERN, avg_density);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, MIN_SIZE_PATTERN, min_size);
        write(1, buff, bytes);
        free(buff);

        bytes = asprintf(&buff, MAX_SIZE_PATTERN, max_size);
        write(1, buff, bytes);
        free(buff);
    } else {
        write(1, DATA_ERROR_MSG, strlen(DATA_ERROR_MSG));
    }

    free(data);
    free(header);
}
