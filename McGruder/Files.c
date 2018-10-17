/*******************************************************************
*
* @File: Files.c
* @Purpose: Search and sends the files of the telescope
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

#include "Files.h"

void scanDirectory(Configuration config) {
     write(1, FILES_NOT_FOUND_MSG, strlen(FILES_NOT_FOUND_MSG));
}
