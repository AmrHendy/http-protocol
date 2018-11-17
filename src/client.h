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

typedef enum {GET, POST} requestType;
typedef struct command_struct{
    requestType type;
    char file_name[100];
    char ip_number[100];
    int port_number = DEFAULT_PORT_NUMBER;
};

const char *commands_file = "./commands_file.txt";
int port_number;
char* ip_number;

void establishConnection();

char *readCommand();

void startClient(char* ip_number, int port_number);

#endif //CLIENT_H
