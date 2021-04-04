#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PASSWORD_LEN 8

int main() {
  struct termios initialrsettings, newrsettings;
  char password[PASSWORD_LEN + 1];
  tcgetattr(fileno(stdin), &initialrsettings);
  newrsettings = initialrsettings;
  newrsettings.c_lflag &= ~ECHO;

  struct timeval timeout;

  int result;

  fd_set inputs, testfds;

  FD_ZERO(&inputs);
  FD_SET(0,&inputs);

  char buffer[128];

  while(1) {
    printf("Enter password: ");
    printf("\n");
    if(tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0) {
        fprintf(stderr,"Could not set attributes\n");
    }
    else {
        //fgets(password, PASSWORD_LEN, stdin);
        testfds = inputs;
        timeout.tv_sec = 5;
        tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);

        switch (result) {
          case 0:
            printf("timeout\n");
            break;
          case -1:
            printf("okay?");
          default:
            printf("password is: %s\n", buffer);
            exit(1);
        }
      }
  }
  exit(0);
}
