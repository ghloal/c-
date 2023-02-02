#include "Channel.h"

Channel::Channel():fd(-1),inepoll(false){

}

Channel::Channel(int i, std::function<void( int, int)> _cb, uint32_t op):fd(i), callback(_cb), inepoll(false), events(op){
}
//该函数暂时不用
void Channel::set(int i, std::function<void( int, int)> _cb, uint32_t op){
    fd = i;
    callback = _cb;//绑定文件描述符和回调函数
    events = op;
}

/*void Channel::run_task(int i){
    add(callback(i));
}*/

Channel::~Channel(){
}
