#include <iostream>
#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <unistd.h>

#define SERVER_PORT 8080
#define BUFSIZE 4096

int main(int argc, char*argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: client server-name file-name";
        exit(-1);
    }
    struct hostent*h;
    if (!(h = gethostbyname(argv[1])))
    {
        std::cerr << "gethostbyname failed to locate " << argv[1];
        exit(-1);
    }
    struct sockaddr_in channel;
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    channel.sin_port = htons(SERVER_PORT);
    int s;
    if((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        std::cerr << "socket call failed";
        exit(-1);
    }
    if(connect(s, (struct sockaddr*)&channel, sizeof(channel)) < 0)
    {
       std::cerr << "connect failed";
       exit(-1);
    }
    write(s, argv[2], strlen(argv[2]) + 1);
    int buf[BUFSIZE], bytes;
    while ((bytes = read(s, buf, BUFSIZE)) > 0)
        write(1, buf, bytes);
    return 0;
}