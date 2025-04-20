#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "ProducerConsumerPosix.h"

typedef struct sem_buf{
    ItemData *buf; //存放数据的缓冲区
    size_t itemlen;// item length
    int n;  //缓冲区的大小
    int front; // buf[(front+1)%n] is the first item
    int rear; //buf[rear % n] is the rear item
    sem_t mutex; //用于控制读写buf的信号量
    sem_t slots; //counts available slots
    sem_t items;//counts available items
}sem_buf;

sem_buf *sembuf_init(IN int n, IN size_t datalen)
{
    if(n == 0 || datalen == 0) return NULL;
    sem_buf *sembuf = (sem_buf *)malloc(sizeof(sem_buf));
    if(sembuf == NULL) return NULL;
    sembuf->n = n;
    sembuf->buf = (ItemData *)calloc(n,datalen);
    sembuf->rear = 0;
    sembuf->front = 0;
    sembuf->itemlen = datalen;
    int ret = 0;
    ret = sem_init(&sembuf->mutex,0,1);
    if(ret != 0) goto Err;
    ret = sem_init(&sembuf->slots,0,n);
    if(ret != 0) goto Err;
    ret =  sem_init(&sembuf->items,0,0);//
    if(ret != 0) goto Err;
    return sembuf;
Err:
    free(sembuf);
    return NULL;
}


void sembuf_remove(sem_buf *sembuf, ItemData data)
{
    //移除时要判断是否有数据
    sem_wait(&sembuf->items);//
    sem_wait(&sembuf->mutex);
    memcpy(data,&sembuf->buf[(++sembuf->front)% (sembuf->n)],sembuf->itemlen);
    //(ItemData*)data = sembuf->buf[(++sembuf->front)% (sembuf->n)];// 赋值
    sem_post(&sembuf->mutex);
    sem_post(&sembuf->slots);
}


void sembuf_insert(sem_buf *sembuf, const ItemData data)
{
    //插入时要判断是否有空闲的位置
    sem_wait(&sembuf->slots);
    sem_wait(&sembuf->mutex);
    memcpy(&sembuf->buf[(++sembuf->rear) % sembuf->n],data,sembuf->itemlen);
    //sembuf->buf[(++sembuf->rear) % sembuf->n] = (ItemData*) data;
    sem_post(&sembuf->mutex);
    sem_post(&sembuf->items);
}

void sembuf_destory(sem_buf **sembuf)
{

    if(sembuf == NULL || *sembuf == NULL) return;
    sem_buf *temp = *sembuf;
    sem_destroy(&temp->mutex);
    sem_destroy(&temp->items);
    sem_destroy(&temp->slots);
    free(temp->buf);
    free(temp);
    *sembuf = NULL;
}
