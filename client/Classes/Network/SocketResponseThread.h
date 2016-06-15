//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#ifndef __ResponseThread_H__
#define __ResponseThread_H__

#include <pthread.h>
#include "SocketResponseThreadDelegate.h"

class ResponseThread {
public:
    ~ResponseThread(void);
    static ResponseThread* getInstance();
    int start (void* = NULL);
    void stop();
    void sleep (int tesec);
    void detach();
    void* wait();
    void setDelagate(SocketResponseThreadDelegate *delegate);
private:
    ResponseThread(void);
    pthread_t _handle;
    bool started;
    bool detached;
    static void* threadFunc(void *);
    SocketResponseThreadDelegate *_delegate;
};

#endif