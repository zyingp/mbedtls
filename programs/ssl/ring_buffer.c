//
//  ring_buffer.c
//  mbedtls
//
//  Created by ZengYingpei on 2018/3/27.
//  Copyright © 2018年 ZengYingpei. All rights reserved.
//
//  Code modified from https://embedjournal.com/implementing-circular-buffer-embedded-c/
//  Modifications:
//  1. Add suppport for different content type (增加支持不同内容)
//  2. Add wait/signal for pop and push for multiple consumers/producers（增加同步来支持多个生产者消费者访问）

#include <stdbool.h>  // for bool type, require C99

#include "ring_buffer.h"


void circ_buf_push(circ_buf_t *c, buffer_node data)
{
    // next is where head will point to after this write.
    int s;
    s = pthread_mutex_lock(&(c->mtx));
    if (s != 0)
        printf("pthread_mutex_lock error\n");
    
    // 注意被唤醒后也要循环判断，因为可能被唤醒但是条件不满足，除非各只有一个生产者一个消费者线程
    for(;;)
    {
        int next = c->head + 1;
        if (next >= c->maxLen)
            next = 0;
        
        if (next == c->tail){ // check if circular buffer is full
            pthread_cond_wait(&(c->cond), &(c->mtx));
        }
        else{
            c->buffer[c->head] = data; // Load data and then move
            c->head = next;            // head to next data offset.
            break;            // Break loop after successful push.
        }
    }
    
    // Unlock first (有些平台这样性能更好)
    pthread_mutex_unlock(&(c->mtx));
    
    // Then, wake all waiters, including consumers
    pthread_cond_broadcast(&(c->cond));
    
}

void circ_buf_pop(circ_buf_t *c, buffer_node *data)
{
    int s;
    s = pthread_mutex_lock(&(c->mtx));
    
    // if the head isn't ahead of the tail, we don't have any characters
    while (c->head == c->tail && c->should_break != true){         // check if circular buffer is empty
        pthread_cond_wait(&(c->cond), &(c->mtx)); // and return with an error
    }
    
    // If we need to break wait, then fill with empty data.
    // However, if there are still has data, we should read first(如果还有数据，即使要break也先读完).
    if (c->head == c->tail && c->should_break)
    {
        data->buffer = NULL;
        data->len = 0;
    }else
    {
        // next is where tail will point to after this read.
        int next = c->tail + 1;
        if(next >= c->maxLen)
            next = 0;
        
        *data = c->buffer[c->tail]; // Read data and then move
        c->tail = next;             // tail to next data offset.
    }
    
    // Unlock first (有些平台这样性能更好)
    pthread_mutex_unlock(&(c->mtx));
    
    // Then, wake all waiters, including producers
    pthread_cond_broadcast(&(c->cond));
    
    
}

// Indicate that consumers should break the endless wait for poping data
void circ_buf_break_wait_pop(circ_buf_t *c)
{
    c->should_break = true;
    pthread_cond_broadcast(&(c->cond));
}

