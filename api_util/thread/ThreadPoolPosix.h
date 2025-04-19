#ifndef THREADPOOLPOSIX_H
#define THREADPOOLPOSIX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 定义任务结构
typedef struct Task {
    void (*function)(void*); // 任务函数指针
    void* arg;               // 任务参数
} Task;

// 定义线程池结构
typedef struct PosixThreadPool {
    pthread_mutex_t lock;          // 互斥锁保护任务队列
    pthread_cond_t notify;         // 条件变量，用于任务通知
    Task* taskQueue;               // 任务队列
    int queueSize;                 // 队列大小
    int queueFront;                // 队列头部索引
    int queueRear;                 // 队列尾部索引
    int count;                     // 当前任务数
    int threadCount;               // 线程数量
    pthread_t* threads;            // 工作线程数组
    int stop;                      // 停止标志
} PosixThreadPool;

// 任务队列操作宏
#define NEXT_INDEX(i, size) ((i + 1) % size)

PosixThreadPool* threadPoolCreate(int threadCount, int queueSize);
void* threadWorker(void* arg);
int threadPoolAdd(Task task, PosixThreadPool* pool);
void threadPoolDestroy(PosixThreadPool* pool);


#endif

#ifdef __cplusplus
}
#endif

#endif // THREADPOOLPOSIX_H
