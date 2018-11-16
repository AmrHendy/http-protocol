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
 * A function to get file name from GET request
 */
string getFilePath(string p_toParse);

/*
 * open the required file then send it.
 */
void openFileWithPathAndSend(string file_path, int client);

/*
 * sending file to client
 */

void sendFile(FILE* file, int client);

#endif //RECEIVER_H
