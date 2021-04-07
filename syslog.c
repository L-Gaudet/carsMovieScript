#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	
	if( argc == 1){
		printf("Error. Please enter a file name ");
		exit(-1);
	}
	
	else if (argc == 2){
		
		//argv[1] is the filename 

		FILE *fp;
		
		fp = fopen(argv[1], "w+");
		
		if( fp == NULL) {
			syslog(LOG_ERR, "error - No such file or directory");
		}
		else{
		
			syslog(LOG_INFO, "info message %s opened", argv[1]);
			
			fclose(fp);
			
			syslog(LOG_INFO , "info message %s closed", argv[1]);
		}
	}
	
	else {
		printf("Error: too many arguments");
		exit(-1);
	}
	
	exit(0);
}
