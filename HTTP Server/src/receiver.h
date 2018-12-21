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

#include "utils.h"

using namespace std;

typedef enum {GET, POST, OTHER} requestType;


/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd);

#endif //RECEIVER_H
