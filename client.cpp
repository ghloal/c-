#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "InetAddress.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

//using namespace std;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd == -1)
    {
        perror( " socket creat error");
        exit(EXIT_FAILURE);
    }


    InetAddress* addr = new InetAddress("127.0.0.1", 8888);
    
    //bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); 客户端不进行bind操作

    if(connect(sockfd, (sockaddr*)&addr->addr, addr->addr_len) == -1)
    {
        perror(" connect failure");
        exit(-1);
    }
    
    while( true)
    {
        char buf[1024];
        bzero(buf, sizeof(buf) );
        //string sin;
        std::cin >>buf;
        //char* buf = sin.data();
        if( write( sockfd, buf, sizeof(buf) ) == -1)
        {
            std::cout << "socket already disconnected, can't write any more!" << std::endl;
            break;
        }
        if( !strcmp(buf,"exit"))
        {
            break;
        }
        bzero( buf, sizeof(buf));
        switch (read( sockfd, buf, sizeof(buf)) )
        {
        case -1/* constant-expression */:
            /* co(de */
            close( sockfd);
            std::cout << "socket read error" << std::endl; 
            break;
        case 0:
            std::cout << "server socket disconnected!" << std::endl;
            break;
        default:
            std::cout << buf<< std::endl;
            break;
        }

    }
    delete addr;
    close( sockfd);
    return 0;
}