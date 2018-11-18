//
// Created by Amr Hendy on 11/16/2018.
//

#ifndef RECEIVER_H
#define RECEIVER_H

#include <regex>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>

using namespace std;

typedef enum {GET, POST, OTHER} requestType;


/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd);
void receiveGETRequest(int client);
string parse_req(string p_toParse, int order_of_returned_str);
void openFileWithPathAndSend(string file_path, int client);;
void sendFile(FILE* file, int client, char status_line[]);
void parse_data_from_file(char * buffer, int buffer_size, int &data_start_position, int &data_content_length);
void receivePOSTRequest(int client_socketfd);
int sendBufferToSocket(char *buffer, int buffer_size, int socketfd);
int getFileLength(FILE * fp);

#endif //RECEIVER_H
