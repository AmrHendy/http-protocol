//
// Created by Amr Hendy on 11/17/2018.
//

#include <bits/stdc++.h>
#include "client.h"

using namespace std;

int main(int argc, char** argv){
    if(argc != 3){
        printf("Invalid number of arguments\n");
        return;
    }
    char* ip_number = argv[1];
    char* port_number = argv[2];
    startClient(ip_number, port_number, "commands_file.txt");
}

