#pragma once
#include <functional>
#include <map>
#include <string>
class Epoll;
class InetAddress;
class ThreadPool;
class Channel;

class Eventloop;

class Reactor
{
private:
    int sockfd;
    InetAddress* addr;
    std::string r_buf;
public:
    Reactor(/* args */);
    Reactor(int i);
    ~Reactor();
    int Read_buf(int fd);
    int Getfd();
    std::string Getbuf();
};





