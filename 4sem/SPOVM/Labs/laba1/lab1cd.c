#include "stdlib.h"
#include "signal.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>


int main()
{
   long int times;
    initscr();
    while(1)
    {   
    sleep(1);
    times = time (NULL);
    move(0,0);
    printw("Time Chi:: %s",ctime(&times));
    refresh();
    }
}