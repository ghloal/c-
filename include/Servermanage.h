#pragma once
#include <functional>
#include <vector>

class Epoll;
class InetAddress;
class ThreadPool;
class Channel;
class Reactor;

class Servermanage
{
private:
    Epoll *ep;
    bool quit;
    ThreadPool *threadpool;
    Channel *channel;
    Reactor* mainReactor;
    std::vector<Reactor*> subReactors;
    //int sockfd;
    //InetAddress* addr;
    //std::string r_buf;
    int connects;
public:
    Servermanage();
    ~Servermanage();
    void loop();
    void pro_read(int events, int fd);
    void newconnect(/*int events, int fd*/);
    //int Read_buf(int fd);
};
