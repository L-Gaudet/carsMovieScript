/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <Python.h>
#include <pthread.h>

PyObject *pyModule;

double getTemperature(PyObject *pyModule){
    double temp = 0.0;
    
    PyObject *pFunc = PyObject_GetAttrString(pyModule, "getTemperature");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject *pValue = PyObject_CallObject(pFunc, NULL);
        temp = PyFloat_AsDouble(pValue);
        Py_DECREF(pValue);
    } else {
        PyErr_Print();
    } 
    //printf("%lf\n", temp);
    Py_DECREF(pFunc);
    
    return temp;
}

double getHumidity(PyObject *pyModule){
    double hum = 0.0;
    
    PyObject *pFunc = PyObject_GetAttrString(pyModule, "getHumidity");
    if(pFunc && PyCallable_Check(pFunc)) {
        PyObject *pvalue = PyObject_CallObject(pFunc, NULL);
        hum = PyFloat_AsDouble(pvalue);
        Py_DECREF(pvalue);
    } else {
        PyErr_Print();
    }
    //printf("%lf\n", hum);
    Py_DECREF(pFunc);
    
    return hum;
}

double getPressure(PyObject *pyModule){
    double press = 0.0;
    
    PyObject *pFunc = PyObject_GetAttrString(pyModule, "getPressure");
    if(pFunc && PyCallable_Check(pFunc)) {
        PyObject *pvalue = PyObject_CallObject(pFunc, NULL);
        press = PyFloat_AsDouble(pvalue);
        Py_DECREF(pvalue);
    } else {
        PyErr_Print();
    }
    //printf("%lf\n", press);
    Py_DECREF(pFunc);
    
    return press;
}

void displayMsg(PyObject *pyModule, char *message){

    PyObject *pfunc = PyObject_GetAttrString(pyModule, "displayMsg");
    PyObject *arglist;
    if(pfunc && PyCallable_Check(pfunc)) {
        arglist = Py_BuildValue("(s)", message);
        PyObject *pvalue = PyObject_CallObject(pfunc, arglist);
        Py_DECREF(pvalue);
        Py_DECREF(arglist);
    } else {
        PyErr_Print();
    }
    Py_DECREF(pfunc);
    
    return;
   
}

void *thread_function(void* thread_arg){
    
     PyObject *pModule;
     pModule = pyModule;
    
    int socketfd = *(int *)thread_arg;
    char commandstr[128];
    
    char cmd[32];
    char option[32];
    
    char temp[128];
    char hum[128];
    char press[128];
    
    
    while(1){
    
        if((read(socketfd, commandstr, sizeof(commandstr))) == -1){
            perror("Error: ");
            printf("error reading socket\n");
            exit(-1);
        }
        
        sscanf(commandstr, "%s %s", cmd, option);
        //printf("cmd=%s;\n", cmd);
        //printf("option=%s;\n", option);
        
        
        if(strcmp(cmd, "GET") == 0){
            if(strcmp(option, "TEMPERATURE") == 0){
                //printf("Temperature: %lf \n", getTemperature(pModule));
                sprintf(temp, "%lf", getTemperature(pModule));
                //Py_Finalize();
                write(socketfd, temp, sizeof(temp));
                
            }
            else if(strcmp(option, "HUMIDITY") == 0){
                //char hum[128];
                //printf("Humidity = %lf \n", getHumidity(pModule));
                sprintf(hum, "%lf", getHumidity(pModule));
                //Py_Finalize();
                write(socketfd, hum, sizeof(hum));
                
            }
            else if(strcmp(option, "PRESSURE") == 0){
                //char press[128];
                //printf("Pressure = %lf \n", getPressure(pModule));
                sprintf(press, "%lf", getPressure(pModule));
                write(socketfd, press, sizeof(press));
                
            }
            else{
                printf("invalid input\n");  
                exit(-1);
            }
        }
        else if(strcmp(cmd, "SET") == 0){
            displayMsg(pModule, option);
            write(socketfd, option, sizeof(option));
        }
        else if(strcmp(cmd, "EXIT") == 0){
            //printf("EXIT");
            
            write(socketfd, "closing", sizeof("closing"));
            close(socketfd);
            pthread_exit(NULL);
            
        }
        else{
            printf("invalid input\n");
            exit(-1);
        }
        

    }
    
    close(socketfd);
    
    pthread_exit(NULL);
    
    
    
    //parse command string for command (GET or SHOW)
    //check to see that command is a valid command
    //if invalid send message back to client over socket that it is an invalid command
        //*******    //loop back and read again
    //if command is GET verify valid option 
    //if option TEMP call getTemperature() 
        //return temperatire back to client over socket
    //if option PRESSURE call getPressure()
        //return pressure to client over socket
    //if option HUMIDITY call getHumidity()
        //return humidity to client over socket
    //if command is SHOW then call fucntion to display message (showMessage)
        //return message to client over socket
}

int main(int argc, char *argv[])
{
    //printf("ipadd=%s:\n", argv[1]);
    
    Py_Initialize();
    
    
    PyObject *pyname = PyUnicode_FromString("senseHatModule");
    if(pyname == NULL){
        PyErr_Print();
        exit(-1);
    }
    
    pyModule = PyImport_Import(pyname);
    if(pyModule == NULL){
        PyErr_Print();
        exit(-1);
    }
    
    int port = atoi(argv[2]);
    
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    
    struct sockaddr_in client_address;
    
    pthread_t tid;

        //command line ipaddr
        

/*  Remove any old socket and create an unnamed socket for the server.  */

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket.  */

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    //change port
    server_address.sin_port = htons(port);
    
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    
    //put python module here 

/*  Create a connection queue and wait for clients.  */

    listen(server_sockfd, 5);
    while(1) {

        printf("server waiting\n");

/*  Accept a connection.  */

        client_len = sizeof(client_address);
        
        //clients side: i want to talk server
        //accept connection -- create new thread to talk to client
        //main will wait for another client to show up
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
        printf("Socket fd accept: %d\n", client_sockfd);
        
    
        int res;
        res = pthread_create(&tid, NULL, thread_function, (void *)&client_sockfd);
        if (res != 0){
            perror("Thread creation failed\n");
            exit(-1);
        }
    
    }
    
    
    Py_Finalize();
    exit(0);
}

