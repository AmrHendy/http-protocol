//
// Created by Amr Hindi on 11/16/2018.
//

#include "client.h"

// Steps:
// 1) establish connection with server
// 2) read the commands from the file
// 3) send the request using Sender
// 4) get the response using Receiver
// 5) execute action
FILE *fp;
bool file = false;
int sock;

void establishConnection() {
    char *line = readCommand();
    command_struct command = parse(line);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = command.port_number;

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, command.ip_number, &serv_addr.sin_addr) <= 0) {
        perror("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("\nConnection Failed \n");
        return -1;
    }
}

char *readCommand() {
    if (fp == NULL && !file) {
        fp = fopen(file_name, "r");
        file = true;
    }
    if (fp == NULL) {
        perror("Error while opening the file.\n");
        file = false;
        exit(EXIT_FAILURE);
    }
    char command[100];
    int index = 0;
    char c = fgetc(fp);
    while (c != EOF || c != '\n') {
        command[index++] = c;
        c = fgetc(fp);
    }
    command[index] = '\0';
    if (c == EOF) {
        file = false;
        fclose(fp);
    }
    return command;
}

void startClient(int port_number, int ip_number) {
    setIPNumber(ip_number);
    setPortNumber(port_number);
}

void setPortNumber(int num) {
    port_number = num;
}

void setIPNumber(int num) {
    ip_number = num;
}

command_struct parse(char *command) {
    command_struct result;
    int index = 0;
    int i = 0;
    char temp[100];
    while (command[index++] != ' ') {
        temp[i++] = command[index - 1];
    }
    temp[i] = '\0';
    if (strcmp(temp, "GET")) {
        result.type = 0;
    } else {
        result.type = 1;
    }
    i = 0;
    while (command[index++] != ' ') {
        result.file_name[i++] = command[index - 1];
    }
    result.file_name[i] = '\0';
    i = 0;
    while (command[index] != ' ' || command[index] != '\0') {
        temp[i++] = command[index++];
    }
    temp[i] = '\0';
    i = 0;
    result.ip_number = atoi(temp);
    if (command[index] == '\0') {
        return result;
    }
    i = 0;
    while (command[index] != '\0') {
        temp[i++] = command[index++];
    }
    temp[i] = '\0';
    i = 0;
    result.port_number = atoi(temp);
    return result;
}
