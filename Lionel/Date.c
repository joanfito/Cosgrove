/*******************************************************************
*
* @File: Date.c
* @Purpose: Manage dates
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
*
********************************************************************/

#include "Date.h"

char *getFormattedDate(Date date) {
     char *formatted_date;

     asprintf(&formatted_date, DATE_PATTERN, date.year, date.month, date.day);
     return formatted_date;
}

Date createDateFromName(char *name) {
    Date date;
    char *aux;
    int i, j;

    //If the aren't at least 16 characters it is an invalid name for sure
    if (strlen(name) < 16) {
        date.year = DATE_ERROR_VALUE;
        date.month = DATE_ERROR_VALUE;
        date.day = DATE_ERROR_VALUE;

        return date;
    }

    //Get the year
    aux = calloc(1, sizeof(char) * 5);

    j = 0;
    for (i = 0; i < 4; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    date.year = atoi(aux);

    //Get the month
    aux = realloc(aux, sizeof(char) * 3);

    j = 0;
    for (i = 4; i < 6; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    date.month = atoi(aux);

    //Get the day
    j = 0;
    for (i = 6; i < 8; i++) {
        aux[j++] = name[i];
    }
    aux[j] = '\0';

    date.day = atoi(aux);
    free(aux);

    return date;
}
