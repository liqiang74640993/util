#include "ThreadPoolPosix.h"


#ifdef __linux__

// 线程运行函数
void* threadWorker(void* arg) {
    PosixThreadPool* pool = (PosixThreadPool*)arg;
    while (1) {
        Task task;

        // 获取任务
        pthread_mutex_lock(&pool->lock);
        while (pool->count == 0 && !pool->stop) { //condition wait
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        if (pool->stop && pool->count == 0) { //pool exit
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // 取出任务
        task.function = pool->taskQueue[pool->queueFront].function;
        task.arg = pool->taskQueue[pool->queueFront].arg;
        pool->queueFront = NEXT_INDEX(pool->queueFront, pool->queueSize);
        pool->count--;

        pthread_mutex_unlock(&pool->lock);

        // 执行任务
        (*(task.function))(task.arg);
    }
    return NULL;
}

// 创建线程池
PosixThreadPool* threadPoolCreate(int threadCount, int queueSize) {
    PosixThreadPool* pool = (PosixThreadPool*)malloc(sizeof(PosixThreadPool));
    if (!pool) {
        perror("Failed to allocate thread pool");
        return NULL;
    }

    // 初始化线程池结构
    pool->threadCount = threadCount;
    pool->queueSize = queueSize;
    pool->queueFront = 0;
    pool->queueRear = 0;
    pool->count = 0;
    pool->stop = 0;
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * threadCount);
    pool->taskQueue = (Task*)malloc(sizeof(Task) * queueSize);

    if (!pool->threads || !pool->taskQueue) {
        perror("Failed to allocate threads or task queue");
        free(pool->threads);
        free(pool->taskQueue);
        free(pool);
        return NULL;
    }

    // 初始化互斥锁和条件变量
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);

    // 创建工作线程
    for (int i = 0; i < threadCount; i++) {
        if (pthread_create(&pool->threads[i], NULL, threadWorker, pool) != 0) {
            perror("Failed to create thread");
            threadPoolDestroy(pool);
            return NULL;
        }
    }

    return pool;
}

// 提交任务到线程池
int threadPoolAdd(Task task, PosixThreadPool* pool) {
    pthread_mutex_lock(&pool->lock);

    if (pool->count == pool->queueSize) {
        // 队列已满
        pthread_mutex_unlock(&pool->lock);
        return -1;
    }

    // 添加任务到队列
    pool->taskQueue[pool->queueRear].function = task.function;
    pool->taskQueue[pool->queueRear].arg = task.arg;
    pool->queueRear = NEXT_INDEX(pool->queueRear, pool->queueSize);
    pool->count++;

    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    return 0;
}
// 销毁线程池
void threadPoolDestroy(PosixThreadPool* pool) {
    if (pool == NULL) return;

    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    // 等待所有线程退出
    for (int i = 0; i < pool->threadCount; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // 清理资源
    free(pool->threads);
    free(pool->taskQueue);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    free(pool);
}

#endif
