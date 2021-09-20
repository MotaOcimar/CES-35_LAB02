#include <iostream>
#include <string>
#include <cstring>
#include <netdb.h>  // gethostbyname, socket, htons, hostent, sockaddr_in, PF_INET, AF_INET, SOCK_STREAM, IPPROTO_TCP
#include <unistd.h> // write, read
#include <sstream>
#include <fstream>

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
        return -1;
    }

    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        std::cerr << "socket call failed" << std::endl;
        return -1;
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
        return -1;
    }

    std::string filename = s.substr(pos);
    std::string request = "GET " + filename + " HTTP/1.1\r\nHost: " + hostname + "\r\nAccept: text/html\r\n\r\n";
    write(socket_fd, request.c_str(), request.length());
    send(socket_fd, request.c_str(), request.length(), 0);
    std::stringstream ss;
    int buffer[BUFSIZE];
    long bytes;
    while ((bytes = read(socket_fd, buffer, BUFSIZE)) > 0) {
        // Only if you want terminal printout. Otherwise, comment the line below
        // write(STDOUT_FILENO, buffer, bytes);
        ss.write(reinterpret_cast<const char *>(buffer), bytes);
    }
    s = ss.str();
    pos = s.find("200 OK");
    if (pos != std::string::npos) {
        std::cout << "200 OK" << std::endl;
        std::ofstream fout(filename.substr(filename.find_last_of('/') + 1, filename.length()));
        fout << s.substr(s.find("\r\n\r\n") + 4, s.length());
        fout.close();
        return 0;
    }
    pos = s.find("400 Bad Request");
    if (pos != std::string::npos) {
        std::cout << "400 Bad Request" << std::endl;
        return 0;
    }
    pos = s.find("404 Not Found");
    if (pos != std::string::npos) {
        std::cout << "404 Not Found" << std::endl;
        return 0;
    }
    return -1;
}
