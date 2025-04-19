#include  <gtest/gtest.h>
#include <iostream>
#include "mythread.h"
#include "ThreadPool.h"

class MyTask :public MyThread{
    virtual int doPrepare(){ std::cout <<  "prepare" << std::endl;  return 0;}
    virtual int doTask() { std::cout <<  "doTask" << std::endl;  return 0;}
    virtual void doFinish(){std::cout <<  "doFinish" << std::endl; }
};

TEST(test, MyThreadTest) {
    MyTask task;
    task.setSleepPolicy(MyThread::SLEEP_POLICY_SLEEP_FOR,1000);
    task.start();
    sleep(1);
    std::cout << "----------pause----------" << std::endl;
    task.pause();
    sleep(1);
    std::cout << "----------resume----------" << std::endl;
    task.resume();
    std::cout << "111111111111" << std::endl;
    sleep(1);
    std::cout << "222222222222" << std::endl;
    sleep(1);
    std::cout << "33333333333" << std::endl;
    task.stop(); //wait thread stop
}

static void printMsg(const std::string &msg)
{
    std::cout << msg.c_str() << std::endl;
}


TEST(test,MyThreadPoolTest)
{
    // 创建线程池，包含 4 个线程
    ThreadPool pool(4);

    // 提交一些任务
    auto result1 = pool.enqueue([] {
        std::cout << "Task 1 is running\n";
        return 1;
    });

    auto result2 = pool.enqueue([](int x) {
        std::cout << "Task 2 is running with x = " << x << "\n";
        return x * 2;
    }, 5);

    auto result3 = pool.enqueue([] {
        std::cout << "Task 3 is running\n";
        return 42;
    });

    auto result4 = pool.enqueue(printMsg, "helloworld");

    // 获取任务结果
    std::cout << "Result 1: " << result1.get() << "\n";
    std::cout << "Result 2: " << result2.get() << "\n";
    std::cout << "Result 3: " << result3.get() << "\n";
    result4.get();
}

#ifdef __linux__

#include "ThreadPoolPosix.h"

// 示例任务函数
void exampleTask(void* arg) {
    int num = *(int*)arg;
    printf("Task %d is running on thread %lu\n", num, pthread_self());
    sleep(1); // 模拟任务耗时
}

TEST(test,PosixThreadPoolTest)
{
    //创建线程池
   PosixThreadPool* pool = threadPoolCreate(4, 10);
   if (!pool) {
       fprintf(stderr, "Failed to create thread pool\n");
       return;
   }
   // 提交任务
   for (int i = 0; i < 20; i++) {
       int* arg = (int*)malloc(sizeof(int));
       *arg = i;
       Task task = {exampleTask, arg};
       if (threadPoolAdd(task, pool) != 0) {
           printf("Task %d was rejected (queue full)\n", i);
           free(arg);
       }
   }
   // 等待所有任务完成
   sleep(5);
   // 销毁线程池
   threadPoolDestroy(pool);

}

#endif


