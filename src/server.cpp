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

void handle_request(int client_fd);
int waitForRequest(int client_fd);
void handle_connection(int client_fd);
void init_server(int port_number);

const int SERVER_CONNECTION_QUEUE_SIZE = 10;
const int MAX_ALLOWED_CONNECTIONS = 20;
const int MAX_ALLOWED_REQUESTS_PER_CONNECTION = 30;
int server_socketfd;
// number of clients can be used as counter to handle number of current connections.
int clients;

std::mutex mtx;


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
    printf("Server is waiting for connections\n");

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
        printf("Successfully Established Connection with a Client has fd = %d \n",client);
        /*
         *    handle the connection, by creating new thread and send it all the information needed
         *    and the function which will handle the connection
         */
        printf("Thread Started for the new client.\n");
        if(clients == MAX_ALLOWED_CONNECTIONS){
            printf("Reached the max limit number of connections, So server can't handle that client connection\n")
            continue
        }
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

// Handle connection/client
void handle_connection(int client_fd) {
    // That thread will serve the single client
    int requests_count = 0;
    while(1){
        // serving the requests
        int status = waitForRequest(client_fd);
        if(status == -1){
            printf("No more requests from client with fd = %d within the last 5 seconds, So the server will close the client connection\n",client_fd);
            break;
        }
        if(requests_count == MAX_ALLOWED_REQUESTS_PER_CONNECTION){
            printf("Reached the max limit number of requests of the same connection, So server can't handle that request from the client\n")
            break;
        }
        requests_count++;
        handle_request(client_fd);
        requests_count--;
    }
    mtx.lock();
    clients--;
    mtx.unlock();
}

// return status : 1 in case of received request or -1 in case of not receiving so timeout
int waitForRequest(int client_fd){
    struct timeval tv;
    /* wait up to 5 seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(client_fd, &rfds);
    int retval = select(1, &rfds, NULL, NULL, &tv);
    return (retval!= -1) ? 1 : -1;
}

// TODO will the requests run in separate thread / parallel or the same thread / sequential
// TODO problems with multiple thread :
// select will return true which means i have a new request but that is wrong as socket may recev data from old request also not new requests always
// also we will need mutex for every fd , that mutex will be used as we use in receiver
void handle_request(int client_fd){
    const int request_size = 10000;
    char* buffer = (char*) malloc(request_size);
    int val_read = recv(client_fd , buffer, request_size, MSG_PEEK);
    receiveRequest(buffer, val_read, client_fd);
}