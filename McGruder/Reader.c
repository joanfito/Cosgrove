/*******************************************************************
*
* @File: Reader.c
* @Purpose: Manages the reading from files descriptors
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 16/10/2018
*
********************************************************************/

#include "Reader.h"

char *readLine(int fd, char delimiter, int* endOfFile) {
     int size, end, read_value;
     char buff[1];
     char *input;
     input = malloc(sizeof(char) * 1);
     size = 1;
     end = 0;
     while(end == 0) {
          read_value = read(fd, buff, 1);
          if (read_value <= 0) {
               //EOF or error
               *endOfFile = end = 1;
          } else {
               if(buff[0] == delimiter) {
                    end = 1;
                    break;
               }
          }
          //Copy the new character
          input[size - 1] = buff[0];
          size++;
          input = realloc(input, sizeof(char) * size);

     }
     input[size - 1] = '\0';

     return input;
}
