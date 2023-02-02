# pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable Contemp;
    bool stop;
public:
    ThreadPool(int size = 8);
    ~ThreadPool();
    template<class F, class... Args>
    auto add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>;
};

template<typename F, typename...Args>
auto ThreadPool::add(F&& f, Args&&... args) ->std::future<typename std::result_of<F(Args...)>::type>{//添加任务
    using return_type = typename std::result_of<F(Args...)>::type;//using 类似typedef

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if(stop){
            throw std::runtime_error("ThreadPoll already stop, can't add task any more");
        }
        tasks.emplace([task](){ (*task)();} );
    }
    Contemp.notify_one();//唤醒1个线程
    return res;
}

