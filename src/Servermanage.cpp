#include "Servermanage.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include "Epoll.h"
#include "InetAddress.h"
#include "ThreadPool.h"
#include "Reactor.h"

#define READ_BUFFER 1024
//#define key_PATH1 "/dev/input/event1"
//using namespace std;

Servermanage::Servermanage():ep(nullptr), quit( false), threadpool(nullptr), connects(0), channel(nullptr){
    mainReactor = new Reactor;
    ep = new Epoll;
    std::function<void( int, int)> cb = [this](int ,int){
        this->newconnect();
    };
    //std::bind(&Servermanage::newconnect, this, std::placeholders::_1, std::placeholders::_2);
    //newfd->set(sockfd, cb);
    //Channel *main_fd = new Channel(mainReactor->Getfd(), cb);
    ep->UpdateChannel( mainReactor->Getfd(), cb);    

    int size = std::thread::hardware_concurrency();
    threadpool = new ThreadPool(size);
    for(int i = 0; i < size; ++i){
        subReactors.push_back(new Reactor(1));
    }
    //stdin_init();
}

Servermanage::~Servermanage(){
    delete mainReactor;
    delete threadpool;
    delete channel;
    for(int i = 0; i < subReactors.size(); ++i){
        delete subReactors[i];
    }
    delete ep;
}

void Servermanage::loop(){
    while (!quit)
    {
        int nfds = ep->poll();
        for( int i = 0; i< nfds; i++){
            channel = (Channel*)ep->events[i].data.ptr;
            channel->callback(channel->events, channel->fd);
            //threadpool->add(channel->callback);
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
    ep->UpdateChannel(clnt_sockfd, cb);
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
            //int a =subReactors[random]->Read_buf(fd);
            connects -= subReactors[random]->Read_buf(fd);
            std::cout<< "connects:"<< connects<< std::endl;           
            Unpacking(fd, subReactors[random]->Getbuf());
            if(connects == 0){
                pid_t pid = getpid();
                kill(pid, SIGINT);
            }
        };//使用lambda设置回调函数
        //std::bind(&(subReactors[random]->Read_buf), this, fd);//设置回调读函数            
        threadpool->add(cb, fd);//加入任务队列
    }
}

//void Servermanage::stdin_init( ){
//    std::function<void(int, int )> cb = [this](int, int fd){
//        std::cout<< 1 << std::endl; 
//        char buf[1024];
//        bzero(buf, sizeof(buf) );        
//        int ret = read(fd, buf, sizeof(buf));
//        std::cout<< ret << " "<< sizeof(buf)<< std::endl;        
//        if( !strcmp(buf,"exit\0")){
//            exit(-1);
//            std::cout<< buf <<std::endl; 
//        }
//    };
//    Channel *in_chl = new Channel(STDIN_FILENO, cb);
//    ep->UpdateChannel( in_chl);
//}

void Servermanage::set_unpack( std::function< void(int, std::string)> cb = []( int fd, std::string buffer){
    std::cout<< buffer<< std::endl;
    write( fd, buffer.c_str(), buffer.size()); 
    })
{
    Unpacking = cb;
}