#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <Python.h>
#include <string.h>
#include <iostream>


// server side program

/*
4 functions: these call python code
  - get pressure
  - get temperature
  - get humidity
  - set message
*/

PyObject *pModule;

// function to run python code to get temperature from sense hat
double getTemperature(PyObject *pModule)
{

    double temperature = 0.0;

    PyObject *pFunc = PyObject_GetAttrString(pModule, "getTemperature");
    if (pFunc && PyCallable_Check(pFunc)) {
      PyObject *pValue = PyObject_CallObject(pFunc, NULL);
    	temperature = PyFloat_AsDouble(pValue);
    	Py_DECREF(pValue);
    } else {
    	PyErr_Print();
    }

    Py_DECREF(pFunc);

    return temperature;
}

// function to run python code to get humidity from sense hat
double getHumidity(PyObject *pModule)
{

    double humidity = 0.0;

    PyObject *pFunc = PyObject_GetAttrString(pModule, "getHumidity");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pValue = PyObject_CallObject(pFunc, NULL);
	humidity = PyFloat_AsDouble(pValue);
	Py_DECREF(pValue);
    } else {
	PyErr_Print();
    }

    Py_DECREF(pFunc);

    return humidity;
}

// function to run python code to get pressure from sense hat
double getPressure(PyObject *pModule)
{

    double pressure = 0.0;

    PyObject *pFunc = PyObject_GetAttrString(pModule, "getPressure");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pValue = PyObject_CallObject(pFunc, NULL);
	pressure = PyFloat_AsDouble(pValue);
	Py_DECREF(pValue);
    } else {
	PyErr_Print();
    }

    Py_DECREF(pFunc);

    return pressure;
}

// need function to display message on sensehat
// function to run python code to set message on sense hat
void setMessage(PyObject *pModule, char *clientMsg)
{

    char *msgSet;
    
    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(clientMsg));

    PyObject *pFunc = PyObject_GetAttrString(pModule, "helloMsg");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
	msgSet = PyString_AsString(pValue);
	Py_DECREF(pValue);
    } else {
	PyErr_Print();
    }

    Py_DECREF(pFunc);

    // void doesnt return anything 
}

void *thread_function(void *client_sockfd){
  /* read socket for command from client (ie get temperature)
    then parse the command ie "GET TEMPERATURE"
    if nothing matches command send error message
    call get temperature function
    return temperature back over the socket
  */
  
  char buffer[50];
  char opt;
  while(1) {

    read((int)client_sockfd, &opt, 1);
    
    //std::cout << opt << std::endl; used for debugging 

    switch(opt) {
      case '1':
        sprintf(buffer, "%.2f", getTemperature(pModule)); 
        //std::cout << buffer << std::endl; // used for debugging 
        write((int)client_sockfd, &buffer, 20);
        break;

      case '2':
        sprintf(buffer, "%.2f", getPressure(pModule));
        //std::cout << buffer << std::endl; // used for debugging 
        write((int)client_sockfd, &buffer, 20);
        break;

      case '3':
        sprintf(buffer, "%.2f", getHumidity(pModule));
        //std::cout << buffer << std::endl; // used for debugging 
        write((int)client_sockfd, &buffer, 20);
        break;

      case '4':
        sprintf(buffer, "enter message: ");
        //std::cout << buffer << std::endl; // used for debugging 
        write((int)client_sockfd, &buffer, 20);
        std::cout << "waiting for client..." << std::endl;
        read((int)client_sockfd, &buffer, 50);
        //std::cout << buffer << std::endl; // used for debugging
        setMessage(pModule, buffer);
        //std::cout << buffer << std::endl; // used for debugging
        sprintf(buffer, "message set");
        //std::cout << buffer << std::endl; // used for debugging
        write((int)client_sockfd, &buffer, 20);
        break;

      case '5':
        pthread_exit(NULL);
        //close((int)client_sockfd);
      default:
        sprintf(buffer, "Invalid request.\n");
        std::cout << buffer << std::endl; // used for debugging 
        write((int)client_sockfd, &buffer, 20);
    }
    //free(buffer);
  }
}

int main(int argc, char *argv[]) {

  int server_sockfd, client_sockfd;
  //int server_len, client_lenatoi(argv[1]);
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  socklen_t server_len, client_len;
  
  
  int port = atoi(argv[1]);

  int res;
  pthread_t a_thread;


  Py_Initialize();


  PyObject *pName = PyUnicode_FromString("senseHatModules");
  if (pName == NULL) {
      PyErr_Print();
      return -1;
  }

  pModule = PyImport_Import(pName);
  if (pModule == NULL) {
      PyErr_Print();
      return -1;
  }


  //              network-socket, tcp (SOCK_STREAM = tcp protocol)
  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Name the socket.
  server_address.sin_family = AF_INET; // set protocol family
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); // sets address
  server_address.sin_port = htons(port); // set port number
  server_len = sizeof(server_address);
  bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

  // create connection queue
  listen(server_sockfd, 5); // listens for connection requests (qeuau up to 5 at any one time)
  while(1) {
    printf("server waiting\n");
    /*  Accept a connection.  */

      client_len = sizeof(client_address); // need
      client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len); // need

      // threads
      // new thread for each client to connect
      res = pthread_create(&a_thread, NULL, thread_function, (void *)client_sockfd);

  }
  
  Py_Finalize();
  exit(60);
}
