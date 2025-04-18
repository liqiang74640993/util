#include "mythread.h"
#include "htime.h"

MyThread::MyThread():
    m_status(STATUS_STOP),m_statusChange(false),
    m_policy(SLEEP_POLICY_NO_SLEEP),m_millseconds(0)
{

}

int MyThread::start()
{
    if(m_status != STATUS_STOP)//already start
        return 0;
    int ret = 0;
    m_thread = std::thread([this,&ret](){
        if((ret = doPrepare())!= 0) return;
        changeStatus(STATUS_RUNNING);
        ret = run();
        changeStatus(STATUS_STOP);
        doFinish();
        return;}
    );
    return 0;
}

int MyThread::resume()
{
   if(m_status != STATUS_RUNNING){
        changeStatus(STATUS_RUNNING);
   }
   return 0;
}

int MyThread::stop()
{
    if(m_status != STATUS_STOP){
        changeStatus(STATUS_STOP);
    }
    if(m_thread.joinable()){
        m_thread.join();
    }
    return 0;
}

int MyThread::pause()
{
    if(m_status == STATUS_RUNNING){
        changeStatus(STATUS_PAUSE);
    }
    return 0;
}

int MyThread::run()
{
    while(m_status != STATUS_STOP){
        if(m_status  == STATUS_PAUSE){
            std::this_thread::yield();//让出cpu
            continue;
        }
        doTask();
        sleep();
    }
    return 0;
}

void MyThread::sleep()
{
    switch (m_policy) {
    case SLEEP_POLICY_YIELD:
        std::this_thread::yield();
        break;
    case SLEEP_POLICY_SLEEP_FOR:
        std::this_thread::sleep_for(std::chrono::milliseconds(m_millseconds));
        break;
    case SLEEP_POLICY_SLEEP_UNTIL: //
        if(m_statusChange){
            m_statusChange = false;
            m_tp = std::chrono::system_clock::now();
        }
        m_tp += std::chrono::milliseconds(m_millseconds);
        std::this_thread::sleep_until(m_tp);
        break;
    default: //do nothing
        break;
    }
}

void MyThread::changeStatus(Status status)
{
   m_status = status;
   m_statusChange = true;
}
