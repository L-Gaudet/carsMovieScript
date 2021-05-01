#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

// client side program

using namespace std;

int main(int argc, char *argv[]) {

    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char opt;
    char buffer[20];
    char *ip = argv[1];
    int port = atoi(argv[2]);


    // Create a socket for client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // name socket same as server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);
    len = sizeof(address);

    // attach socket to server's socket
    result = connect(sockfd, (struct sockaddr *)&address, len);

    // error handling
    if(result == -1) {
        perror("oops: connect failed");
        exit(1);
    }

    while(1){
      cout << "\nSenseHat Menu" << endl;
      cout << "--------------------" << endl;
      cout << "1. Get Temperature" << endl;
      cout << "2. Get Pressure" << endl;
      cout << "3. Get Humidity" << endl;
      cout << "4. Set Message" << endl;
      cout << "5. Exit" << endl;
      cout << "\nEnter Option: ";
      cin >> opt;
      cout << endl;

      switch(opt) {
        case '1':
          sprintf(buffer, "Get Temperature");
          write(sockfd, &opt, 20);
          cout << "waiting for server...\n" << endl;
          while(buffer=="Get Temperature"){
              read(sockfd, &buffer, 20);
          }
          cout << "Temperature is: " << opt << endl;
          break;

        case '2':
          sprintf(buffer, "Get Pressure");
          write(sockfd, &buffer, 20);
          cout << "waiting for server...\n" << endl;
          while(buffer=="Get Pressure"){
              read(sockfd, &opt, 20);
          }
          cout << "Pressure is: " << opt << endl;
          break;

        case '3':
          sprintf(buffer, "Get Humidity");
          write(sockfd, &buffer, 20);
          cout << "waiting for server...\n" << endl;
          while(buffer=="Get Humidity"){
              read(sockfd, &buffer, 20);
          }
          cout << "Humidity is: " << opt << endl;
          break;

        case '4':
          sprintf(buffer, "Set Message");
          write(sockfd, &buffer, 20);
          cout << "waiting for server...\n" << endl;
          while(buffer=="Set Message"){
              read(sockfd, &buffer, 20);
          }
          cout << buffer;
          cin >> buffer;
          cout << endl;
          char msg[50];
          sprintf(msg, buffer);
          write(sockfd, &buffer, 50);
          cout << "waiting for server...\n" << endl;
          while(buffer==msg){
              read(sockfd, &buffer, 20);
          }
          cout << buffer << endl;
          break;

        case '5':
          sprintf(buffer, "Exit");
          write(sockfd, &buffer, 20);
          exit(99);

        default:
          char invalid[50];
           sprintf(invalid, buffer);
          write(sockfd, &buffer, 20);
          cout << "waiting for server...\n" << endl;
          while(buffer==invalid){
              read(sockfd, &buffer, 20);
          }
          cout << buffer << endl;
      }
      cout << "\n\n"; 

    }

}
