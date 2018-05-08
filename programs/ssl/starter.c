//
//  starter.c
//  mbedtls
//
//  Created by ZengYingpei on 2018/3/18.
//  Copyright © 2018年 ZengYingpei. All rights reserved.
//

#include <pthread.h>

#include "starter.h"

#include "ssl_client1.c"
#include "ssl_server.c"
#include "ring_buffer.h"

CIRCBUF_DEF(client2server, 32);
CIRCBUF_DEF(server2client, 32);

buffer_t client_read_temp =
{
    .buf = NULL,
    .size = 0,
    .offset = 0
};

buffer_t server_read_temp =
{
    .buf = NULL,
    .size = 0,
    .offset = 0
};

static void printHexBytes(const unsigned char* bytes, int byteSize)
{
    char* buffer = (char*)malloc(byteSize * 2 + 1);
    buffer[byteSize * 2] = 0;    // mark the last position as \0
    for(int i = 0; i < byteSize; i++){
        snprintf(&(buffer[2*i]), 3, "%02X", (unsigned char)bytes[i]);
    }
    printf("0x%s",  buffer);
    free(buffer);
}

int get_from_left_buffer(buffer_t *left_buf, const unsigned char *buf, size_t len)
{
    size_t left_len = left_buf->size - left_buf->offset;
    if(left_len > 0 )
    {
        size_t len_to_copy = left_len;
        if (left_len > len)
        {
            len_to_copy = len;
        }
        memcpy((void*)buf, left_buf->buf + left_buf->offset, len_to_copy);
        if (len_to_copy == left_len)
        {   // all data are got
            free(left_buf->buf);
            left_buf->buf = NULL;
            left_buf->offset = 0;
            left_buf->size = 0;
        }else{
            left_buf->offset += len_to_copy;
        }
        return (int)len_to_copy;
    }
    return -1;
}

int server_membuf_send( void *ctx, const unsigned char *buf, size_t len )
{
    buffer_node node;
    char *newbuf = (char *)malloc(len);
    memcpy(newbuf, buf, len);
    node.buffer = (uint8_t *)newbuf;
    node.len = len;
    circ_buf_push(&server2client, node);
    return (int)len;
}

int server_membuf_recv( void *ctx, unsigned char *buf, size_t len )
{
    int got_from_left = get_from_left_buffer(&server_read_temp, buf, len);
    if(got_from_left != -1)
    {
        return got_from_left;
    }
    
    int result = 0;
    buffer_node node;
    circ_buf_pop(&client2server, &node);
    if (len < node.len) {
        memcpy(buf, node.buffer, len);
        result = len;
        // The length passed in is too small, we have to store some data in the temp
        size_t left_len = node.len - len;
        server_read_temp.buf = malloc(left_len);
        server_read_temp.size = left_len;
        server_read_temp.offset = 0;
        memcpy(server_read_temp.buf, node.buffer + len, left_len);
        
        //printf("Error: input buffer is too small\n");
        //result = -1;
    }else{
        memcpy(buf, node.buffer, node.len);
        result = (int)node.len;
    }
    free((void*)node.buffer);
    return result;
}

int client_membuf_send( void *ctx, const unsigned char *buf, size_t len )
{
    buffer_node node;
    char *newbuf = (char *)malloc(len);
    memcpy(newbuf, buf, len);
    node.buffer = (uint8_t *)newbuf;
    node.len = len;
    circ_buf_push(&client2server, node);
    return (int)len;
}

int client_membuf_recv( void *ctx, unsigned char *buf, size_t len )
{
    int got_from_left = get_from_left_buffer(&client_read_temp, buf, len);
    if(got_from_left != -1)
    {
        return got_from_left;
    }
    
//    // Check any data left first
//    size_t left_len = client_read_temp.size - client_read_temp.offset;
//    if(left_len > 0 )
//    {
//        size_t len_to_copy = left_len;
//        if (left_len > len)
//        {
//            len_to_copy = len;
//        }
//        memcpy(buf, client_read_temp.buf + client_read_temp.offset, len_to_copy);
//        if (len_to_copy == left_len)
//        {   // all data are got
//            free(client_read_temp.buf);
//            client_read_temp.offset = 0;
//            client_read_temp.size = 0;
//        }else{
//            client_read_temp.offset += len_to_copy;
//        }
//        return (int)len_to_copy;
//    }
    
    int result = 0;
    buffer_node node;
    circ_buf_pop(&server2client, &node);
    if (len < node.len)
    {
        memcpy(buf, node.buffer, len);
        result = len;
        // The length passed in is too small, we have to store some data in the temp
        size_t left_len = node.len - len;
        client_read_temp.buf = malloc(left_len);
        client_read_temp.size = left_len;
        client_read_temp.offset = 0;
        memcpy(client_read_temp.buf, node.buffer + len, left_len);
        
        //printf("Error: input buffer is too small\n");
        //result = -1;
    }else{
        memcpy(buf, node.buffer, node.len);
        result = (int)node.len;
    }
    free((void*)node.buffer);
    return result;
}

pthread_t serverThread;

void startServerThread()
{
    int res = pthread_create(&serverThread, NULL, (void *(*)(void *))ssl_server_main, NULL);
    
    if (res != 0) {
        printf("pthread_create server error\n");
    }
    
}


pthread_t clientThread;

void startClientThread()
{
    int res = pthread_create(&clientThread, NULL, (void *(*)(void *))ssl_client1_main, NULL);
    
    if (res != 0) {
        printf("pthread_create client error\n");
    }
    
}


void ring_buffer_test()
{
    CIRCBUF_DEF(ring_buf, 4);
    buffer_node node={NULL,3};
    circ_buf_push(&ring_buf, node);
    node.len = 5;
    circ_buf_push(&ring_buf, node);
    node.len = 6;
    circ_buf_push(&ring_buf, node);
    buffer_node node_out;
    circ_buf_pop(&ring_buf, &node_out);
    if (node_out.len == 3) {
        printf("pop a node right\n");
    }
    circ_buf_pop(&ring_buf, &node_out);
    if (node_out.len == 5) {
        printf("pop a node right\n");
    }
    circ_buf_pop(&ring_buf, &node_out);
    if (node_out.len == 6) {
        printf("pop a node right\n");
    }
}


void startFuzz(void)
{
    void *ret;

    //ring_buffer_test();
    
    startServerThread();
    startClientThread();
    
    pthread_join(clientThread, &ret);
    pthread_join(serverThread, &ret);
}
