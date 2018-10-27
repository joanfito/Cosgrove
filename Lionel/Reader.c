/*******************************************************************
*
* @File: Reader.c
* @Purpose: Manages the reading from files descriptors
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 27/10/2018
*
********************************************************************/

#include "Reader.h"

char *readLine(int fd, char delimiter, int* endOfFile) {
     int size, end, read_value;
     char buff;
     char *input;
     input = malloc(sizeof(char) * 1);
     size = 1;
     end = 0;
     while(end == 0) {
          read_value = read(fd, &buff, 1);
          if (read_value <= 0) {
               //EOF or error
               *endOfFile = end = 1;
          } else {
               if(buff == delimiter) {
                    end = 1;
                    break;
               }
          }
          //Copy the new character
          input[size - 1] = buff;
          size++;
          input = realloc(input, sizeof(char) * size);

     }
     input[size - 1] = '\0';

     return input;
}

char *readHeader(int fd, int *endOfFile) {
     int size, end, read_value;
     char buff;
     char *input;
     input = malloc(sizeof(char) * 1);
     size = 1;
     end = 0;
     while(end == 0) {
          read_value = read(fd, &buff, 1);
          if (read_value <= 0) {
               //EOF or error
               *endOfFile = end = 1;
          } else {
               if(buff == ']') {
                    end = 1;
               }
          }
          //Copy the new character
          input[size - 1] = buff;
          size++;
          input = realloc(input, sizeof(char) * size);

     }
     input[size - 1] = '\0';
     return input;
}

char *readData(int fd, int length, int *endOfFile) {
     int read_value, i;
     char buff;
     char *input;
     input = malloc(sizeof(char) * (length + 1));

     for (i = 0; i < length; i++) {
          read_value = read(fd, &buff, 1);
          if (read_value <= 0) {
               *endOfFile = 1;
               break;
          }
          input[i] = buff;
     }
     input[i] = '\0';
     return input;
}
