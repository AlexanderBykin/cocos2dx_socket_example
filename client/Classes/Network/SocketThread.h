//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#ifndef __SocketThread_H__
#define __SocketThread_H__

#include "ODSocket.h"
#include "SocketResponseThreadDelegate.h"
#include <string>
#include <pthread.h>

class SocketThread {
public:
    enum SocketState {
        ssInit,
        ssFailed
    };
    ~SocketThread(void);
    static SocketThread* getInstance();
    int start();
    ODSocket* getSocket();
    void setSocket(ODSocket* socket);
    SocketState getSocketState();
    void setSocketState(SocketState state);
    void stop();
    void setDelegate(SocketResponseThreadDelegate *delegate);
private:
    static void* start_thread(void *);
    pthread_t _pid;
    SocketState _socketState;
    ODSocket *_socket;
    SocketThread(void);
};

#endif