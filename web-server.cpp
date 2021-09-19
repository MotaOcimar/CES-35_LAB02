#include <iostream>
#include <cstring>
#include <unistd.h> // read, write, close
#include <netdb.h> // htons, htonl, listen, accept, sockaddr_in, INADDR_ANY, AF_INET, SOCK_STREAM, IPPROTO_TCP, SOL_SOCKET, SO_REUSEADDR
#include <sys/fcntl.h> // O_RDONLY

#define SERVER_PORT 8080
#define QUEUE_SIZE 10
#define BUF_SIZE 4096

int main() {
    int soc_file_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (soc_file_descr < 0) {
        std::cerr << "socket call failed"<<std::endl;
        exit(-1);
    }

    // Allows the socket to be bound to an address that is already in use:
    bool opt = true;
    if (setsockopt(soc_file_descr, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        std::cerr << "set socket option failed"<<std::endl;
    }

    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT);

    if (bind(soc_file_descr, (struct sockaddr *) &channel, sizeof(channel)) < 0) {
        std::cerr << "bind failed"<<std::endl;
        exit(-1);
    }
    if (listen(soc_file_descr, QUEUE_SIZE) < 0) {
        std::cerr << "listen failed"<<std::endl;
        exit(-1);
    }

    while (true) {
        int accepted_socket = accept(soc_file_descr, nullptr, nullptr);
        if (accepted_socket < 0) {
            std::cerr << "accept failed"<<std::endl;
            exit(-1);
        }

        char buffer[BUF_SIZE];
        read(accepted_socket, buffer, BUF_SIZE);

        int file_descriptor = open(buffer, O_RDONLY);
        if (file_descriptor < 0)
            std::cerr << "open failed"<<std::endl;
        else {
            long bytes;
            while ((bytes = read(file_descriptor, buffer, BUF_SIZE)) > 0)
                write(accepted_socket, buffer, bytes);
            close(file_descriptor);
        }
        close(accepted_socket);
    }
}
