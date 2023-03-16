#include "Epoll.h"
#include <iostream>

#define MAXEVENTS 1024

Epoll::Epoll(){
    epfd = epoll_create1( 0);
    if( epfd == -1){
        perror(" epfd error");
        exit(-1);
    }
    bzero( &events, sizeof(*events)*MAX_EVENTS);
}

Epoll::~Epoll(){
    for(int i= 0; i < chs.size(); i++){
        std::cout << chs[i] ->fd << "fd" << std::endl;
        std::cout << "chs.size():" << chs.size()<< " i: " << i << std::endl;
        if(chs[i] != nullptr){
            delete chs[ i];            
        }
    }
    if( epfd != -1){
        close( epfd);
        epfd = -1;
    }
}

//该函数也不用
//void Epoll::AddFd( int fd, std::function<void( int, int)> _cb, uint32_t op){
//    struct epoll_event ev;
//    bzero( &ev, sizeof(ev));
//    ev.events = op;
//    Channel* newfd;
//    newfd->set(fd,_cb);
//    ev.data.ptr = newfd;
//    if( op & EPOLLET){
//        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
//    }
//    epoll_ctl( epfd, EPOLL_CTL_ADD, fd, &ev);
//}

void Epoll::UpdateChannel( int sockfd, std::function<void( int, int)> cb){
    struct epoll_event ev;
    bzero( &ev, sizeof(ev));
    Channel *ch = new Channel(sockfd, cb);
    ev.events = ch->events;
    ev.data.ptr = ch;
    chs.push_back(ch);
    std::cout<< "ch->fd" << ch ->fd<< std::endl; 
    if( ev.events & EPOLLET){
        fcntl(ch->fd, F_SETFL, fcntl(ch->fd, F_GETFL) | O_NONBLOCK);
    }
    if (!ch->inepoll){
        epoll_ctl( epfd, EPOLL_CTL_ADD, ch->fd, &ev);
        ch->inepoll = true;
    }
    else
    {
        epoll_ctl( epfd, EPOLL_CTL_MOD, ch->fd, &ev);
    }
    
}

int Epoll::poll( ){
    int nfds = epoll_wait( epfd, events, MAXEVENTS, -1);
    if( nfds == -1){
        perror( "epwait error");
    }
    return nfds;
}