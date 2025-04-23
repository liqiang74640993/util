#include  <gtest/gtest.h>
#include <iostream>
#include "HThread.h"
#include "ThreadPool.h"
#include <shared_mutex>

int add_th1(int a, int b)
{
   int sum = a + b;
   std::cout <<"a:" << a << ",b:" << b << ",sum:" <<  sum << std::endl;
   return sum;
}

class Adder{
public:
    int operator()(int a,int b){
        return add_th1(a,b);
    }
};

TEST(ThreadTest, CreateThread)
{
    std::thread t1(add_th1,1,1);
    int a = 10,b = 20;
    std::thread t2([a,b](){std::cout << "a + b:" << a + b << std::endl; }); //lumbda
    int (*addFunc)(int, int) = add_th1;
    std::thread t3(addFunc,1,2); //function ptr
    Adder adder;
    std::thread t4(adder,3,2); //仿函数（函数对象）
    std::function<int(int,int)> func = add_th1;
    std::thread t5(func,2,4);
    auto bindobj = std::bind(add_th1,std::placeholders::_1,3);//bind object
    std::thread t6(bindobj,6);
    t1.join();//一定要在和线程相关联的std::thread对象销毁前，对线程运用join
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}

int g_count = 0;
std::mutex g_mutex;

void addCountWithMutex(){
    for(int i = 0; i < 10000; i++){
        //std::lock_guard<std::mutex> lock(g_mutex);
        std::unique_lock<std::mutex> lock(g_mutex);
        g_count++;
    }
}

int g_shared_count = 0;
std::shared_mutex g_sharedMutex;
void addCountWithSharedMutex()
{
    for(int i = 0; i < 10000; i++){
        //这个放在循环里面，如果放在外面，排他锁一直占有，相当单线程execute。
        std::unique_lock<std::shared_mutex> lock(g_sharedMutex);//std::unique_lock lock(g_sharedMutex)
//        std::lock_guard<std::shared_mutex> lock(g_sharedMutex);
        //g_sharedMutex.lock();//execusive mutex lock,
        g_shared_count++;
        //g_sharedMutex.unlock();
    }
}

void getCountWithShareMutex()
{
    //共享锁加在for循环的外面，可以看到里面的thread_id打印是交替的，可以看到共享锁可以同时被多个线程共享。
    //但是g_shared_count的数值一直没变化，说明虽然可以多个共享锁可以同时读取，但是一旦有线程获取到共享锁，就不能有线程获取到排他锁
    std::shared_lock lock(g_sharedMutex); //g_sharedMutex.lock_shared() ,g_sharedMutex.unlock_shared()
    //g_sharedMutex.lock_shared();
    for(int i = 0; i < 400; i++)
        std::cout << "thread_id:" << std::this_thread::get_id() << "thread_count:" <<
                     g_shared_count  << std::endl;
    //g_sharedMutex.unlock_shared();
}


TEST(ThreadTest, MutexTest)
{
    std::thread t1(addCountWithMutex);
    std::thread t2(addCountWithMutex);
    t1.join();
    t2.join();
    std::cout << "g_count :" << g_count << std::endl;

    std::thread t3(addCountWithSharedMutex);
    std::thread t6(addCountWithSharedMutex);
    std::thread t4(getCountWithShareMutex);
    std::thread t5(getCountWithShareMutex);
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    std::cout << "g_share_count:" << g_shared_count << std::endl;//g_sahre_count:20000,排他锁是互斥的
}


std::atomic<int> g_atomicCount = 0;
void addCountAtomic(){
    for(int i = 0; i < 10000; i++){
        g_atomicCount++;
    }
}

void increment() {
    g_atomicCount.fetch_add(1, std::memory_order_relaxed);//++ operation
}


//原子布尔类型可以实现互斥锁的部分功能，但在使用条件变量condition variable时，
//仍然需要mutex保护对condition variable的消费，即使condition variable是一个atomic object。
std::chrono::milliseconds interval(100);
std::atomic<bool> readyFlag(false);     //原子布尔类型，取代互斥量
std::atomic<int> job_shared(0); //两个线程都能修改'job_shared',将该变量特化为原子类型
int job_exclusive = 0; //只有一个线程能修改'job_exclusive',不需要保护
//此线程只能修改 'job_shared'
void job_1()
{
    std::this_thread::sleep_for(5 * interval);
    job_shared.fetch_add(1);
    std::cout << "job_1 shared (" << job_shared.load() << ")\n";
    readyFlag.store(true);      //改变布尔标记状态为真
}
// 此线程能修改'job_shared'和'job_exclusive'
void job_2()
{
    while (true) {    //无限循环，直到可访问并修改'job_shared'
        if (readyFlag.load()) {     //判断布尔标记状态是否为真，为真则修改‘job_shared’
            job_shared.fetch_add(1);
            std::cout << "job_2 shared (" << job_shared.load() << ")\n";
            int oldValue = job_shared.exchange(30); //exchange ,return the old value
            std::cout << "oldValue:" << oldValue << " newValue:" << job_shared.load() << std::endl;

            int expected = 70;
            int newValue = 80;
            //compare_exchange_weak 和 compare_exchange_strong 方法：原子地比较 std::atomic 对象的值与期望值，如果相等，
            //则将其替换为新值；如果不相等，则将期望值更新为 std::atomic 对象的当前值。
            //compare_exchange_weak 可能会虚假失败，而 compare_exchange_strong 不会。CAS:
            bool success = job_shared.compare_exchange_strong(expected, newValue); //
            if (success) {
                std::cout << "Value was updated to " << newValue << std::endl;
            } else {
                std::cout << "Value was not updated. Expected: " << expected << std::endl; //expected:30
            }
            std::cout << "job_shared:" << job_shared.load() << std::endl;//return expected:30

            return;
        } else {      //布尔标记为假,则修改'job_exclusive'
            ++job_exclusive;
            std::cout << "job_2 exclusive (" << job_exclusive << ")\n";
            std::this_thread::sleep_for(interval);
        }
    }
}


std::atomic<int> head(0);
std::atomic<int> tail(0);

//由于 std::memory_order_relaxed 不提供线程间的同步保证，可能会出现以下问题：

//数据不一致：其他线程可能无法及时看到 head 和 tail 的更新。例如，在上述示例中，updateTail 线程可能在
//updateHead 线程更新 head 之前就复制了 head 的值，导致 tail 的值不是最新的 head 值。
void updateTail() {
    tail.store(head.load(std::memory_order_relaxed), std::memory_order_relaxed);
    std::cout << "Tail is updated to: " << tail.load() << std::endl;
}

void updateHead() {
    head.fetch_add(1, std::memory_order_relaxed);
    std::cout << "Head is updated to: " << head.load() << std::endl;
}


TEST(ThreadTest, AtomicTest)
{
    std::thread t1(addCountAtomic);
    std::thread t2(addCountAtomic);
    t1.join();
    t2.join();
    std::cout << "g_atomiccount :" << g_atomicCount << std::endl;

    g_atomicCount = 0;
    std::thread t3(increment);
    std::thread t4(increment);
    t3.join();
    t4.join();
    std::cout << "Counter value: " << g_atomicCount.load(std::memory_order_relaxed) << std::endl;

    std::thread t5(job_1);
    std::thread t6(job_2);
    t5.join();
    t6.join();

    std::thread t8(updateTail);//Tail can be 0 or 1
    std::thread t7(updateHead);


    t7.join();
    t8.join();

}





class MyTask :public HThread{
    virtual int doPrepare(){ std::cout <<  "prepare" << std::endl;  return 0;}
    virtual int doTask() { std::cout <<  "doTask" << std::endl;  return 0;}
    virtual void doFinish(){std::cout <<  "doFinish" << std::endl; }
};

TEST(ThreadTest, HThread) {
    MyTask task;
    task.setSleepPolicy(HThread::SLEEP_POLICY_SLEEP_FOR,1000);
    task.start();
    sleep(1);
    std::cout << "----------pause----------" << std::endl;
    task.pause();
    std::cout << "after pause" << std::endl;
    sleep(1);
    std::cout << "----------resume----------" << std::endl;
    task.resume();
    sleep(1);
    std::cout << "after resumer" << std::endl;
    sleep(1);
    task.stop(); //wait thread stop
    std::cout << "after stop" << std::endl;
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

std::once_flag onceflag;

void CallOnce() {
   std::call_once(onceflag, []() {
       cout << "call once" << endl;
  });
}

TEST(ThreadTest, ThreadCallOnce)
{
    std::thread threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(CallOnce);
     }
     for (auto& th : threads) {
        th.join();
     }
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

#ifdef SEM_BUF

sem_buf *g_buf = NULL;

#else

condition_buf *g_buf = NULL;

#endif

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
#ifdef SEM_BUF
        sembuf_insert(g_buf,(ItemData)&o);
#else
        condbuf_insert(g_buf,(ItemData)&o);
#endif
    }
    return (void *)0;
}

void *consumerTask(void *arg)
{
    (void )arg;
    struct object o;
    for(size_t j = 0; j < 100; j++){
#ifdef SEM_BUF
     sembuf_remove(g_buf,(ItemData)&o);
#else
     condbuf_remove(g_buf,(ItemData)&o);
#endif
        std::cout << "o.a(int):" << o.a << ", o.str(string):" << o.str << std::endl;
    }
    return (void *)0;
}

TEST(ThreadTest,PosixProducerConsumer)
{
#ifdef SEM_BUF
    g_buf = sembuf_init(10,sizeof(struct object));
#else
    g_buf = condbuf_init(10,sizeof(struct object));
#endif
    if(g_buf == NULL) return;
    pthread_t th1, th2;
    pthread_create(&th1,NULL,produceTask,NULL);
    pthread_create(&th2,NULL,consumerTask,NULL);
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
#ifdef SEM_BUF
    sembuf_destory(&g_buf);
#else
    condbuf_destory(&g_buf);
#endif

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


