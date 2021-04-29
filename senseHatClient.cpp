#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

// client side program

int main(int argc, char *argv[]); {

    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char opt[];


    // Create a socket for client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // name socket same as server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(atoi(argv[1]));
    address.sin_port = htons(atoi(argv[2]));
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
        case "1":
          opt = "Get Temperature";
          write(sockfd, &opt, 20);
          cout << "waiting for server..." << endl;
          while(opt=="Get Temperature"){
              read(sockfd, &opt, 20);
          }
          cout << "Temperature is: " << opt << "degrees celsius" << endl;
      }

    }

}
