#pragma once
#include <functional>
#include <sys/epoll.h>
class EventLoop;

class Channel
{
public:
    int fd;
    std::function<void( int, int)> callback;
    int events;
    bool inepoll;
public:
    Channel();
    Channel(int fd, std::function<void( int, int)>, uint32_t op = (EPOLLIN | EPOLLET));
    void set(int i, std::function<void( int, int)>, uint32_t op = (EPOLLIN | EPOLLET));//暂时无用
    //void run_task(int i);
    //void setes(int es);
    ~Channel();
};



