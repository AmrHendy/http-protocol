//
// Created by Amr Hendy on 11/16/2018.
//

#ifndef SENDER_H
#define SENDER_H


void sendRequest(requestType type, char* hostname, char * port_number, char * file_url, int client_socketfd);

#endif //SENDER_H
