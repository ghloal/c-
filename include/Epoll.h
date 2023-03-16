#pragma once
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "Channel.h"
#define MAX_EVENTS 1024


class Epoll{
    public:
        int epfd;//epoll树
        struct epoll_event events[ MAX_EVENTS];//epoll
        std::vector<Channel*> chs;
        
        Epoll();
        ~Epoll();

        void AddFd( int fd, std::function<void( int, int)>, uint32_t op = (EPOLLIN | EPOLLET));
        void UpdateChannel( int sockfd, std::function<void( int, int)> cb);
        int poll();//返回的epoll列表
};

