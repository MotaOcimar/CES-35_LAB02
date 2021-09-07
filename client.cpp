#include <iostream>
#include <cstring>
#include <netdb.h>  // gethostbyname, socket, htons, hostent, sockaddr_in, PF_INET, AF_INET, SOCK_STREAM, IPPROTO_TCP
#include <unistd.h> // write, read
#include <sstream>
#include <string>

#define BUFSIZE 4096

/* Requests a file from the server program `server.cpp`
 * argv[1]: host name to connect
 * argv[2]: file name to request
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: client http://[host-name]:[port]/[file-name]" << std::endl;
        return -1;
    }

    std::string s = argv[1];
    if (s.substr(0, 7) == "http://")
        s.erase(0, 7);

    int pos = s.find_first_of(':');
    if (pos == std::string::npos) {
        std::cerr << "Invalid URL" << std::endl << "Please enter http://[host-name]:[port]/[file-name]" << std::endl;
        return -1;
    }
    std::string hostname = s.substr(0, pos);
    struct hostent *host = gethostbyname(hostname.c_str());
    if (!host) {
        std::cerr << "gethostbyname failed to locate " << hostname << std::endl;
        exit(-1);
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        std::cerr << "socket call failed" << std::endl;
        exit(-1);
    }

    struct sockaddr_in channel{};
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, host->h_addr, host->h_length);
    s.erase(0, pos + 1);
    pos = s.find_first_of('/');
    int port = std::stoi(s.substr(0, pos));
    channel.sin_port = htons(port);
    if (connect(socket_fd, (struct sockaddr *) &channel, sizeof(channel)) < 0) {
        std::cerr << "connect failed" << std::endl;
        exit(-1);
    }

    std::string filename = s.substr(pos);
    std::stringstream ss;
    std::string request = "GET " + filename + " HTTP/1.1\r\nHost: " + hostname + "\r\nAccept: text/html\r\n\r\n";
    write(socket_fd, request.c_str(), request.length());
    send(socket_fd, request.c_str(), request.length(), 0);
    int buffer[BUFSIZE];
    long bytes;
    while ((bytes = read(socket_fd, buffer, BUFSIZE)) > 0)
        write(STDOUT_FILENO, buffer, bytes);
    return 0;
}
