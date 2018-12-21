//
// Created by Amr Hendy on 11/16/2018.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "sender.h"
#include "receiver.h"

#define DEFAULT_PORT_NUMBER "80";

typedef struct command_struct{
    requestType type;
    char* file_name;
    char* ip_number;
    char* port_number = DEFAULT_PORT_NUMBER;
};

void startClient(char* ip_number, char* port_number, char* commands_file);

#endif //CLIENT_H
