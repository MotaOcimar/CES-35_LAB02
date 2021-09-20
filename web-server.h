#ifndef LAB02_WEB_SERVER_H
#define LAB02_WEB_SERVER_H

#include <unistd.h> // read, write, close
#include <netdb.h> // htons, htonl, listen, accept, sockaddr_in, INADDR_ANY, AF_INET, SOCK_STREAM, IPPROTO_TCP, SOL_SOCKET, SO_REUSEADDR
#include <sys/fcntl.h> // O_RDONLY
#include <sstream> // ss
#include <cstring>
#include <mutex>
#include <sys/stat.h>

#define BUF_SIZE 4096

std::mutex mutex_;

struct sockaddr_in createSockaddrByHostAndPort(hostent *host, int port) {
    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, host->h_addr, host->h_length);
    channel.sin_port = htons(port);
    return channel;
}

std::string getFilenameFromRequest(const char *request) {
    std::istringstream ss(request);
    std::string str;

    getline(ss, str, ' '); // "GET"
    getline(ss, str, ' '); // filename
    return str;
}

void removeEndSlash(char *path) {
    if (path[strlen(path) - 1] == '/')
        path[strlen(path) - 1] = '\0';
}

void listenConnections(int socket_fd, std::string server_directory, int thread_id) {
    // Always listening for connections:
    while (true) {
        // Accept a queued connection request and return a new socket descriptor
        int accepted_socket = accept(socket_fd, nullptr, nullptr);
        if (accepted_socket < 0) {
            std::string s = "HTTP/1.0 400 Bad Request\r\n";
            write(accepted_socket, s.c_str(), s.size());
            std::cerr << "accept failed" << std::endl;
        } else {
            // Read the request
            char request[BUF_SIZE];
            read(accepted_socket, request, BUF_SIZE);
            std::string file_location = server_directory + getFilenameFromRequest(request);

            // open and read the file
            mutex_.lock();
            struct stat path_stat{};
            stat(file_location.c_str(), &path_stat);
            if (!S_ISREG(path_stat.st_mode)){
                std::string status = "HTTP/1.0 404 Not Found\r\n";
                write(accepted_socket, status.c_str(), status.size());
                std::cerr << "open "<<file_location<<" failed" << std::endl;
            } else {
                int file = open(file_location.c_str(), O_RDONLY);
                if (file < 0) {
                    std::string status = "HTTP/1.0 404 Not Found\r\n";
                    write(accepted_socket, status.c_str(), status.size());
                    std::cerr << "open " << file_location << " failed" << std::endl;
                } else {
                    std::string aux_str = "HTTP/1.0 200 OK\r\n\r\n";
                    write(accepted_socket, aux_str.c_str(), aux_str.size());
                    long num_bytes;
                    char buffer[BUF_SIZE];
                    while ((num_bytes = read(file, buffer, BUF_SIZE)) > 0) {
                        // std::cout<<"Response by thread "<<thread_id<< std::endl;
                        // Sends the read num_bytes through the socket connection
                        write(accepted_socket, buffer, num_bytes);
                        // Also writes to stdout, if you want. Otherwise, comment the line below
                        // write(STDOUT_FILENO, buffer, num_bytes);
                    }
                    aux_str = "\r\n";
                    write(accepted_socket, aux_str.c_str(), aux_str.size());
                    close(file);
                }
            }
            mutex_.unlock();
            close(accepted_socket);
        }
    }
}

#endif //LAB02_WEB_SERVER_H
