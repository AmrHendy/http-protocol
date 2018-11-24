//
// Created by Amr Hendy on 11/17/2018.
//

#include <bits/stdc++.h>
#include "server.h"

using namespace std;

int main(int argc, char** argv){
    if(argc != 2){
        printf("Invalid number of arguments\n");
        return;
    }
    printf("Starting Server, Server is waiting now for client connections\n");
    start_server(atoi(argv[1]));
}

