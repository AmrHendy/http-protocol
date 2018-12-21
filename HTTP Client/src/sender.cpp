//
// Created by Amr Hendy on 11/16/2018.
//

#include "sender.h"

void waitUnitOK(int client_socketfd, FILE* file);
void sendGETRequest(char* hostname, char * port_number, char * file_url, int client_socketfd);
void sendPOSTRequest(char* hostname, char * port_number, char * file_url, int client_socketfd);

const long long TIMEOUT = 5LL;

// hostname and port_number of the server , fd of client socket
void sendRequest(requestType type, char* hostname, char * port_number, char * file_url, int client_socketfd) {
    if(type == GET){
        sendGETRequest(hostname, port_number, file_url, client_socketfd);
        // server will respond to the client with success and content of desired file
        // or failed 404 Not Found
    } else if(type == POST) {
        sendPOSTRequest(hostname, port_number, file_url, client_socketfd);
        // server will respond to the client by Ok.
    }
    else{
        perror("unsupported request type");
    }
}

void sendGETRequest(char* hostname, char * port_number, char * file_url, int client_socketfd) {
    // Note: If the file is not found (in case of GET), the server should respond with(as
    // would a real http server) HTTP/1.0 404 Not Found\r\n
    char * request_buffer = (char *) malloc(1000);
    memset(request_buffer, '\0', sizeof(request_buffer));
    /*
     * Sending the get request
     */
    int length = sprintf(request_buffer,"GET /%s HTTP/1.1\r\nHOST: %s:%s\r\n\r\n",file_url, hostname, port_number);
    puts(request_buffer);
    if(length > 0){
        int status = sendBufferToSocket(request_buffer, length, client_socketfd);
        if(status == 0){
            perror("Can't send GET Request from client");
        }
    }
    if(!request_buffer){
        free(request_buffer);
    }
}

void sendPOSTRequest(char* hostname, char * port_number, char * file_url, int client_socketfd) {
    FILE *file = fopen(file_url, "rb");
    if(!file ) {
        //file not found
        perror("Can't find the file\n");
        return;
    }
    /*
     * Sending the post request
     */
    char * request_buffer = (char *) malloc(500000);
    memset(request_buffer, '\0', sizeof(request_buffer));
    // Send POST Header
    int post_header_length = sprintf(request_buffer, "POST %s HTTP/1.1\r\nHOST: %s\r\nContent-Length: %d\r\n\r\n",file_url, hostname, getFileLength(file));
    puts(request_buffer);
    sendBufferToSocket(request_buffer, post_header_length, client_socketfd);
    waitUnitOK(client_socketfd, file);
    if(!request_buffer){
        free(request_buffer);
    }
}

void waitUnitOK(int client_socketfd, FILE* file){
    struct timeval tv;
    /* wait up to 5 seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(client_socketfd, &rfds);
    int retval = select(1, &rfds, NULL, NULL, &tv);
    if(retval == -1){
        perror("Response Timeout From Server, 5 seconds exceeded waiting to OK response from server\n");
        return;
    }
    /* If data found send.*/
    printf("OK response received from server, Client start sending the file of POST Request\n");
    sendFile(file, client_socketfd, "");
}
