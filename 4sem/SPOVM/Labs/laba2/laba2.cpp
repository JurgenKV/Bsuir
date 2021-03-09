#include "stdlib.h"
#include "signal.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <semaphore.h>

using namespace std;

int i = 0;
pid_t ms[10];   
long int w = 1;;
int flag = 0;

void action(int d){
   flag = 1;
}

int main()
{
    char choice;   
    ms[0]=getpid();
    int l = 1;
       if(w == 1)
        {
        flag = 1;
        w = 0;
        }
                    
    while(true)
    {
        start:
        printf("\n '+' - add\n '-' - del\n 'q' - quit\n");
        std::cin >> choice;
        switch (choice)
        {
        case '+':
            i++;
            ms[i] = fork();

            switch (ms[i])
            {
            case -1:
                printf("Can't create fork =(\n PID is %d\n", getpid());
                break;
            case 0:
                     if(flag == 1){
               while(1){
                       flag = 0;
                       sleep(1);
                cout << getpid() << endl;
                signal(SIGUSR1, action);
                //sleep(1);
                   }              
               }
            default:
               goto start;
            }

        case '-':
            kill(ms[i], SIGKILL);
            waitpid(ms[i],NULL,0);
            cout << "deleted" << endl;
            i--;
            goto start;  
        case 'q':
            for(;i != 0; i--){
            kill(ms[i], SIGKILL);
            waitpid(ms[i],NULL,0); 
            }
            return 0;

        default:
            cout << "Bad Ch" << endl;
            goto start;
        }
    }  
    //printf("\nEnd.\n");
    return 0;
}