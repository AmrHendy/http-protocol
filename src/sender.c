//
// Created by Amr Hendy on 11/16/2018.
//

#include "sender.h"
#include "client.h"

const long long TIMEOUT = 5LL:

// hostname and port_number of the server , fd of client socket
void sendRequest(requestType type, char* hostname, char * port_number, char * file_url, int client_socketfd){
    if(type == GET){
        sendGETRequest(hostname, port_number, file_url, client_socketfd);
    }
    else if(type == POST){
        sendPOSTRequest();
    }
    else{
        perror("unsupported request type");
    }
}

void sendGETRequest(char* hostname, char * port_number, char * file_url, int client_socketfd) {
    char * request_buffer = (char *) malloc(1000);
    memset(request_buffer, '\0', sizeof(request_buffer))
    int length = sprintf(request_buffer,"GET /%s HTTP/1.1\r\nHOST: %s:%s\r\n\r\n",file_url, hostname, port_number);
    puts(request_buffer);
    if(length > 0){
        int status = sendRequest(request_buffer, length, client_socketfd);
        if(status == 0){
            // i don't know whether we should send the request again or ignore.
        }
    }
    free(request_buffer);
}


/* sends buffer of chars over socket */
int sendBufferToSocket(char *buffer, int buffer_size, int socketfd){
    int sent = 0;
    unsigned long long curTime = getCurrentSeconds();
    // checking Timeout as socket may fail for many many times so we will stop trying to repeat.
    while(sent < buffer_size && getCurrentSeconds() - curTime < TIMEOUT){
        sent += send(socketfd, (void *)(buffer + sent), buffer_size - sent, 0);
    }
    if(sent != buffer_size) {
        // then that means not all characters are sent because of timeout */
        return 0;
    }
    return 1;
}
