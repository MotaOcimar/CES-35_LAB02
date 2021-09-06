#include <iostream>
#include <cstring>
#include <netdb.h>  // gethostbyname, socket, htons, hostent, sockaddr_in, PF_INET, AF_INET, SOCK_STREAM, IPPROTO_TCP
#include <unistd.h> // write, read

#define SERVER_PORT 8080
#define BUFSIZE 4096

/* Requests a file from the server program `server.cpp`
 * argv[1]: host name to connect
 * argv[2]: file name to request
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client host-name file-name"<<std::endl;
        exit(-1);
    }

    char *hostname = argv[1];
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        std::cerr << "gethostbyname failed to locate " << hostname;
        exit(-1);
    }

    int soc_file_descr = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (soc_file_descr < 0) {
        std::cerr << "socket call failed"<<std::endl;
        exit(-1);
    }

    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, host->h_addr, host->h_length);
    channel.sin_port = htons(SERVER_PORT);

    if (connect(soc_file_descr, (struct sockaddr *) &channel, sizeof(channel)) < 0) {
        std::cerr << "connect failed"<<std::endl;
        exit(-1);
    }

    char *filename = argv[2];
    write(soc_file_descr, filename, strlen(filename) + 1);
    int buffer[BUFSIZE];
    long bytes;
    while ((bytes = read(soc_file_descr, buffer, BUFSIZE)) > 0)
        write(1, buffer, bytes);

    return 0;
}
