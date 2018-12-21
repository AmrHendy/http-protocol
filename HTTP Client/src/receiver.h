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
#include <sys/time.h>

#include "utils.h"

using namespace std;

typedef enum {GET, POST, OTHER} requestType;

void receiveGETResponse(int client_socketfd, char * filename);

#endif //RECEIVER_H
