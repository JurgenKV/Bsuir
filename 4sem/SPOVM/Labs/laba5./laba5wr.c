#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include "signal.h"

int flag = 1;
struct sigaction action;

void actionDeact(int d)
{
    flag = 0;
}

void actionAct(int d)
{
    flag = 1;
}

bool findTxt(char *str)
{

    char findTxt[] = "txt.";
    int i = 0;

    while (str[i + 1])
        i++;

    for (int j = 0; j < 2; j++)
    {
        if (str[i - j] != findTxt[j])
        {
            return 0;
        }
    }
    return 1;
}

bool validation(char *str)
{

    if (!findTxt(str) || !strcmp(str, "fullFile.txt"))
    {
        return 0;
    }
    else
        return 1;
}

void *fileWrite(void *ptr)
{

    struct aiocb writeFile;
    int pipe = *(int *)ptr;
    long int rd;
    char buffer[100];
    int file;

    action.sa_handler = actionAct;
    sigaction(SIGUSR1, &action, NULL);

    action.sa_handler = actionDeact;
    sigaction(SIGUSR2, &action, NULL);

    file = open("fullFile.txt", O_WRONLY || O_CREAT || O_RDONLY);

    writeFile.aio_fildes = file;
    writeFile.aio_offset = 0;
    writeFile.aio_buf = &buffer;

    while (true)
    {
        if (flag == 1)
        {
            signal(SIGUSR1, actionDeact);
            sleep(1);

            rd = read(pipe, buffer, 100);

            writeFile.aio_nbytes = rd;

            aio_write(&writeFile);
            while (aio_error(&writeFile) == 115); 

            writeFile.aio_offset += rd;
            signal(SIGUSR1, actionAct);
        }
    }
}

void *fileRead(void *ptr)
{
    struct stat StructStat;
    struct aiocb readFile;
    DIR *directory;
    struct dirent *nextFile;
    int file = 0;
    int size = 0;
    int pipe = *(int *)ptr;
    char buffer[100];

    action.sa_handler = actionAct;
    sigaction(SIGUSR1, &action, NULL);

    action.sa_handler = actionDeact;
    sigaction(SIGUSR2, &action, NULL);

    readFile.aio_buf = &buffer;

    directory = opendir("./res/");

    while (true)
    {
        sleep(1);
        char FilePath[50] = "./res/";
        nextFile = readdir(directory);

        if (nextFile == NULL)
            break;

        if (!validation(nextFile->d_name))
            continue;

        strcat(FilePath, nextFile->d_name);
        file = open(FilePath, O_RDONLY);

        fstat(file, &StructStat);
        size = StructStat.st_size;
        readFile.aio_fildes = file;
        readFile.aio_offset = 0;

        if (flag == 1)
        {

            signal(SIGUSR1, actionDeact);

            while (true)
            {

                signal(SIGUSR1, actionDeact);
                if (size > 100)
                    readFile.aio_nbytes = 100;
                else
                    readFile.aio_nbytes = size;

                aio_read(&readFile);
                while (aio_error(&readFile) == 115)
                    ;

                write(pipe, buffer, readFile.aio_nbytes);
                signal(SIGUSR1, actionAct);

                if (size > 100)
                {
                    size -= 100;
                    readFile.aio_offset += 100;
                }
                else
                    break;
            }
        }
    }
    close(file);
}