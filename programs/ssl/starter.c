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


void startFuzz()
{
    startServerThread();
    startClientThread();
}
