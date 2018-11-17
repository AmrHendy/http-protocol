//
// Created by Amr Hendy on 11/16/2018.
//

#include "receiver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Client call this function to receive response from the serevr.
 */
void receiveResponse(){

}

/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd){
    if(strncmp(request, "GET", 3) == 0){
        receiveGETRequest(request, request_size, client_socketfd);
    }
    else if(strncmp(request, "POST", 4) == 0){
        receivePOSTRequest(request, request_size, client_socketfd);
    }
    else{
        perror("unsupported type request");
    }
}

/*
 * Server call this function to respond to GET request from client.
 */
void receiveGETRequest(char *get_request, int request_size, int client_socketfd){

}

/*
 * Server call this function to respond to POST request from client.
 */
void receivePOSTRequest(char *post_request, int request_size, int client_socketfd){
    const int MAX_SIZE = 1000;
    char *file_url = (char *)malloc(sizeof(MAX_SIZE));
    sscanf(post_request, "POST %S", file_url);
    if(file_url == NULL) {
        perror("invalid file to write in");
        return;
    }


    int data_start_position, data_content_length;
    for(int i=0; i < request_size; i++){
        if(i > 2){
            if(strncmp(post_request + i - 3, "\n\r\n\r", 4) == 0){
                data_start_position = i+1;
            }
        }
    }
    int cur_index = -1, last_index = -1;
    for(int i =0 ; i < request_size ; i++){
        if(i > 1){
            if(post_request[i] == '\n' && post_request[i-1] == '\r'){
                if(cur_index == -1){
                    cur_index = i-2;
                }else{
                    last_index = cur_index + 3;
                    cur_index = i - 2;
                    int flag = 1;
                    char * str = "Content-Length: ";
                    for(int idx=0 ; str[idx] != 0 && post_request[idx + last] != 0 ; idx++){
                        if(post_request[idx + last] != str[idx]) {
                            flag = 0;
                            break;
                        }
                    }
                    if(flag == 1){
                        data_content_length = 0;
                        int numbers_index = last_index + 16;
                        while(post_request[numbers_index] >= '0' && post_request[numbers_index] <= '9'){
                            data_content_length *= 10;
                            numbers_index += (post_request[numbers_index] - '0');
                            numbers_index++;
                        }
                        goto finished;
                    }
                }
            }
        }
    }
    data_content_length = -1;

    finished:
    if(data_content_length <= 0){
        perror("Invalid Request As No Content is sent in POST request");
        return;
    }

    FILE * fp = fopen(file_url, "wb+");
    if(!fp || fp == NULL){
        perror("Error, Could not write to file");
        return;
    }
    // Write all data content in this buffer then to the file.
    char * content_buffer = (char *)malloc(data_content_length);
    for(int current_index = 0; indx < request_size - data_start_position; current_index++){
        content_buffer[current_index] = post_request[current_index + data_start_position];
    }
    // Write the file
    fwrite(content_buffer, data_content_length, 1, fp);
    fclose(fp);
    free(file_url);
    free(content_buffer);
}
