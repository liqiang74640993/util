#ifndef MYTHREADPOOL_H
#define MYTHREADPOOL_H

#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>
#include <memory>
#include <stdexcept>
#include <atomic>

class ThreadPool
{
public:
    ThreadPool(int threadsize);

    template<typename F, typename ...Args>
    auto enqueue(F  &&f, Args&& ...args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    std::vector<std::thread> m_workers;
    std::queue< std::function<void()> > m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stop;
};

ThreadPool::ThreadPool(int threadSize):m_stop(false)
{
    for(int i = 0; i < threadSize; ++i){
        m_workers.emplace_back(
            [this](){
                    std::function<void()> task;
                    for(;;){
                    //get one task
                    {
                        std::unique_lock<std::mutex> lock(this->m_mutex);// set one lock
                        m_cv.wait(lock, [this](){
                            return this->m_stop || !this->m_tasks.empty();
                        }
                        );
                        if(this->m_stop && this->m_tasks.empty()){//pool exit and no task
                            return;
                        }
                        task = std::move(this->m_tasks.front());
                        this->m_tasks.pop();   
                    }   
                    task();//execute task;
                }
            }
        );
    }
}

template<typename F, class ...Args>
auto ThreadPool::enqueue(F &&f, Args&& ...args)
-> std::future<typename std::result_of<F(Args ...)>::type>
{
    using returnType = typename std::result_of<F(Args...)>::type; //returnType
    //package task
    auto task = std::make_shared<std::packaged_task<returnType()>>
                  (std::bind(std::forward<F>(f),std::forward<Args>(args)...));
    std::future<returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_stop){
            throw std::runtime_error("thread pool has stopped,can't enquqe task");
        }
        m_tasks.emplace([task](){(*task)();});
    }
    m_cv.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for(auto &work: m_workers){
        work.join();
    }
}


#endif // MYTHREADPOOL_H
