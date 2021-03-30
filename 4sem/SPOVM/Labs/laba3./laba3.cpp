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

using namespace std;

//struct sigaction active;

int serverAct = 0;
char dataBuff[BUFSIZ + 50];
char data[50];
int w = 0;
pid_t pid;
int i = 0;
int transfering = 0;

//void Server(int num){
//  serverAct = 1;
//}

void WaitSig(int num){
  w = 1;
}

void transfer(int num){
  transfering = 1;
}


int main()
{
  string bt;
  int filedes[2];
  //active.sa_handler = Server;
  //sigaction(SIGUSR1,&active, NULL);
  pipe(filedes);
  
  pid = getpid();
  pid = fork();

  switch(pid){
    case -1:
      printf("Can't create fork =(\n PID is %d\n", getpid());
    case 0:
      close(filedes[1]);
      while(true){
            sleep(1);
          sleep(1);
          read(filedes[0], dataBuff, sizeof(dataBuff));
          cout << "Data (Server): \n" << dataBuff << "\n "<< endl;
        //  serverAct = 0;
      }
     default:
      while(true){

    close(filedes[0]);
    cout << "--------------------------------" << endl;
    cout << "input string Data (Client):" << endl;
    cin >> data;
    write(filedes[1], data, (strlen(data)+1));
    //signal(SIGUSR1, Server);
    serverAct = 1;
    sleep(2);
    if(data[0] == '0'){
            kill(pid, SIGKILL);
            waitpid(pid,NULL,0);
            break; 
    }   
   }   
  }
   
   return 0;

}