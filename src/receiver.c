//
// Created by Amr Hendy on 11/16/2018.
//

#include "receiver.h"
#include <regex>
#include<stdio.h>
#include<iostream>
#include<memory>
#include <fstream>

// input regex.
static const string REGEX_GET_POST = "((GET|POST)\s\/(.+)\s(HTTP.+))";

/*
 * Client call this function to receive response from the serevr.
 */
void receiveResponse(){

}

/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(int request_size, int client_socketfd){
    //TODO:: I see that we don't need char *request.

    char buffer[request_size] = {0};
    int val_read = read( new_socket , buffer, request_size);
    string req_str(buffer);
    string req = parse_req(req_str, 1);
    if(req == "GET"){
        receiveGetRequest(req_str);
    }else if(req == "POST"){
        //TODO:: add POST code here.
    }
}

/*
 * Server call this function to respond to GET request from client.
 */

void receiveGetRequest(string req_str, int client){
    //TODO:: I changed this to string as i see that we don't need to read it again.
    string file_path = parse_req(req_str, 2);
    openFileWithPathAndSend(file_path, client);
}

/*
 * Server call this function to respond to POST request from client.
 */
void receivePOSTRequest(char *get_request, int request_size, int client_socketfd){

}

/**
 *
 * @param p_toParse string to parse
 * @param order_of_returned_str = 1 for req type or 2 for file name.
 * @return if there is no match so the request is not HTTP it will return empty string
 */

string parse_req(string p_toParse, int order_of_returned_str) {
    regex rx(REGEX_GET);
    string extractedSubmatchPath = { "" };

    std::smatch pieces_match;
    if (std::regex_match(p_toParse, pieces_match, rx)) {
        std::ssub_match sub_match = pieces_match[order_of_returned_str];
        extractedSubmatchPath = sub_match.str();
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
    errno_t err;
    err = fopen_s(&file, filePath.c_str(), "r");

    if (err == 0)//if i found the file i can send it back to browser
    {
        cout << "The file :" << filePath << " was opened." << endl;
        sendFile(file, client);

    }
    else//i didnt find the file i have to send 404 page not found
    {
        string responseNotFound = "HTTP/1.0 404 Not Found \r\n";
        cout << "File not found." << endl;
        send(client, responseNotFound.c_str(), (int)responseNotFound.size(), 0);
    }

    if (file)//if i opened the file i have to close it
    {
        err = fclose(file);
        if (err == 0)
        {
            printf("The file was closed\n");
        }
        else
        {
            printf("The file was not closed\n");
        }
    }
}

/**
 *
 * @param file => file to send
 * @param client => client socket
 */

void sendFile(FILE* file, int client) {
    //TODO:: add Content-type.
    char statusLine[] = "HTTP/1.0 200 OK\r\n";

    //get file size.
    fseek(file, 0, SEEK_END);
    int bufferSize = ftell(file);
    cout << "The file lenght is :" << bufferSize << endl;;
    rewind(file);

    //this creates unique pointer to my array
    unique_ptr<char[]> myBufferedFile = make_unique<char[]>(bufferSize);

    //this reads whole file into buffer.
    int numRead = fread_s(myBufferedFile.get(), bufferSize, sizeof(char), bufferSize, file);

    int totalSend = bufferSize + strlen(statusLine);

    unique_ptr<char[]> myUniqueBufferToSend = make_unique<char[]>(totalSend);

    //add status line.
    memcpy(myUniqueBufferToSend.get(), &statusLine, strlen(statusLine));
    //add data.
    memcpy(myUniqueBufferToSend.get() + strlen(statusLine), myBufferedFile.get(), bufferSize);

    cout << "Sending response." << endl;
    int iResult = send(client, myUniqueBufferToSend.get(), totalSend, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n");
        closesocket(client);
    }

    cout << "Total bytes send: " << iResult << endl;

}
