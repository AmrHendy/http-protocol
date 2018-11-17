//
// Created by Amr Hendy on 11/16/2018.
//

#include "server.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>

using namespace std;


// Steps:
// 1) init the server with the host address and port number
// 2) start server by listening on the server socket
// 3) if there is any connection then accept it and make a new thread handling this connection individually
// 4) handle the connection


const int SERVER_CONNECTION_QUEUE_SIZE = 10;
int server_socketfd;
// number of clients can be used as counter to handle number of current connections.
int clients;

std::mutex mtx;

//TODO check that with arsanous  (argument ? void*) (close thread ?) (where receving buufer ? )
void handle_connection(int client_fd){

    const int request_size = 10000;
    char* buffer = (char*) malloc(request_size);
    int val_read = read(client_fd , buffer, request_size);;
    receiveRequest(buffer, val_read, client_fd);

    mtx.lock();
    clients--;
    mtx.unlock();
}


void start_server(int port_number){
    init_server(port_number);
    int status;
    /* start listening on this port */
    status = listen(server_socketfd, SERVER_CONNECTION_QUEUE_SIZE);
    /* if error happened print it*/
    if(status){
        perror("error in listening");
        exit(EXIT_FAILURE);
    }
    printf("server: waiting for connections, Let's start Amr (problem setter, problem solver, machine learning engineer and seeb el koos maftooh \n");

    struct sockaddr_storage client_addr;    /* client address info */
    socklen_t sock_size;
    int client;                          /* client socket descriptor */
    while(1) {
        /* server main loop */
        sock_size = sizeof(client_addr);
        client = accept(server_socketfd, (struct sockaddr *)&client_addr, &sock_size); /* accept connection */
        if(client == -1){
            perror("accept error ");
            continue;
        }
        printf("Successfully Established Connection with a Client \n");
        /*
         *    handle the connection, by creating new thread and send it all the information needed
         *    and the function which will handle the connection
         */
        printf("Thread Started.");

        std::thread t(handle_connection, client);
        t.detach();//this will allow the thread run on its own see join function in docs for more information
        clients++;
    }
}

void init_server(int port_number){
    struct sockaddr_in address;
    //initialize number of clients
    clients = 0;
    address.sin_family = AF_INET;
    // Creating socket file descriptor for the server
    if ((server_socketfd = socket(address.sin_family, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    // Check if it is used and forcefully attaching socket to our desired address and port
    if (setsockopt(server_socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_number);

    int ret = bind(server_socketfd, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}