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
int client_socketfd;

// return status of connection
int establishConnection(char * client_ipaddress, int client_port_number) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\n Socket creation error \n");
        return 0;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(client_port_number);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, client_ipaddress, &serv_addr.sin_addr) <= 0) {
        perror("\nInvalid address / Address not supported \n");
        return 0;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("\nConnection Failed \n");
        return 0;
    }
    client_socketfd = sockaddr_in;
    return 1;
}

void closeconnection(){
    close(client_socketfd);
}

char *readCommand() {
    if(fp == NULL) return "";
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


void startClient(char* client_hostname, char* client_portnumber) {
    setIPNumber(client_hostname);
    setPortNumber(client_portnumber);
    // open the file in start Client once.
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    while(1){
        char * command_line = readCommand();
        command_struct command = parse(command_line);
        printf("Trying to connect to: %s on port %d requesting url %s\n",command.ip_number, command.port_number, command.file_name);
        int status = establishConnection(client_hostname, atoi(client_portnumber));
        if(status == 0){
            perror("Failed to connect to the server");
            exit(EXIT_FAILURE);
        }
        printf("Connected Successfully to the server\n");
        // handling the request using sender
        sendRequest(command.type, command.ip_number, command.port_number, command.file_name, client_socketfd);
        closeconnection();
    }
}

void setPortNumber(int portnumber) {
    client_port_number = portnumber;
}

void setIPNumber(char* hostname) {
    client_ip = hostname;
}

command_struct parse(char *command) {
    command_struct result;
    int index = 0;
    int curr_index = 0;
    char temp[100];
    memset(temp, '\0', sizeof(temp));
    // extract request type
    while(command[index] != ' ') {
        temp[curr_index++] = command[index++];
    }
    index++;
    temp[curr_index] = '\0';
    if (strcmp(temp, "GET") == 0) {
        result.type = GET;
    }else if (strcmp(temp, "POST") == 0){
        result.type = POST
    }
    else{
        result.type = OTHER;
    }

    // extract file url
    curr_index = 0;
    memset(temp, '\0', sizeof(temp));
    while (command[index] != ' ') {
        temp[curr_index++] = command[index++];
    }
    index++;
    temp[curr_index] = '\0';
    result.file_name = temp;

    // extract host ip
    memset(temp, '\0', sizeof(temp));
    curr_index = 0;
    while (command[index] != ' ' || command[index] != '\0') {
        temp[curr_index++] = command[index++];
    }
    temp[curr_index] = '\0';
    curr_index = 0;
    result.ip_number = temp;

    // extract the port number if exists
    if (command[index] == '\0') {
        return result;
    }
    memset(temp, '\0', sizeof(temp));
    curr_index = 0;
    index++;
    while (command[index] != '\0') {
        temp[curr_index++] = command[index];
    }
    temp[curr_index] = '\0';
    result.port_number = temp;
    return result;
}
