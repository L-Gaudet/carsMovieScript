#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


#define PASSWORD_LEN 8

int main()
{
    char buffer[128]; //set the buffer
    int result, nread; //setup result and nread
    
    fd_set inputs, testfds; //setup fd_set
    struct timeval timeout; // setup the structure timeval and timeout
    
    FD_ZERO(&inputs); // setup FD_ZERO
    FD_SET(0,&inputs); // let fd set handle an input 
    
    struct termios initialrsettings, newrsettings;
    char password[PASSWORD_LEN + 1];
    tcgetattr(fileno(stdin), &initialrsettings);
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ECHO;
    
    printf("Enter password: \n");
    
    while(1) {
        
        testfds = inputs; //declare testfds
        timeout.tv_sec = 60; //declare the timer and set to 60 seconds
        
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout); //select line taken from select.c
        
        switch(result) { //output the timneout statement
            case 0:
                printf("timeout\n");
                exit(0);
           // case 1:
               // perror("select");
               // exit(1);
            
            
            default:
                if(FD_ISSET(0,&testfds)) {
                    ioctl(0,FIONREAD,&nread); //read and show the user what they are typing
                    //buffer[nread] = 0;
                    //printf("read from keyboard: %s", buffer);
                } 
        }
        //output system from original password.c
        if(tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0) {
        fprintf(stderr,"Could not set attributes\n");
        }
        else {
            fgets(password, PASSWORD_LEN, stdin);
            tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
            fprintf(stdout, "\nYou entered %s\n", password);
            exit(0); 
        }
    }
    exit(0);
}
