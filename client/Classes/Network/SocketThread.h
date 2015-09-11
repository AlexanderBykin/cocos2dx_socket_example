//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#ifndef __SocketThread_H__
#define __SocketThread_H__

#include "ODSocket.h"
#include "pthread.h"
#include "SocketResponseThreadDelegate.h"

class SocketThread {
public:	
	~SocketThread(void);
	static SocketThread* GetInstance();
	int start();  
	ODSocket getSocket();
	int state;
	void stop();
    void setDelegate(SocketResponseThreadDelegate *delegate);
private:
	pthread_t pid;	
	static void* start_thread(void *); 	
	SocketThread(void);
private:
    ODSocket csocket;
	static SocketThread* m_pInstance;
};

#endif