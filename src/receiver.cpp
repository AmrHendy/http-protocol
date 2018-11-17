//
// Created by Amr Hendy on 11/16/2018.
//

#include "receiver.h"


// input regex. "GET /index.html HTTP/1.1\r\nHOST: 0.0.0.0:2000\r\n\r\n"
static const string REGEX_GET_POST = "((GET|POST)\\s/(.+)\\s(HTTP.+)\\r\\n(.+).*?)";

/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd) {
    if (strncmp(request, "GET", 3) == 0) {
        string tmp(request);
        receiveGETRequest(tmp, client_socketfd);
    } else if (strncmp(request, "POST", 4) == 0) {
        receivePOSTRequest(request, request_size, client_socketfd);
    } else {
        perror("unsupported type request");
    }
}

/*
void receiveRequest(int request_size, int client_socketfd){
    //TODO:: I see that we don't need char *request.

    char buffer[request_size] = {0};
    int val_read = read( new_socket , buffer, request_size);
    string req_str(buffer);
    string req = parse_req(req_str, 1);
    if(req == "GET"){
        receiveGetRequest(req_str);
    }else if(req == "POST"){
    }
}
*/

/*
 * Server call this function to respond to GET request from client.
 */

void receiveGETRequest(string req_str, int client){
    //TODO:: I changed this to string as i see that we don't need to read it again.
    string file_path = parse_req(req_str, 2);
    cout << file_path << endl;
    openFileWithPathAndSend(file_path, client);
    cout << "File sent and function returned." << endl;
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
                    for(int idx=0 ; str[idx] != 0 && post_request[idx + last_index] != 0 ; idx++){
                        if(post_request[idx + last_index] != str[idx]) {
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
    for(int current_index = 0; current_index < request_size - data_start_position; current_index++){
        content_buffer[current_index] = post_request[current_index + data_start_position];
    }
    // Write the file
    fwrite(content_buffer, data_content_length, 1, fp);
    fclose(fp);
    free(file_url);
    free(content_buffer);
}


/**
 *
 * @param p_toParse string to parse
 * @param order_of_returned_str = 1 for req type or 2 for file name.
 * @return if there is no match so the request is not HTTP it will return empty string
 */

string parse_req(string p_toParse, int order_of_returned_str) {
    istringstream s(p_toParse);
    string extractedSubmatchPath;
    if(order_of_returned_str == 1){
        s >> extractedSubmatchPath;
        return extractedSubmatchPath;
    }else if(order_of_returned_str == 2){
        s >> extractedSubmatchPath;
        s >> extractedSubmatchPath;
        return extractedSubmatchPath;
    }
    return extractedSubmatchPath;
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
        cout << "The file :" << file_path << " was opened." << endl;
        sendFile(file, client);

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

/**
 *
 * @param file => file to send
 * @param client => client socket
 */

void sendFile(FILE* file, int client) {
    //TODO:: add Content-type.
    char statusLine[] = "HTTP/1.1 200 OK\r\n";


    //get file size.
    fseek(file, 0, SEEK_END);
    int bufferSize = ftell(file);
    cout << "The file lenght is :" << bufferSize << endl;;
    rewind(file);

    //this creates unique pointer to my array
    unique_ptr<char[]> myBufferedFile = make_unique<char[]>(bufferSize);

    //this reads whole file into buffer.
    int numRead = fread(myBufferedFile.get(), sizeof(char), bufferSize, file);
    //bufferSize = numRead;

    int totalSend = bufferSize;

    string content_len = "Content-Length: " + to_string(totalSend) + "\r\n\r\n";
    cout << statusLine << endl;
    cout << content_len << endl;
    char tmp[content_len.size()];
    for(int i=0 ;i < content_len.size(); i++){
        tmp[i] = content_len[i];
    }

    unique_ptr<char[]> myUniqueBufferToSend = make_unique<char[]>(strlen(statusLine) + content_len.size() + bufferSize);

    //add status line.
    memcpy(myUniqueBufferToSend.get(), &statusLine, strlen(statusLine));
    //add status line.
    memcpy(myUniqueBufferToSend.get() + strlen(statusLine), &tmp, content_len.size());
    //add data.
    memcpy(myUniqueBufferToSend.get() + strlen(statusLine) + content_len.size(), myBufferedFile.get(), bufferSize);
    //memcpy(myUniqueBufferToSend.get() + strlen(statusLine), myBufferedFile.get(), bufferSize);

    cout << "Sending response." << endl;
    cout << "start of data at " << strlen(statusLine) + content_len.size() << endl;
    int iResult = send(client, myUniqueBufferToSend.get(), strlen(statusLine) + strlen(tmp) + bufferSize, 0);

    /*
    FILE * ffp;
    ffp = fopen ("tmpp.png", "wb");
    fwrite(myBufferedFile.get(), bufferSize, 1, ffp);
    fclose(ffp);
    */
    if (iResult == -1) {
        printf("send failed with error: %d\n");
        close(client);

    }

    cout << "Total bytes send: " << iResult << endl;
}


/*
 * Client call this function to receive GET response from the server.
 * actually before that call the server finished calling recieveGETRequest so
 * so the client socket will have the file content.
 */
void receiveGETResponse(int client_socketfd, char * filename) {
    char * buffer = (char *) malloc(10000);
    int status = recv(client_socketfd, buffer, 10000, 0);
    if(status < 0){
        perror("error receiving packages");
        free(buffer);
        return;
    }
    // parsing the response
    int resp;
    sscanf(buffer, "HTTP/1.1 %d",&resp);
    char *response = buffer;
    printf("response from server = %d\n",resp);
    if(resp != 200){
        puts(buffer);
        //TODO how to display not found file ???
        free(buffer);
        return;
    }

    int response_size = status;
    int data_start_position, data_content_length;
    for(int i=0; i < response_size; i++){
        if(i > 2){
            if(strncmp(response + i - 3, "\n\r\n\r", 4) == 0){
                data_start_position = i+1;
            }
        }
    }
    int cur_index = -1, last_index = -1;
    for(int i =0 ; i < response_size ; i++){
        if(i > 1){
            if(response[i] == '\n' && response[i-1] == '\r'){
                if(cur_index == -1){
                    cur_index = i-2;
                }else{
                    last_index = cur_index + 3;
                    cur_index = i - 2;
                    int flag = 1;
                    char * str = "Content-Length: ";
                    for(int idx=0 ; str[idx] != 0 && response[idx + last_index] != 0 ; idx++){
                        if(response[idx + last_index] != str[idx]) {
                            flag = 0;
                            break;
                        }
                    }
                    if(flag == 1){
                        data_content_length = 0;
                        int numbers_index = last_index + 16;
                        while(response[numbers_index] >= '0' && response[numbers_index] <= '9'){
                            data_content_length *= 10;
                            numbers_index += (response[numbers_index] - '0');
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
        perror("Invalid GET RESPONSE As not content exist in the response");
        return;
    }
    /*open new file */
    FILE * fp = fopen(filename, "wb");
    if(!fp){
        perror("Client could not write the response to the file");
        return;
    }
    printf("Client is writing the data content = %d into the file %s\n",data_content_length, filename);

    char * file_buffer = (char *)malloc(data_content_length);
    for(int current_index = 0; cur_index < response_size - data_start_position; current_index++){
        file_buffer[current_index] = response[current_index + data_start_position];
    }
    fwrite(file_buffer, data_content_length, 1, fp);
    fclose(fp);
    free(file_buffer);
    free(response);
    free(buffer);
    return;
}