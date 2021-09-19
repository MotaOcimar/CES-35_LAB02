#include <iostream>
#include <cstring>
#include "web-server.h"

#define QUEUE_SIZE 10
#define BUF_SIZE 4096

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

    // Always listening for connections:
    int response;
    while (true) {
        // Accept a queued connection request and return a new socket descriptor
        int accepted_socket = accept(socket_fd, nullptr, nullptr);
        if (accepted_socket < 0) {
            std::cerr << "accept failed" << std::endl;
            response = 400;
        } else {
            // Read the request
            char request[BUF_SIZE];
            read(accepted_socket, request, BUF_SIZE);

            char *URI = getRequestURI(request);

            char *file_location = argv[3];
            removeEndSlash(file_location);
            strcat(file_location, URI);

            // open and read the file
            int file = open(file_location, O_RDONLY);
            if (file < 0) {
                std::cerr << "open failed" << std::endl;
                response = 404;
            } else {
                response = 200;
                long num_bytes;
                while ((num_bytes = read(file, file_location, BUF_SIZE)) > 0) {
                    // Sends the read num_bytes through the socket connection
                    write(accepted_socket, file_location, num_bytes);
                    // Also writes to stdout
                    write(STDOUT_FILENO, file_location, num_bytes);
                }
                close(file);
            }

            close(accepted_socket);
        }
    }
}
