#pragma once
#include <arpa/inet.h>
#include <string.h>

class InetAddress
{
private:

public:
    socklen_t addr_len;
    struct sockaddr_in addr;
    InetAddress(const char *ip, int port);
    ~InetAddress();
};

