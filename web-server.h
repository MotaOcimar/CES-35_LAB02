#ifndef LAB02_WEB_SERVER_H
#define LAB02_WEB_SERVER_H

#include <unistd.h> // read, write, close
#include <netdb.h> // htons, htonl, listen, accept, sockaddr_in, INADDR_ANY, AF_INET, SOCK_STREAM, IPPROTO_TCP, SOL_SOCKET, SO_REUSEADDR
#include <sys/fcntl.h> // O_RDONLY
#include<sstream> // ss

struct sockaddr_in createSockaddrByHostAndPort(hostent *host, int port){
    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, host->h_addr, host->h_length);
    channel.sin_port = htons(port);
    return channel;
}

char *getRequestURI(const char *request){
    std::istringstream ss(request);
    std::string str;

    getline(ss, str, ' '); // "GET"
    getline(ss, str, ' '); // URI
    int URI_len = (int) str.length();
    char *URI = (char*) malloc(URI_len + 1);
    strcpy(URI, str.c_str());
    return URI;
}

void removeEndSlash(char *path){
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
}

#endif //LAB02_WEB_SERVER_H
