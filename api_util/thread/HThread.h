#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <thread> //thread
#include <atomic> //atomic
#include <chrono> //time

using namespace std;

class HThread
{
public:
     enum Status{
         STATUS_STOP,
         STATUS_RUNNING,
         STATUS_PAUSE
     };

     enum SleepPolicy{
         SLEEP_POLICY_YIELD,//
         SLEEP_POLICY_SLEEP_UNTIL,//
         SLEEP_POLICY_SLEEP_FOR, //执行完任务后，休眠指定的时间
         SLEEP_POLICY_NO_SLEEP //not sleep
     };

     /**
     * @brief setSleepPolicy
     * @param policy
     * @param millseconds :sleep ms
     * @param reCountTime
     */
    void setSleepPolicy(SleepPolicy policy,uint64_t millseconds)
    {
        m_policy = policy;
        m_millseconds = millseconds;
    }

    thread::id getId();//thread id

    virtual int start();
    virtual int resume();
    virtual int stop();
    virtual int pause();

protected:
    //success - 0
    virtual int doPrepare(){ return 0;}
    virtual int doTask() { return 0;}
    virtual void doFinish(){}
private:
    std::atomic<Status> m_status;//status
    std::atomic<bool> m_statusChange;//is status change
    SleepPolicy m_policy; //sleep policy
    uint64_t m_millseconds;//sleep_for休眠的ms,sleep_until则为修改状态后，重新计算休眠时间
    std::thread m_thread;//thread object
    std::chrono::system_clock::time_point m_tp;
    int run();
    void sleep();
    void changeStatus(Status status);
public:
    HThread();
    HThread(std::thread&& t);
    //not copy and move

    HThread(const HThread&) = delete;
    HThread& operator=(const HThread& othre) = delete;
    HThread(const HThread&&) = delete;
    HThread& operator=(const HThread&& othre) = delete;

    virtual ~HThread(){ stop(); } //
};

#endif // MYTHREAD_H
