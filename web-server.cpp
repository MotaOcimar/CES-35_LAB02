#include <iostream>
#include <cstring>
#include <unistd.h> // read, write, close
#include <netdb.h> // htons, htonl, listen, accept, sockaddr_in, INADDR_ANY, AF_INET, SOCK_STREAM, IPPROTO_TCP, SOL_SOCKET, SO_REUSEADDR
#include <sys/fcntl.h> // O_RDONLY

#define QUEUE_SIZE 10
#define BUF_SIZE 4096

/* Creates a server to receive HTTP requests for files
 * argv[1]: hostname
 * argv[2]: listening port for requests
 * argv[3]: directory with files to be served
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: web-server host port dir"<<std::endl;
        exit(-1);
    }

    // Get the host
    char *hostname = argv[1];
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        std::cerr << "gethostbyname failed to locate " << hostname;
        exit(-1);
    }

    // Create a socket:
    int soc_file_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (soc_file_descr < 0) {
        std::cerr << "socket call failed"<<std::endl;
        exit(-1);
    }

    // Get the port:
    char *pEnd;
    int port = (int) strtol(argv[2], &pEnd, 10);

    // Create a socket address
    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, host->h_addr, host->h_length);
    channel.sin_port = htons(port);

    // Binds the address to the socket:
    if (bind(soc_file_descr, (struct sockaddr *) &channel, sizeof(channel)) < 0) {
        std::cerr << "bind failed"<<std::endl;
        exit(-1);
    }

    // Prepare the socket to listen connections and limit the queue of incoming connections
    if (listen(soc_file_descr, QUEUE_SIZE) < 0) {
        std::cerr << "listen failed"<<std::endl;
        exit(-1);
    }

    // Always listening for connections:
    while (true) {
        // Accept a queued connection request and return a new socket descriptor
        int accepted_socket = accept(soc_file_descr, nullptr, nullptr);
        if (accepted_socket < 0) {
            std::cerr << "accept failed"<<std::endl;
            exit(-1);
        }

        // Read the requested file name to `filename`
        char file_name[BUF_SIZE];
        read(accepted_socket, file_name, BUF_SIZE);

        char *file_location = argv[3];
        strcat(file_location, "/");
        strcat(file_location, file_name);

        // open and read the file
        int file = open(file_location, O_RDONLY);
        if (file < 0)
            std::cerr << "open failed"<<std::endl;
        else {
            long bytes;
            while ((bytes = read(file, file_location, BUF_SIZE)) > 0){
                // send the read bytes through the socket connection
                write(accepted_socket, file_location, bytes);
            }
            close(file);
        }
        close(accepted_socket);
    }
}
