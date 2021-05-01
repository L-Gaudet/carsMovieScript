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

// before running set environment variable:
//     export PYTHONPATH=/home/pi/Documents


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
char setMessage(PyObject *pModule, char *clientMsg)
{

    char msgSet[50];
    
    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(clientMsg));

    PyObject *pFunc = PyObject_GetAttrString(pModule, "helloMsg");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
	msgSet = pValue;
	Py_DECREF(pValue);
    } else {
	PyErr_Print();
    }

    Py_DECREF(pFunc);

    return msgSet; // return "message set"
}

void *thread_function(void *arg){
  /* read socket for command from client (ie get temperature)
    then parse the command ie "GET TEMPERATURE"
    if nothing matches command send error message
    call get temperature function
    return temperature back over the socket
  */
  char buffer[50];
  while(1) {

    read(client_sockfd, &buffer, 20);

    switch(buffer) {
      case "Get Temperature":
        buffer = std::to_string(getTemperature(pModule));
        write(client_sockfd, &buffer, 20);
        break;

      case "Get Pressure":
        buffer = std::to_string(getPressure(pModule));
        write(client_sockfd, &buffer, 20);
        break;

      case "Get Humidity":
        buffer = std::to_string(getHumidity(pModule));
        write(client_sockfd, &buffer, 20);
        break;

      case "Get Pressure":
        buffer = std::to_string(getPressure(pModule));
        write(client_sockfd, &buffer, 20);
        break;

      case "Set Message":
        buffer = "enter message: ";
        write(client_sockfd, &buffer, 20)
        cout << "waiting for client..." << endl;
        while(buffer=="enter message: "){
          read(client_sockfd, &buffer, 50);
        }
        setMessage(pModule, buffer);
        buffer = "message set";
        write(client_sockfd, &buffer, 20);
        break;

      case "Exit":
        pthread_exit(NULL);
      default:
        buffer = "Invalid request.";
        write(client_sockfd, &buffer, 20);
    }
  }
}

int main(int argc, char *argv[]) {

  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

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
  server_address.sin_port = htons(atoi(argv[1])); // set port number
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
      res = pthread_create(&a_thread, NULL, thread_function, (doublevoid *)message);

  }
  
  Py_Finalize();

}
