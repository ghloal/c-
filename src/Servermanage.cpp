#include "Servermanage.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Epoll.h"
#include "InetAddress.h"
#include "ThreadPool.h"
#include "Reactor.h"

#define READ_BUFFER 1024
//using namespace std;

Servermanage::Servermanage():ep(nullptr), quit( false), threadpool(nullptr), connects(0), channel(nullptr){
    mainReactor = new Reactor;
    ep = new Epoll;
    std::function<void( int, int)> cb = [this](int ,int){
        this->newconnect();
    };
    //std::bind(&Servermanage::newconnect, this, std::placeholders::_1, std::placeholders::_2);
//  newfd->set(sockfd, cb);
    Channel *main_fd = new Channel(mainReactor->Getfd(), cb);
    ep->UpdateChannel( main_fd);    

    int size = std::thread::hardware_concurrency();
    threadpool = new ThreadPool(size);
    for(int i = 0; i < size; ++i){
        subReactors.push_back(new Reactor(1));
    }
}

Servermanage::~Servermanage(){
    delete ep;
    delete mainReactor;
    delete threadpool;
    delete channel;
    for(int i = 0; i < subReactors.size(); ++i){
        delete subReactors[i];
    }
}

void Servermanage::loop(){
    while (!quit)
    {
        //if(connects == 0)
        //{
        //    quit = true;
        //    for(auto it = ep->chs.begin(); it != ep->chs.end(); ++it){
        //        delete (*it);
        //    }
        //}//原先在epoll_wait后面，connects为0也不会运行到；
        //现在在上面，考虑初入问题
        int nfds = ep->poll();
        for( int i = 0; i< nfds; i++){
            channel = (Channel*)ep->events[i].data.ptr;
            channel->callback(channel->events, channel->fd);
            //threadpool->add(channel->callback);
            std::cout<< "connects:"<< connects<< std::endl;
        }
    }
}

void Servermanage::newconnect(/*int events, int fd*/){
    struct sockaddr_in clnt_addr;
    bzero(&clnt_addr, sizeof(clnt_addr));
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    int clnt_sockfd = accept(mainReactor->Getfd(), (sockaddr*)&clnt_addr, &clnt_addr_len);//创建通信套接字
    connects +=1;
    if( clnt_sockfd == -1){
        perror( "accept error");
        exit(-1);
    }
    std::cout<<"new client fd "<< clnt_sockfd<<"! IP: "<< inet_ntoa(clnt_addr.sin_addr)<<"Port: "<<ntohs(clnt_addr.sin_port)<<std::endl;
    std::function<void( int, int)> cb = [this](int events, int fd){this->pro_read(events, fd);};
    //std::bind(&Servermanage::pro_read, this, std::placeholders::_1, std::placeholders::_2);
    Channel *newfd = new Channel(clnt_sockfd, cb);
    ep->UpdateChannel( newfd);
}

void Servermanage::pro_read(int events, int fd){
    if( events & (EPOLLHUP| EPOLLERR| EPOLLRDHUP)){
        close(fd);
        std::cout << "EPOLLHUP| EPOLLERR| EPOLLRDHUP"<< std::endl;
        connects--;
    }
    else if( events & EPOLLIN){
        std::function<void( int)> cb =[this](int fd){ 
            int random = fd%subReactors.size();
            connects-=subReactors[random]->Read_buf(fd);
        };//使用lambda设置回调函数
        //std::bind(&(subReactors[random]->Read_buf), this, fd);//设置回调读函数            
        threadpool->add(cb, fd);//加入任务队列
    }
}