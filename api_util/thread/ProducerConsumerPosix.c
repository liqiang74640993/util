#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "c_ring_buffer.h"
#include "ProducerConsumerPosix.h"

#ifdef __linux__

//sem buf define
typedef struct sem_buf{
    ring_buffer rb_buf;//缓冲区对象
    ItemData buf; //存放数据的缓冲区
    size_t itemlen;// 每个item length
    sem_t mutex; //用于控制读写buf的信号量
    sem_t slots; //counts available slots
    sem_t items;//counts available items
}sem_buf;

sem_buf *sembuf_init(int maxsize, size_t datalen)
{
    if(maxsize == 0 || datalen == 0) return NULL;
    sem_buf *sembuf = (sem_buf *)malloc(sizeof(sem_buf));
    if(sembuf == NULL) return NULL;
    sembuf->buf = (ItemData)calloc(maxsize,datalen);
    if(sembuf->buf == NULL) goto Err;
    int ret = rb_init(&sembuf->rb_buf,(uint8_t*)sembuf->buf,maxsize * datalen); //maxsize * datalen is the total size
    if(ret != RING_BUFFER_SUCCESS) goto Err;
    sembuf->itemlen = datalen;
    ret = sem_init(&sembuf->mutex,0,1);
    if(ret != 0) goto Err;
    ret = sem_init(&sembuf->slots,0,maxsize);
    if(ret != 0) goto Err;
    ret =  sem_init(&sembuf->items,0,0);//
    if(ret != 0) goto Err;
    return sembuf;
Err:
    if(sembuf->buf != NULL)
        free(sembuf->buf);
    free(sembuf);
    return NULL;
}

bool sembuf_insert(sem_buf *sembuf, const ItemData data)
{
    //插入时要判断是否有空闲的位置
    sem_wait(&sembuf->slots);
    sem_wait(&sembuf->mutex);
    int ret = rb_write_string(&sembuf->rb_buf,(uint8_t *)data, sembuf->itemlen);
    sem_post(&sembuf->mutex);
    sem_post(&sembuf->items);
    return ret == RING_BUFFER_SUCCESS ? true : false;
}


bool sembuf_remove(sem_buf *sembuf, ItemData data)
{
    //移除时要判断是否有数据
    sem_wait(&sembuf->items);//
    sem_wait(&sembuf->mutex);
    int ret = rb_read_string(&sembuf->rb_buf,(uint8_t *)data,sembuf->itemlen);
    sem_post(&sembuf->mutex);
    sem_post(&sembuf->slots);
    return ret == RING_BUFFER_SUCCESS ? true : false;
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

typedef struct condition_buf
{
    ItemData buf;//data buf
    ring_buffer rb_buf; //ring buffer
    size_t itemlen;// 每个item length
    pthread_mutex_t buf_mutex;
    //使用一个锁即可。
   // pthread_mutex_t full_mutex;
   // pthread_mutex_t empty_mutex;
    pthread_cond_t full_cv;
    pthread_cond_t empty_cv;
}condition_buf;

condition_buf *condbuf_init(int maxsize, size_t itemlen)
{
    condition_buf *cbuf = (condition_buf *)calloc(1, sizeof(condition_buf));
    if(cbuf == NULL) return NULL;
    cbuf->buf = (ItemData)calloc(maxsize,itemlen);
    if(cbuf->buf == NULL){
        goto Err;
    }
    int ret = RING_BUFFER_SUCCESS;
    ret = rb_init(&cbuf->rb_buf,(uint8_t*)cbuf->buf,maxsize * itemlen);
    if(ret != RING_BUFFER_SUCCESS) goto Err;
    cbuf->itemlen = itemlen;
    pthread_mutex_init(&cbuf->buf_mutex,NULL);
    pthread_cond_init(&cbuf->full_cv,NULL);
    pthread_cond_init(&cbuf->empty_cv,NULL);
    return cbuf;
 Err:
    if(cbuf->buf != NULL)
        free(cbuf->buf);
    free(cbuf);
    return NULL;
}

bool condbuf_insert(condition_buf *condbuf,const ItemData data)
{
    pthread_mutex_lock(&condbuf->buf_mutex);
    while(rb_get_freeSize(&condbuf->rb_buf) == 0){
        pthread_cond_wait(&condbuf->full_cv,&condbuf->buf_mutex);//当缓冲区满时,会主动释放锁
    }
    int ret = rb_write_string(&condbuf->rb_buf,(uint8_t *)data, condbuf->itemlen);
    pthread_cond_signal(&condbuf->empty_cv);
    pthread_mutex_unlock(&condbuf->buf_mutex);
    return ret == RING_BUFFER_SUCCESS ? true : false;
}

bool condbuf_remove(condition_buf *condbuf, ItemData  data)
{
    pthread_mutex_lock(&condbuf->buf_mutex);
    while(rb_get_length(&condbuf->rb_buf) == 0){
        pthread_cond_wait(&condbuf->empty_cv,&condbuf->buf_mutex);//当缓冲区empty,会主动释放锁
    }
    int ret = rb_read_string(&condbuf->rb_buf,(uint8_t*)data,condbuf->itemlen);
    pthread_cond_signal(&condbuf->full_cv);
    pthread_mutex_unlock(&condbuf->buf_mutex);
    return ret == RING_BUFFER_SUCCESS ? true : false;
}

void condbuf_destory(condition_buf **condbuf)
{
    if(condbuf == NULL || *condbuf == NULL) return;
    condition_buf *temp = *condbuf;
    pthread_mutex_destroy(&temp->buf_mutex);
    pthread_cond_destroy(&temp->full_cv);
    pthread_cond_destroy(&temp->empty_cv);
    if(temp->buf) free(temp->buf);
    free(temp);
    *condbuf = NULL;
}

#endif

