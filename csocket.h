#ifndef CSOCKET_H
#define CSOCKET_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>

class cSocket
{
    bool fOpen;
    int sock;
    std::string path;
    struct sockaddr_in addr;
    struct sockaddr_in from;
    int send(const std::string &buf);
    std::string shortName() const;
public:
    ~cSocket();
    cSocket(const std::string &ip,
            const int port,
            const std::string &path,
            const bool fServ=true);
    void listen();
    void sendout();

};

#endif // CSOCKET_H
