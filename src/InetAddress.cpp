#include "InetAddress.h"

InetAddress::InetAddress( const char *ip = "127.0.0.1", int port = 8888) : addr_len(sizeof( addr)){
    bzero( &addr, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr( ip);
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress()
{
}