//
// Created by Amr Hendy on 11/16/2018.
//

#include "receiver.h"

void parse_data_from_file(char * buffer, int buffer_size, int &data_start_position, int &data_content_length);

void receiveGETResponse(int client_socketfd, char * filename) {
    char * buffer = (char *) malloc(500000);
    int status = recv(client_socketfd, buffer, 500000, MSG_PEEK);
    if(status < 0){
        perror("error receiving packages");
        free(buffer);
        return;
    }
    // parsing the response
    int resp = atoi(parse_req(buffer, 2).c_str());
    char *response = buffer;
    printf("response from server = %d\n",resp);
    if(resp != 200){
        puts(buffer);
        printf("response has no file.\n");
        //TODO how to display not found file ???
        if(!buffer) free(buffer);
        return;
    }
    int data_start_position, data_content_length;
    parse_data_from_file(buffer, status, data_start_position, data_content_length);
    if(data_content_length <= 0){
        perror("Invalid GET RESPONSE As not content exist in the response");
        return;
    }
    //must be data start position only.


    printf("%d %d\n", data_start_position,data_content_length);
    status = recv(client_socketfd, response, data_start_position + data_content_length, MSG_PEEK);

    char * file_buffer = (char *)malloc(data_content_length);
    int current_index = 0;
    // we may not recieve all the data content completely if the data content is large.
    for(; current_index < status - data_start_position; current_index++){
        file_buffer[current_index] = response[current_index + data_start_position];
    }
    // continue reading until we finish reading all the content
    while(data_content_length - current_index > 1){
        status = recv(client_socketfd, (char *)(file_buffer + current_index), data_content_length - current_index, 0);
        //recv(client_socketfd, buffer, 1000, MSG_PEEK);
        printf("%d\n", status);
        current_index += status;
    }

    /*open new file */
    FILE * fp = fopen(filename, "wb");
    if(!fp){
        perror("Client could not write the response to the file");
        return;
    }
    printf("Client is writing the data content = %d into the file %s\n",data_content_length, filename);
    fwrite(file_buffer, data_content_length, 1, fp);
    fclose(fp);
    free(file_buffer);
    if(!buffer){
        free(buffer);
    }
    //cout << data_start_position <<endl;
    //cout << "the file size is " << data_content_length <<endl;
    return;
}


/*
 * Client call this function to receive GET response from the server.
 * actually before that call the server finished calling recieveGETRequest so
 * so the client socket will have the file content.
 */
void parse_data_from_file(char * buffer, int buffer_size, int &data_start_position, int &data_content_length){
    for(int i=0; i < buffer_size; i++){
        if(i > 2){
            if(strncmp(buffer + i - 3, "\r\n\r\n", 4) == 0){
                data_start_position = i+1;
            }
        }
    }
    for(int i =0 ; i < buffer_size; i++){
        int flag = 1;
        char * str = "Content-Length: ";
        for(int idx=0 ; str[idx] != 0 && buffer[idx + i] != 0 ; idx++){
            if(buffer[idx + i] != str[idx]) {
                flag = 0;
                break;
            }
        }
        if(flag == 1){
            data_content_length = 0;
            int numbers_index = i + 16;
            while(buffer[numbers_index] >= '0' && buffer[numbers_index] <= '9'){
                data_content_length *= 10;
                data_content_length += (buffer[numbers_index] - '0');
                numbers_index++;
            }
            return;
        }
    }
    data_content_length = -1;
    return;
}