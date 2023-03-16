#pragma once
#include <functional>
#include <vector>
#include <string>

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
    Reactor *mainReactor;
    std::vector<Reactor*> subReactors;
    std::function<void( int fd, std::string)> Unpacking;

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
    void set_unpack( std::function< void( int fd, std::string)> cb);
    //void key_init();
    //int Read_buf(int fd);
};
