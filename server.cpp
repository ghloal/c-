#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "Epoll.h"
#include "InetAddress.h"
#include "Servermanage.h"


#define READ_BUFFER 1024
using namespace std;

int main() {
    Servermanage server;//改成servermanage类
    server.loop();
    
    return 0;
}
