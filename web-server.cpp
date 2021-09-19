#include <iostream>
#include <cstring>
#include "web-server.h"

#define QUEUE_SIZE 10

/* Creates a server to receive HTTP requests for files
 * argv[1]: hostname
 * argv[2]: listening port for requests
 * argv[3]: directory with files to be served
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: web-server host port dir" << std::endl;
        exit(-1);
    }

    char *hostname = argv[1];
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        std::cerr << "gethostbyname failed to locate " << hostname;
        exit(-1);
    }

    // Create a socket:
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        std::cerr << "socket call failed" << std::endl;
        exit(-1);
    }

    // Get the port:
    char *pEnd;
    int port = (int) strtol(argv[2], &pEnd, 10);

    // Create a socket address
    sockaddr_in channel = createSockaddrByHostAndPort(host, port);

    // Binds the address to the socket:
    if (bind(socket_fd, (struct sockaddr *) &channel, sizeof(channel)) < 0) {
        std::cerr << "bind failed" << std::endl;
        exit(-1);
    }

    // Prepare the socket to listen connections and limit the queue of incoming connections
    if (listen(socket_fd, QUEUE_SIZE) < 0) {
        std::cerr << "listen failed" << std::endl;
        exit(-1);
    }

    char *file_location = argv[3];
    removeEndSlash(file_location);

    listenConnections(socket_fd, file_location);

}
