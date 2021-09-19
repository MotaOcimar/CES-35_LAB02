# CES-35 LAB2

## Web server

### Building
To build web-server, please tap:
```sh
g++ -pthread web-server.cpp -o web-server
```

### Usage
Here is the usage of web-server:
```sh
./web-server [hostname] [port] [server_directory]
```
For example, you can refer to your own computer as a server using `localhost` as the hostname, listening on port `8080` and serving files on `/tmp` directory:
```sh
./web-server localhost 8080 /tmp
```

## Web client

### Building
To build web-client, please tap:
```sh
g++ -o web-server web-client.cpp
```  

### Usage
Here is the usage of web-client:
```sh
./web-client http://[hostname]:[port]/[file_name]
```

For example, you can have an HTTP GET by:
```sh
./web-client gaia.cs.umass.edu:80/wireshark-labs/HTTP-wireshark-file1.html
```
