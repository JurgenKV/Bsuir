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
#include <unistd.h>
#include <pthread.h>

using namespace std;

struct sigaction action;

int j = 0;
int i = 0;
int flag = 0;
int last = 0;

void actionDeact(int d){
   flag = 0;
}

void actionAct(int d){
   flag = 1;
}
/*
void lastAdd(int d){
   last = 1;
}
*/
pthread_t threadMS[100];   

    char choice; 

void *thread_function(void * arg)
{
    int pidTH = i;
    while(1)
    {
        sleep(4);   
        signal(SIGUSR1, actionAct);
        flag = 1;
        if (flag == 1){
        signal(SIGUSR2, actionDeact);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
        flag = 0;
        cout << pidTH << endl;  
        flag = 1;
       // if(pidTH == i)
         //signal(SIGUSR2, lastAdd); 
        }
    }
}

int main(int argc, char *argv[])
{
     action.sa_handler = actionAct;
     sigaction(SIGUSR1,&action, NULL);

     action.sa_handler = actionDeact;
     sigaction(SIGUSR2,&action, NULL);

        void* threadData = NULL;

    printf("\n '+' - add\n '-' - del\n 'q' - quit\n");          
    while(true)
    {
      //  start:
    
        std::cin >> choice;
        switch (choice)
        {
        case '+':
        {
           i++; 
	       pthread_create(&threadMS[i], NULL, thread_function, threadData);
	    
        break;
     //goto start;
        }
        case '-':
        { 
                      if(i == 0){
                          std::cout << "\r\nYour delete all threads!\r\n";
                          pthread_cancel(threadMS[0]);
                      }
                      else
                      {
                          pthread_cancel(threadMS[i]);   
                          //  signal(SIGUSR1, actionAct);
                          //    flag = 1;
                          std::cout << "\r\n You Kill thread #\t"<< i << std::endl;
                          i--;
                      }
                     // goto start;
                  break;
        }
        case 'q':

          for(; i > 0; i--)
          {
             pthread_cancel(threadMS[i]);
          }
            return 0;
        
        default:
            cout << "Bad Ch" << endl;
            //goto start;
        }
    }  
    //printf("\nEnd.\n");
    return 0;
}