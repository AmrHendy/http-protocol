//
// Created by Amr Hendy on 11/16/2018.
//

#include "sender.h"
#include "client.h"
#include "receiver.h"

const long long TIMEOUT = 5LL:

// hostname and port_number of the server , fd of client socket
void sendRequest(requestType type, char* hostname, char * port_number, char * file_url, int client_socketfd){
    if(type == GET){
        sendGETRequest(hostname, port_number, file_url, client_socketfd);
        // server will respond to the client with success and content of desired file
        // or failed 404 Not Found
        receiveGETResponse(client_socketfd, file_url);
    }
    else if(type == POST){
        sendPOSTRequest();
        // server will not respond to the client.
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
            // i don't know whether we` should send the request again or ignore.
        }
    }
    free(request_buffer);
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
    char * request_buffer = (char *) malloc(1000);
    memset(request_buffer, '\0', sizeof(request_buffer));
    // Send POST Header
    int post_header_length = sprintf(request_buffer, "POST %s HTTP/1.1\r\nHOST: %s\r\nContent-Length: %d\r\n\r\n",filepath, hostname, file_length);
    printf("POST Header is ==>");
    puts(request_buffer);
    if(post_header_length > 0){
        int status = sendBufferToSocket(request_buffer, post_header_length, client_socketfd);
        free(request_buffer);
        if(status == 0) {
            perror("Can't send POST Request Header from client, So ignore the request");
            return;
        }
    }
    else {
        perror("Can't send POST Request Header from client, So ignore the request");
        return;
    }
    free(request_buffer);
    // Send the File Content
    int status = sendFile(fp, socketfd, length);
    if(status == 0){
        perror("Can't send POST Request from client");
        // i don't know whether we should send the request again or ignore.
    }
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

/* send file over socket */
int sendFileToSocket(FILE * file, int socketfd){
    int file_length = getFileLength(file);
    // we can't send the whole file in one time as sendBufferToSocket then can timeout
    // So we will divide the file into parts and send them one after one.
    const int file_part_length = 1000;
    char * file_content = (char *)malloc(file_part_length);
    while(file_length > 0){
        if(length > CHUNK){
            fread(file_content, min(file_part_length, file_length), 1, file);
            int status = sendBufferToSocket(file_content, min(file_part_length, file_length), socketfd);
            if(status == 0){
                perror("Error happened during sending the file");
                // if error is occured during sending the file, send it again or stop
                // if we want to retry then
                // continue;
                // if we want to stop and send error then
                goto Failure;
            }
            file_length -= min(file_part_length, file_length);
        }
    }
    free(file_content);
    return 1;
    Failure:
    free(file_content);
    return 0;
}

/* return file size */
int getFileLength(FILE * fp){
    fseek(fp, 0, SEEK_END);
    int x = ftell(fp);
    rewind(fp);
    return x;
}
