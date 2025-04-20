#ifndef PRODUCERCONSUMER_H
#define PRODUCERCONSUMER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__

#include <stdlib.h>
#include <stdbool.h>
#include "define.h"


typedef void* ItemData;

/**
 * @brief sem_buf 使用信号量sem实现的生产者和消费者
 * @note:
 */

typedef struct sem_buf sem_buf;

/**
 * @brief sembuf_init
 * @param sembuf: should not be null
 * @param maxsize: 可以存放最多多少个item
 * @param datalen: 每个item的字节长度
 * @return  -1: fail ,0-success
 */
sem_buf *sembuf_init(IN int maxsize, IN size_t datalen);

bool sembuf_remove(IN sem_buf *sembuf, OUT ItemData data);

bool sembuf_insert(IN sem_buf *sembuf, IN const ItemData data);

/**
 * @brief sembuf_destory 释放sembuf,如果不调用，会存在内存泄漏
 * @param sembuf
 */
void sembuf_destory(IN sem_buf **sembuf);


/**
 * @brief condition_buf 使用互斥和条件变量实现消费者和生产者
 */

typedef struct condition_buf condition_buf;

condition_buf *condbuf_init(int maxsize, size_t itemlen);

bool condbuf_insert(condition_buf *condbuf,const ItemData data);

bool condbuf_remove(condition_buf *condbuf, ItemData  data);

void condbuf_destory(condition_buf **condbuf);

#endif

#ifdef __cplusplus
}
#endif


#endif // PRODUCERCONSUMER_H
