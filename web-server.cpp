#include <iostream>
#include <thread>
#include "web-server.h"

#define QUEUE_SIZE 10
#define NUM_THREAD 4

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

    // Allows the socket to be bound to the same address after main exits:
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0){
        std::cerr << "set socket option failed"<<std::endl;
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

    char *server_directory = argv[3];
    removeEndSlash(server_directory);
    std::cout<<"Serving files on \""<<server_directory<<"\""<<std::endl;

    // Create threads
    std::thread threads[NUM_THREAD];

    // Execute threads
    for (int i = 0; i < NUM_THREAD; ++i) {
        threads[i] = std::thread(listenConnections, socket_fd, server_directory, i);
    }

    // synchronize threads
    for (auto & thread_ : threads) {
        thread_.join();
    }

    close(socket_fd);
    return 0;
}
