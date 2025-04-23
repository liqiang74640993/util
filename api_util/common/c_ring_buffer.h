/**
 * \file ring_buffer.h
 * \brief 简易环形缓冲相关定义与声明
 * \author netube_99\netube@163.com
 * \date 2022.08.20
 * \version v0.4.0
*/
#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif



//返回值定义
#define RING_BUFFER_SUCCESS     0x01
#define RING_BUFFER_ERROR       0x00

/**
   C语言实现的环形缓冲区
*/

//环形缓冲区结构体
//如果所有数组都用于存放元素，那么当缓冲区的长度为0或者缓冲区满的时候，
//rear 和front指针都想等。因此为了区别满和空的状态，缓冲区的一个位置不用于存放数据
//buffer empty:  front == rear
//buffer full:   front == (rear + 1) % max_Length
//buffer length : (rear + max_Length - front) % max_Length
typedef struct ring_buffer
{
    size_t head ;             //操作头指针
    size_t tail ;             //操作尾指针
    size_t Length ;           //已储存的数据量
    uint8_t *array_addr ;       //缓冲区储存数组基地址
    size_t max_Length ;       //缓冲区最大可储存数据量
}ring_buffer;

uint8_t rb_init(ring_buffer *rb_handle, uint8_t *buffer_addr ,size_t buffer_size);               //初始化基础环形缓冲区
uint8_t rb_delete(ring_buffer *rb_handle, size_t Length);                                        //从头指针开始删除指定长度的数据
uint8_t rb_write_byte(ring_buffer *rb_handle, uint8_t data);                                       //向缓冲区尾指针写一个字节
uint8_t rb_write_string(ring_buffer *rb_handle, uint8_t *input_addr, size_t write_Length);       //向缓冲区尾指针写指定长度数据
uint8_t rb_read_byte(ring_buffer *rb_handle, uint8_t *output_addr);                                //从缓冲区头指针读一个字节
uint8_t rb_read_string(ring_buffer *rb_handle, uint8_t *output_addr, size_t read_Length);        //从缓冲区头指针读指定长度数据
size_t rb_get_length(ring_buffer *rb_handle);                                                    //获取缓冲区里已储存的数据长度
size_t rb_get_freeSize(ring_buffer *rb_handle);                                                  //获取缓冲区可用储存空间

#ifdef __cplusplus
}
#endif

#endif//#ifndef _RING_BUFFER_H_
