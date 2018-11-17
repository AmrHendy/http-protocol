//
// Created by Amr Hendy on 11/16/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#ifndef CLIENT_H
#define CLIENT_H
#define DEFAULT_PORT_NUMBER 80;

void startClient();

typedef struct command_struct{
    int type;
    char file_name[100];
    int ip_number;
    int port_number = DEFAULT_PORT_NUMBER;
};

const char *commands_file = "./commands_file.txt";
int port_number;
int ip_number;

void establishConnection();

char *readCommand();

void startClient();

#endif //CLIENT_H
