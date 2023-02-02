#include"ThreadPool.h"

ThreadPool::ThreadPool(int size):stop(false){
    for (int i = 0; i < size; i++){
        //添加线程
        threads.emplace_back(std::thread([this](){
            while(true){
                std::function<void( )> task;
                {   //在这个{}作用域内(lock的生命周期内)对std::mutex加锁，出了作用域会自动解锁，不需要调用unlock()
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    Contemp.wait(lock, [this](){//等待条件变量，条件为任务队列不为空或线程池停止
                        return stop ||!tasks.empty();
                    });
                    if(stop && tasks.empty())return;//如果线程池终止或任务队列为空，弹出
                    task = tasks.front();//取出任务
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    Contemp.notify_all();//唤醒所有阻塞线程
    for(std::thread &th:threads){
        if(th.joinable()){
            th.join();//连接线程
        }
    }
}


