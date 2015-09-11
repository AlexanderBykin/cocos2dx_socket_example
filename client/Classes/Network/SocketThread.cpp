//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#include "SocketThread.h"
#include "SocketResponseThread.h"

int SocketThread::start() {
	int errCode = 0;
	do {
		pthread_attr_t tAttr;
		errCode = pthread_attr_init(&tAttr);
		assert(errCode == 0);
		errCode = pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
		if (errCode != 0) {
			pthread_attr_destroy(&tAttr);
			break;
		}		
		errCode = pthread_create(&pid, &tAttr, start_thread, this);
	} while (0);
	return errCode;
} 

void* SocketThread::start_thread(void *arg) {
	SocketThread* thred = (SocketThread*)arg;
	ODSocket cdSocket;
	cdSocket.Init();	
	bool isCreated = cdSocket.Create(AF_INET, SOCK_STREAM, 0);
	bool isConnected = cdSocket.Connect("192.168.1.36", 15155);
    thred->csocket = cdSocket;
	if(isCreated && isConnected) {
		thred->state = 0;
		ResponseThread::GetInstance()->start();
	} else {
		thred->state = 1;
	}	
	return NULL;
}

ODSocket SocketThread::getSocket() {
	return this->csocket;
}

SocketThread* SocketThread::m_pInstance = new SocketThread;

SocketThread* SocketThread::GetInstance() {
	return m_pInstance;
}

void SocketThread::stop() {
	pthread_cancel(pid);
	pthread_detach(pid); 
}

SocketThread::SocketThread(void) {

}

SocketThread::~SocketThread(void) {
	if(m_pInstance != NULL) {
		delete m_pInstance;
	}
}

void SocketThread::setDelegate(SocketResponseThreadDelegate *delegate) {
    ResponseThread::GetInstance()->setDelagate(delegate);
}
