//
// Created by Amr Hendy on 11/16/2018.
//

#include "receiver.h"

void openFileWithPathAndSend(string file_path, int client);
void parse_data_from_file(char * buffer, int buffer_size, int &data_start_position, int &data_content_length);
void receiveGETRequest(int client, int request_size);
void receivePOSTRequest(int client_socketfd);

// input regex. "GET /index.html HTTP/1.1\r\nHOST: 0.0.0.0:2000\r\n\r\n"
static const string REGEX_GET_POST = "((GET|POST)\\s/(.+)\\s(HTTP.+)\\r\\n(.+).*?)";

/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd) {
    printf("=============================================\n");
    if (strncmp(request, "GET", 3) == 0) {
        string tmp(request);
        receiveGETRequest(client_socketfd, request_size);
        cout << "Sent File Complete (Finished GET Request at Server)" << endl;
    } else if (strncmp(request, "POST", 4) == 0) {
        receivePOSTRequest(client_socketfd);
        cout << "Received File Complete (Finished POST Request at Server)" << endl;
    } else {
        perror("unsupported type request");
    }
}


/*
 * Server call this function to respond to GET request from client.
 */

void receiveGETRequest(int client, int request_size){
    char* buffer = (char*) malloc(request_size);
    int val_read = read(client , buffer, request_size);
    string request(buffer);
    string file_path = parse_req(request, 2);
    openFileWithPathAndSend(file_path, client);
}

void receivePOSTRequest(int client_socketfd) {
    const int MAX_SIZE = 500000;
    char * buffer = (char *) malloc(MAX_SIZE);


    // read the first part to parse the file url, and not advancing the socket pointer
    int status = recv(client_socketfd, buffer, MAX_SIZE, 0);
    if(status < 0){
        perror("error receiving packages");
        if(!buffer){
            free(buffer);
        }
        return;
    }
    string req(buffer);
    char *file_url = strdup(parse_req(req, 2).c_str());

    if(file_url == NULL) {
        perror("invalid file to write in");
        return;
    }
    int data_start_position, data_content_length;
    parse_data_from_file(buffer, status, data_start_position, data_content_length);
    // we should not enter that condition unless failure occur.
    if(data_content_length <= 0){
        perror("Invalid POST REQUEST IN SERVER As content not exist in the request");
        return;
    }

    /* call here send post respone which include OK. */
    string OkResponse = "HTTP/1.1 200 OK\r\n";
    sendBufferToSocket(strdup(OkResponse.c_str()), OkResponse.size(),client_socketfd);

    // receive the file conetent after the client respond parse the request to get the data
    status = recv(client_socketfd, buffer, data_start_position + data_content_length, 0);
    if(status <= 0){
        perror("Very long waiting for file to be sent.\n");
    }
    char * file_buffer = (char *)malloc(data_content_length);
    int current_index = 0;
    // we may not recieve all the data content completely if the data content is large.
    for(; current_index < status ; current_index++){
        file_buffer[current_index] = buffer[current_index];
    }
    // continue reading until we finish reading all the content
    while(data_content_length - current_index > 1){
        status = recv(client_socketfd, (char *)(file_buffer + current_index), data_content_length - current_index, 0);
        current_index += status;
    }

    /*open new file */
    FILE * fp = fopen(file_url, "wb");
    if(!fp){
        perror("Server could not write the response to the file");
        return;
    }
    printf("Server is writing the data content = %d into the file %s\n",data_content_length, file_url);
    fwrite(file_buffer, data_content_length, 1, fp);
    fclose(fp);
    if(!file_buffer){
        free(file_buffer);
    }
    if(!buffer){
        free(buffer);
    }
    return;
}

/**
 *
 * @param file_path => file name
 * @param client => client socket
 */
void openFileWithPathAndSend(string file_path, int client) {
    FILE* file;
    file = fopen(file_path.substr(1, file_path.size()).c_str(), "rb");

    if (file)//if i found the file i can send it back to browser
    {
        string tmp("HTTP/1.1 200 OK\r\nContent-Length: " + to_string(getFileLength(file)) + "\r\n\r\n");
        char statusLine[tmp.size() + 1];
        for(int i = 0 ;i < tmp.size(); i++){
            statusLine[i] = tmp[i];
        }
        statusLine[tmp.size()] = '\0';
        //cout << "The file :" << file_path << " was opened." << endl;
        sendFile(file, client, statusLine);
    }
    else//i didnt find the file i have to send 404 page not found
    {
        string responseNotFound = "HTTP/1.1 404 Not Found \r\n";
        cout << "File not found." << endl;
        send(client, responseNotFound.c_str(), (int)responseNotFound.size(), 0);
    }

    if (file)//if i opened the file i have to close it
    {
        fclose(file);
    }
}

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