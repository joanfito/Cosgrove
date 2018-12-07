/*******************************************************************
*
* @File: Files.c
* @Purpose: Manage the files received
* @Author: Joan Fitó Martínez
* @Author: Adrián García Garrido
* @Date: 04/12/2018
*
********************************************************************/

#include "Files.h"

Files createFiles() {
     Files files;

     files.images = (Image*)calloc(1, sizeof(Image));
     files.astronomical_data = (AstronomicalData*)calloc(1, sizeof(AstronomicalData));
     files.num_images = 0;
     files.num_astronomical_data = 0;

     return files;
}

int isImage(char *type) {
     return (strcmp("jpg", type) == 0 || strcmp("JPG", type) == 0);
}

int isAstronomicalData(char *type) {
     return (strcmp("txt", type) == 0 || strcmp("TXT", type) == 0);
}

char *calculateChecksum(char *filename) {
     int md5sum, pipe_fd[2], endOfFile = 0;
     char *checksum, *output;

     checksum = calloc(1, sizeof(char));

     if(pipe(pipe_fd) != -1) {
          md5sum = fork();

          switch (md5sum) {
               case 0:
                    //Close the reading and the stdout pipe
                    close(STDOUT_FILENO);
                    close(pipe_fd[0]);

                    //Redirect the stdout pipe to the writing pipe
                    dup(pipe_fd[1]);

                    //Calculate the md5sum
                    execl("/usr/bin/md5sum","md5sum", filename, NULL);

                    close(pipe_fd[1]);
                    break;
               default:
                    //Close the writing pipe
                    close(pipe_fd[1]);
                    output = readLine(pipe_fd[0], ' ', &endOfFile);

                    checksum = realloc(checksum, sizeof(char) * (strlen(output) + 1));
                    strcpy(checksum, output);
                    free(output);

                    //Wait until the checksum is calculated
                    wait(0);
                    close(pipe_fd[0]);

                    break;
          }
     }
     return checksum;
}

int getFileSize(char *filename) {
     int fd, compt = 0;
     char aux;

     fd = open(filename, O_RDONLY);

     if (fd < 0) {
          close(fd);
          return -1;
     } else {
          while (read(fd, &aux, 1) > 0) compt++;
     }

     close(fd);
     return compt;
}

int createFile(char *filename) {
    int fd = open(filename, O_CREAT | O_EXCL, 0644);

    if (fd < 0) {
        //If the file already exists, we remove it and create it again
        removeFile(filename);
        fd = open(filename, O_CREAT | O_EXCL, 0644);
        if (fd < 0) return FILE_CREATED_KO;
    }

    close(fd);
    return FILE_CREATED_OK;
}

void removeFile(char *filename) {
     int script;

     script = fork();

     switch (script) {
          case 0:
               execl("/bin/rm","rm", "-rf", filename, NULL);
               break;
          default:
               wait(0);
               break;
     }
}

char *typeToLowerCase(char *filename) {
    int i = 0;

    //Go to the type
    while (filename[i] != '.') {
        i++;
    }
    i++;

    //If the type is in upper case, change it to lower case
    while (filename[i] != '\0') {
        if (filename[i] >= 'A' && filename[i] <= 'Z') filename[i] = filename[i] + ('a' - 'A');
        i++;
    }

    return filename;
}

void removeLastImage(Files *files) {
    files->images = realloc(files->images, sizeof(Image) * (files->num_images));
    files->num_images--;
}

void removeLastAstronomicalData(Files *files) {
    files->astronomical_data = realloc(files->astronomical_data, sizeof(AstronomicalData) * (files->num_astronomical_data));
    files->num_astronomical_data--;
}

int saveReceivedFiles(Files files) {
    int fd, i, bytes;
    char *line;

    fd = open(KALKUN_PATH, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return KALKUN_SAVED_KO;

    //Save the images
    for (i = 0; i < files.num_images; i++) {
        //If the image has been received successfuly, we save it
        if (files.images[i].percentage == 100) {
            bytes = asprintf(&line, KALKUN_LINE_PATTERN, getFormattedDate(files.images[i].date), getFormattedTime(files.images[i].time), files.images[i].size);
            write(fd, line, bytes);
        }
    }

    //Save the astronomical data
    for (i = 0; i < files.num_astronomical_data; i++) {
        bytes = asprintf(&line, KALKUN_LINE_PATTERN, getFormattedDate(files.astronomical_data[i].date), getFormattedTime(files.astronomical_data[i].time), files.astronomical_data[i].size);
        write(fd, line, bytes);
    }

    free(line);
    return KALKUN_SAVED_OK;
}
