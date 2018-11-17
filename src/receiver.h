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

using namespace std;

typedef enum {GET, POST, OTHER} requestType;


/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd);
void receiveGETRequest(string req_str, int client);
void receivePOSTRequest(char *post_request, int request_size, int client_socketfd);
string parse_req(string p_toParse, int order_of_returned_str);
void openFileWithPathAndSend(string file_path, int client);
void sendFile(FILE* file, int client);
void receiveGETResponse(int client_socketfd, char * filename);

#endif //RECEIVER_H
