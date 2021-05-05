#include "stdlib.h"
#include "signal.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>

void *library;
void *fileWrite;
void *fileRead;

int main()
{
    remove("fullFIle.txt");
    FILE *f1, *f2, *f3;
    char str[20];
    char str2[20];
    int filedes[2];
    pthread_t rTh, wTh;

    printf("Str to first file\n");
    scanf("%s", &str);

    f1 = fopen("res/1.txt", "w");
    fprintf(f1, "%s", str);
    fclose(f1);

    printf("Str to 2 file\n");
    scanf("%s", &str2);

    f2 = fopen("res/2.txt", "w");
    fprintf(f2, "%s", str2);
    fclose(f2);

    f3 = fopen("fullFile.txt", "w");
    fclose(f3);

    library = dlopen("/home/yuri/Документы/laba5wra.so", RTLD_LAZY);

    fileWrite = dlsym(library, "fileWrite");
    fileRead = dlsym(library, "fileRead");

    pipe(filedes);
    pthread_create(&rTh, NULL, fileRead, &filedes[1]);

    if (pthread_create(&wTh, NULL, fileWrite, &filedes[0]))
    {
        printf("trabl =(\n");
        pthread_cancel(rTh);
        return 0;
    }

    pthread_join(rTh, NULL);
    pthread_cancel(wTh);
    dlclose(library);

    printf("Completed successful =)");
    return 0;
}
