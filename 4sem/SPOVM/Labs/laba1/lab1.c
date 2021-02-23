#include "stdlib.h"
#include "signal.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>

int main(){

initscr();
char *arg[]={"./lab1cd", NULL};
int pid;
long int times;

pid = fork();

refresh();

switch(pid)
{
case -1:
printw("Can't create fork =(\n PID is %d\n", getpid());
refresh();
break;
case 0:

execvp(arg[0],arg);
getch();
break;
default:

while(1)
{
sleep(2);
times = time (NULL);
move(5,0);
printw("Time Per:: %s",ctime(&times));
refresh();


}
break;
}

//refresh();

endwin();
return 0;
}