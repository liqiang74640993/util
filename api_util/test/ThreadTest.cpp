#include  <gtest/gtest.h>
#include <iostream>
#include "mythread.h"
#include "ThreadPool.h"

class MyTask :public MyThread{
    virtual int doPrepare(){ std::cout <<  "prepare" << std::endl;  return 0;}
    virtual int doTask() { std::cout <<  "doTask" << std::endl;  return 0;}
    virtual void doFinish(){std::cout <<  "doFinish" << std::endl; }
};

TEST(ThreadTest, MyThread) {
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


TEST(ThreadTest, ThreadPool)
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

TEST(ThreadTest,PosixThreadPool)
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

#include "ProducerConsumerPosix.h"

sem_buf *g_buf = NULL;
struct object{
    int a;
    char str[100];
};

void *produceTask(void *arg)
{
    (void )arg;
    const char *temp = "hello";
    size_t len = strlen(temp);
    for(int i = 0; i < 100; i++){
        struct object o;
        o.a = (i+1);
        memcpy(o.str,temp,len);
        sembuf_insert(g_buf,(ItemData)&o);
    }
    return (void *)0;
}

void *consumerTask(void *arg)
{
    (void )arg;
    struct object o;
    for(size_t j = 0; j < 100; j++){
        sembuf_remove(g_buf,(ItemData)&o);
        std::cout << "o.a:" << o.a << ", o.str:" << o.str << std::endl;
    }
    return (void *)0;
}

TEST(ThreadTest,PosixProducerConsumer)
{
    g_buf = sembuf_init(10,sizeof(struct object));
    if(g_buf == NULL) return;
    pthread_t th1, th2;
    pthread_create(&th1,NULL,produceTask,NULL);
    pthread_create(&th2,NULL,consumerTask,NULL);
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    sembuf_destory(&g_buf);
}

int g_int = 0;
pthread_mutex_t mtx;

void *incr(void *arg){
    for(int i = 0; i < 10000; ++i){
        pthread_mutex_lock(&mtx);
        ++g_int;
        pthread_mutex_unlock(&mtx);
    }
    return (void *)0;
}


TEST(ThreadTest,PosixAPI)
{
    pthread_mutex_init(&mtx,NULL);

    pthread_t th1,th2;

    pthread_create(&th1,NULL,incr,NULL);//create thread
    pthread_create(&th2,NULL,incr,NULL);//create thread

    pthread_join(th1,NULL); //wait thread finish
    pthread_join(th2,NULL);

    std::cout << "g_int:" <<  g_int << std::endl;

}

#endif


