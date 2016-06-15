//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#include "SocketThread.h"
#include "SocketResponseThread.h"
#include "cocos2d.h"

SocketThread* _instance = nullptr;

SocketThread* SocketThread::getInstance() {
    if(_instance == nullptr) {
        _instance = new SocketThread();
    }
    return _instance;
}

SocketThread::SocketThread(void) {
    this->setSocket(new ODSocket(AF_INET, SOCK_STREAM, 0));
}

int SocketThread::start() {
    int errCode = 0;
    do {
        pthread_attr_t tAttr;
        errCode = pthread_attr_init(&tAttr);
        errCode = pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
        if (errCode != 0) {
            pthread_attr_destroy(&tAttr);
            break;
        }
        errCode = pthread_create(&_pid, &tAttr, start_thread, nullptr);
    } while (0);
    return errCode;
}

void* SocketThread::start_thread(void *arg) {
    auto thread = SocketThread::getInstance();
    bool isConnected = thread->getSocket()->Connect("127.0.0.1", 15150);
    if(isConnected) {
        thread->setSocketState(SocketState::ssInit);
        ResponseThread::getInstance()->start();
    } else {
        thread->setSocketState(SocketState::ssFailed);
    }
    return nullptr;
}

SocketThread::SocketState SocketThread::getSocketState() {
    return _socketState;
}

void SocketThread::setSocketState(SocketState state) {
    _socketState = state;
}

ODSocket* SocketThread::getSocket() {
    return this->_socket;
}

void SocketThread::setSocket(ODSocket* socket) {
    this->_socket = socket;
}

void SocketThread::stop() {
#if CC_TARGET_PLATFORM && defined(ANDROID)
    pthread_kill(_pid, SIGUSR1);
#else
    pthread_cancel(_pid);
#endif
    pthread_detach(_pid);
}

SocketThread::~SocketThread(void) {
    if(_instance != nullptr) {
        delete _instance;
        _instance = nullptr;
    }
}

void SocketThread::setDelegate(SocketResponseThreadDelegate *delegate) {
    ResponseThread::getInstance()->setDelagate(delegate);
}
