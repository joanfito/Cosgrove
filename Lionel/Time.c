/*******************************************************************
*
* @File: Time.c
* @Purpose: Manage the time
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
*
********************************************************************/

#include "Time.h"

char *getFormattedTime(Time time) {
     char *formatted_time;

     asprintf(&formatted_time, TIME_PATTERN, time.hour, time.minute);
     return formatted_time;
}

Time createTimeFromName(char *name) {
    Time time;
    char *aux;
    int i, j;

    //If the aren't at least 16 characters it is an invalid name for sure
    if (strlen(name) < 16) {
        time.hour = TIME_ERROR_VALUE;
        time.minute = TIME_ERROR_VALUE;
        time.second = TIME_ERROR_VALUE;

        return time;
    }

    //Get the hour
    aux = calloc(1, sizeof(char) * 3);

    j = 0;
    for (i = 9; i < 11; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    time.hour = atoi(aux);

    //Get the minute
    j = 0;
    for (i = 11; i < 13; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    time.minute = atoi(aux);

    //Get the second
    j = 0;
    for (i = 13; i < 15; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    time.second = atoi(aux);
    free(aux);

    return time;
}
