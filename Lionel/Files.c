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

extern int id_file;
extern semaphore sem_file;

Files createFiles() {
     Files files;

     //Initialize the variables
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
                    char *cmd = "/usr/bin/md5sum";
                    char *argv[3] = {"md5sum", filename, NULL};
                    execvp(cmd, argv);

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
          return -1;
     } else {
          while (read(fd, &aux, 1) > 0) compt++;
     }

     close(fd);
     return compt;
}

int createFile(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        return FILE_CREATED_KO;
    }

    close(fd);

    return FILE_CREATED_OK;
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
    char *line = NULL, *formatted_time, *formatted_date;

    //Open the Kalkun file in order to save the images
    fd = open(KALKUN_PATH, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return KALKUN_SAVED_KO;

    //Save the images
    for (i = 0; i < files.num_images; i++) {
        //If the image has been received successfuly, we save it
        if (files.images[i].bytes_readed == files.images[i].size) {
            formatted_date = getFormattedDate(files.images[i].date);
            formatted_time = getFormattedTime(files.images[i].time);

            bytes = asprintf(&line, KALKUN_LINE_PATTERN, formatted_date, formatted_time, files.images[i].size);
            write(fd, line, bytes);

            free(formatted_date);
            free(formatted_time);
            free(line);
        }
    }

    close(fd);

    //Open the Watkin file in order to save the astronoical data files
    fd = open(WATKIN_PATH, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return KALKUN_SAVED_KO;

    //Save the astronomical data
    for (i = 0; i < files.num_astronomical_data; i++) {
        formatted_date = getFormattedDate(files.astronomical_data[i].date);
        formatted_time = getFormattedTime(files.astronomical_data[i].time);

        bytes = asprintf(&line, KALKUN_LINE_PATTERN, formatted_date, formatted_time, files.astronomical_data[i].size);
        write(fd, line, bytes);

        free(formatted_date);
        free(formatted_time);
        free(line);
    }

    return KALKUN_SAVED_OK;
}

void printProgressbar(float percentage, int *progress_completed, char *name, int *entered) {
     char *bar;
     int bytes;

     //If its a multiple of 10, we increase the progressbar
     if ((int)percentage % 10 == 0 && *entered == 0) {
          *progress_completed = (int)percentage / 10;

          //Show the current progress
          bytes = asprintf(&bar, RECEIVING_FILE_PER_MSG, name, ((int)percentage/10)*10);
          write(1, bar, bytes);
          free(bar);

          *entered = 1;
     } else if ((int)percentage % 10 != 0) {
         *entered = 0;
     }
}

void readAstronomicalData(char *name) {
    int fd, i = 0;
    char *full_name, *line, aux;
    asprintf(&full_name, FILES_PATH, name);

    fd = open(full_name, O_RDONLY);

    if (fd > 0) {
        SEM_wait(&sem_file);

        //Read the astronomical data file
        line = shmat(id_file, NULL, 0);

        //Copy the data of the file into the shared memory
        while (read(fd, &aux, 1) > 0) {
            line[i++] = aux;
        }
        line[i] = '\0';

        //Unlink from the shared memory region
        shmdt(line);

        SEM_signal(&sem_file);
        close(fd);
    }

    free(full_name);
}
