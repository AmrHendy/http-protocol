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

#endif //RECEIVER_H
