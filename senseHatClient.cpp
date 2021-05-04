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
      
      // sends client choice to server
      write(sockfd, &opt, 1);
      
      // each case handles a different option 
      switch(opt) {
        case '1':
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << "Temperature is: " << buffer << endl;
          break;

        case '2':
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << "Pressure is: " << buffer << endl;
          break;

        case '3':
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << "Humidity is: " << buffer << endl;
          break;

        case '4':
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << buffer;
          cin >> buffer;
          cout << endl;
          write(sockfd, &buffer, 50);
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << buffer << endl;
          break;

        case '5':
          sprintf(buffer, "Exit");
          exit(99);

        default:
          cout << "waiting for server...\n" << endl;
          read(sockfd, &buffer, 20);
          cout << buffer << endl;
      }
    }

}
