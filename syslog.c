#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
  // checks if user entered proper arguments
  if (argc == 1){
    printf("Error: no filename specified\n");
    exit(-1);
  }
  else if (argc > 2) {
    printf("Error: too many arguments\n");
    exit(-2);
  }

  //char filename = argv[1];

  FILE *fp;
  fp = fopen(argv[1], "r");

  if (fp == NULL) {
    //syslog(LOG_ERR, "Error: file does not exist\n");
    syslog(LOG_ERR | LOG_USER, "Error: file does not exist\n");
    exit(1);
  }

  syslog(LOG_INFO, "info message %s opened\n", argv[1]);
  fclose(fp);
  syslog(LOG_INFO, "info message %s closed\n", argv[1]);

  exit(0);
}
