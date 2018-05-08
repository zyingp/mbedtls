//
//  starter.h
//  mbedtls
//
//  Created by ZengYingpei on 2018/3/18.
//  Copyright © 2018年 ZengYingpei. All rights reserved.
//

#ifndef starter_h
#define starter_h

#include <stdio.h>

#define APP_HAS_OWN_MAIN


typedef struct temp_buffer
{
    char *buf;
    size_t size;     // the size of buffer
    size_t offset;   // next to read position
} buffer_t;

int server_membuf_send( void *ctx, const unsigned char *buf, size_t len );
int server_membuf_recv( void *ctx, unsigned char *buf, size_t len );
int client_membuf_send( void *ctx, const unsigned char *buf, size_t len );
int client_membuf_recv( void *ctx, unsigned char *buf, size_t len );

void startFuzz(void);

#endif /* starter_h */
