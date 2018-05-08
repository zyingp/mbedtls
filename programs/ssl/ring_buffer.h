//
//  ring_buffer.h
//  mbedtls
//
//  Created by ZengYingpei on 2018/3/27.
//  Copyright © 2018年 ZengYingpei. All rights reserved.
//
//  Code modified from https://embedjournal.com/implementing-circular-buffer-embedded-c/
//  Modifications:
//  1. Add suppport for different content type (增加支持不同内容)
//  2. Add wait/signal for pop and push for multiple consumers/producers（增加同步来支持多个生产者消费者访问）

#ifndef ring_buffer_h
#define ring_buffer_h

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>


typedef struct {
    const uint8_t * buffer;
    size_t len;
} buffer_node;

// GCC old version doesn't support struct Designated Initializers, so I have to
// omit struct field name here.
// (GCC老版本(如4.4.7, 2010)反而不支持带名字的struct初始化，而支持不带名字的初始化。)
// https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html
#define CIRCBUF_DEF(x,y)          \
buffer_node x##_dataSpace[y];     \
circ_buf_t x = {           \
x##_dataSpace,             \
0,                         \
0,                         \
y,                         \
PTHREAD_MUTEX_INITIALIZER, \
PTHREAD_COND_INITIALIZER,  \
0                          \
}

typedef struct circ_buf {
    buffer_node * const buffer;
    int head;
    int tail;
    const int maxLen;   // 目前实现是实际支持存放maxLen个
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int should_break;   // whether we need to break wait for pop
} circ_buf_t;

void circ_buf_push(circ_buf_t *c, buffer_node data);
void circ_buf_pop(circ_buf_t *c, buffer_node *data);
// Indicate that consumers should break the endless wait for poping data
void circ_buf_break_wait_pop(circ_buf_t *c);

#endif /* ring_buffer_h */
