#ifndef PRODUCERCONSUMER_H
#define PRODUCERCONSUMER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__

#include <semaphore.h>
#include "define.h"


typedef void* ItemData;

typedef struct sem_buf sem_buf;

/**
 * @brief sembuf_init
 * @param sembuf: should not be null
 * @param n: 缓冲区的大小
 * @param datalen: 每个数据的字节长度
 * @return  -1: fail ,0-success
 */
sem_buf *sembuf_init(IN int n, IN size_t datalen);

void sembuf_remove(IN sem_buf *sembuf, OUT ItemData data);

void sembuf_insert(IN sem_buf *sembuf, IN const ItemData data);

/**
 * @brief sembuf_destory 释放sembuf,如果不调用，会存在内存泄漏
 * @param sembuf
 */
void sembuf_destory(IN sem_buf **sembuf);


#endif

#ifdef __cplusplus
}
#endif


#endif // PRODUCERCONSUMER_H
