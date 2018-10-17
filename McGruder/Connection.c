/*******************************************************************
*
* @File: Connection.c
* @Purpose: Manages the connection with Lionel
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 17/10/2018
*
********************************************************************/

#include "Connection.h"

int connect(Configuration config) {
     //return CONNECTION_FAILED;
     return CONNECTION_SUCCESSFUL;
}

int disconnect() {
     //return DISCONNECTION_FAILED;
     return DISCONNECTION_SUCCESSFUL;
}

int sendFile() {
     //return FILE_SENDING_FAILED;
     return FILE_SENDING_SUCCESSFUL;
}

int sendChecksum() {
     //return CHECKSUM_KO;
     return CHECKSUM_OK;
}
