#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/fcntl.h>

#define SERVER_PORT 8080
#define QUEUE_SIZE 10
#define BUF_SIZE 4096

int main()
{
    struct sockaddr_in channel;
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT);
    int on = 1, s;
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        std::cerr << "socket call failed";
        exit(-1);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    if (bind(s, (struct sockaddr*)&channel, sizeof(channel)) < 0)
    {
        std::cerr << "bind failed";
        exit(-1);
    }
    if (listen(s, QUEUE_SIZE) < 0)
    {
        std::cerr << "listen failed";
        exit(-1);
    }
    while (true)
    {
        char buf[BUF_SIZE];
        int sa;
        if ((sa = accept(s, 0, 0)) < 0)
        {
            std::cerr << "accept failed";
            exit(-1);
        }
        read(sa, buf, BUF_SIZE);
        int fd;
        if ((fd = open(buf, O_RDONLY)) < 0)
            std::cerr << "open failed";
        int bytes;
        while ((bytes = read(fd, buf, BUF_SIZE)) > 0)
            write(sa, buf, bytes);
        close(fd);
        close(sa);
    }
}