#include "Reactor.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "InetAddress.h"

Reactor::Reactor(/* args */){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    addr = new InetAddress("127.0.0.1", 8888);

    bind(sockfd, (sockaddr*)&addr->addr, addr->addr_len);

    listen(sockfd, SOMAXCONN);
}

Reactor::Reactor(int sub):sockfd(0), addr(nullptr){
}

Reactor::~Reactor(){
    delete addr;
}

int Reactor::Read_buf( int fd){
    int disconn = 0;
    char buf[ 1024];
    r_buf.clear();
    while( true){
        bzero( buf, sizeof( buf));
        int bits = read( fd, buf, sizeof(buf));
        if( bits == 0){//客户端断开连接
            std::cout<<"client fd"<< fd <<"disconnected"<<std::endl;
            close(fd);
            disconn++;
            std::cout<< "Disconnection" << std::endl;
            break;  
        }
        else if( bits == -1){
            if( (errno == EAGAIN) || (errno == EWOULDBLOCK)){
                //数据已经读完
                std::cout << "read success" << std::endl;
                write( fd, r_buf.c_str(), r_buf.size());                
                break;
            }
            else if(errno == EINTR){
                //客户端正常中断，继续读取
                continue;
            }
        }
        if( !strcmp(buf,"exit")){
            close(fd);
            disconn++;
            std::cout<< "Disconnection" << std::endl;
            break; 
        }
        else{
            r_buf = r_buf+buf;
        }
    }
    return disconn;
}

//提供私有成员访问接口
int Reactor::Getfd(){
    return sockfd;
}