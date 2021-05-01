
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <Python.h>

int main(int argc, char *argv[])
{
	int port = atoi(argv[2]);
	int result;
	
	//printf("ippadd=%s:\n", argv[1]);
	
	int client_sockfd;
	int client_len;
	struct sockaddr_in client_address;
	char buffer[1024];
	char buffer2[1024] ;
	
	//check this code
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = inet_addr(argv[1]);
	
	client_address.sin_port = htons(port);
	
	client_len = sizeof(client_address);
	
	result = connect(client_sockfd, (struct sockaddr *)&client_address, client_len);
	
	if(result == -1){
		perror("oops error\n");
		exit(-1);
	}
	
	char gt[128] = "GET TEMPERATURE";
	char gp[128] = "GET PRESSURE";
	char gh[128] = "GET HUMIDITY";
	char e[128] = "EXIT EXIT";
	
	while(1){
		
		int x;
		
		printf("\nSenseHat Menu\n");
		printf("--------------------------------\n");
		printf("1. Get Temperature\n");
		printf("2. Get Pressure\n");
		printf("3. Get Humidity\n");
		printf("4. Set Message\n");
		printf("5. Exit\n");
		
		printf("enter option: ");
		
		scanf("%d", &x);
		
		if(x == 1){
			//check the write
			write(client_sockfd, &gt, sizeof(gt));
			read(client_sockfd, buffer, 1024);
			printf("SenseHat Temperature = %s\n", buffer);
			
		} 
		else if(x == 2){
			write(client_sockfd, &gp, sizeof(gp));
			read(client_sockfd, buffer, 1024);
			printf("SenseHat Pressure = %s\n\n", buffer);
			
		}
		else if(x == 3){
			write(client_sockfd, &gh, sizeof(gh));
			read(client_sockfd, buffer, 1024);
			printf("SenseHat Humidity = %s\n", buffer);
			
		}
		else if(x == 4){
			printf("Enter message: \n");
			scanf("%s", buffer);
			sprintf(buffer2, "%s %s", "SET", buffer);
			write(client_sockfd, buffer2, strlen(buffer2)+1);
			read(client_sockfd, buffer, 1024);
			printf("SenseHat Message = %s\n", buffer);
		}
		else if(x == 5){
			write(client_sockfd, &e, strlen(e)+1);
			read(client_sockfd, buffer, 1024);
			printf("%s\n", buffer);
			close(client_sockfd);
			exit(0);
		}
		else{
			printf("invalid input\n");
			exit(-1);
		}
		
	}
	
	exit(0);
}
