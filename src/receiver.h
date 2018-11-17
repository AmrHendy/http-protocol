//
// Created by Amr Hendy on 11/16/2018.
//

#ifndef RECEIVER_H
#define RECEIVER_H

/*
 * Client call this function to receive response from the serevr.
 */
void receiveResponse();

/*
 * Server call this function to respond to any request from client.
 */
void receiveRequest(char *request, int request_size, int client_socketfd);

/*
 * Server call this function to respond to GET request from client.
 */
void receiveGETRequest(char *get_request, int request_size, int client_socketfd);

/*
 * Server call this function to respond to POST request from client.
 */
void receivePOSTRequest(char *get_request, int request_size, int client_socketfd);

#endif //RECEIVER_H
