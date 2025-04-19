#include <stdlib.h>
#include <pthread.h>
#include "ProducerConsumerPosix.h"

typedef struct sem_buf{
    ItemData *buf; //存放数据的缓冲区
    int n;  //缓冲区的大小
    int front; // buf[(front+1)%n] is the first item
    int rear; //buf[rear % n] is the rear item
    sem_t bufSem; //用于控制读写buf的信号量
    sem_t slot; //counts available slots
    sem_t item;//counts available items
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
    int ret = 0;
    ret = sem_init(&sembuf->bufSem,0,1);
    if(ret != 0) goto Err;
    ret = sem_init(&sembuf->slot,0,n);
    if(ret != 0) goto Err;
    ret =  sem_init(&sembuf->item,0,0);//
    if(ret != 0) goto Err;
    return sembuf;
Err:
    free(sembuf);
    return NULL;
}

#include <stdio.h>
void sembuf_remove(sem_buf *sembuf, ItemData *data)
{
    //移除时要判断是否有数据
    sem_wait(&sembuf->item);//
    sem_wait(&sembuf->bufSem);
    *data = sembuf->buf[(++sembuf->front)% sembuf->n];
    printf("sembuf_remove:%d\n",*data);
    sem_post(&sembuf->bufSem);
    sem_post(&sembuf->slot);
}


void sembuf_insert(sem_buf *sembuf, const ItemData *data)
{
    //插入时要判断是否有空闲的位置
    sem_wait(&sembuf->slot);
    sem_wait(&sembuf->bufSem);
    sembuf->buf[(++sembuf->rear) % sembuf->n] = *data;
    printf("sembuf_insert:%d\n",(int *)sembuf->buf[(sembuf->rear) % sembuf->n]);
    sem_post(&sembuf->bufSem);
    sem_post(&sembuf->item);
}

void sembuf_destory(sem_buf **sembuf)
{

    if(sembuf == NULL || *sembuf == NULL) return;
    sem_buf *temp = *sembuf;
    sem_destroy(&temp->bufSem);
    sem_destroy(&temp->item);
    sem_destroy(&temp->slot);
    free(temp->buf);
    free(temp);
    *sembuf = NULL;
}
