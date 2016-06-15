//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#include "SocketResponseThread.h"
#include "SocketThread.h"
#include "cocos2d.h"

static ResponseThread* _instance = nullptr;

ResponseThread* ResponseThread::getInstance() {
    if(_instance == nullptr) {
        _instance = new ResponseThread();
    }
    return _instance;
}

ResponseThread::ResponseThread(void) {
    started = detached = false;
}

ResponseThread::~ResponseThread(void) {
    stop();
}

int ResponseThread::start(void * param) {
    int errCode = 0;
    do {
        pthread_attr_t attributes;
        errCode = pthread_attr_init(&attributes);
        errCode = pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
        if (errCode != 0) {
            pthread_attr_destroy(&attributes);
            break;
        }
        errCode = pthread_create(&_handle, &attributes, threadFunc, nullptr);
        started = true;
    } while (0);
    return errCode;
}

void* ResponseThread::threadFunc(void *arg) {
    auto thread = ResponseThread::getInstance();
    auto socket = SocketThread::getInstance()->getSocket();
    if(SocketThread::getInstance()->getSocketState() == SocketThread::SocketState::ssInit) {
        while(true) {
            int selectRes = socket->Select();
            if(selectRes == -2) {
                char recvBuf[4];
                int recvLength = (int) socket->Recv(recvBuf, 4, 0);
                if (recvLength == 4) {
                    char preBodyLength[4] = {recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3]};
                    int msgBodyLength = *(int*)&preBodyLength[0];
                    char* msgBody = NULL;
                    if(msgBodyLength > 0) {
                        msgBody = new char[msgBodyLength];
                        socket->Recv(msgBody, msgBodyLength, 0);
                        auto messageResult = std::string(msgBody, msgBodyLength);
                        delete msgBody;
                        if(thread->_delegate) {
                            thread->_delegate->onSocketMessageReceive(messageResult);
                        }
                    }
                } else {
                    if(thread->_delegate) {
                        thread->_delegate->onSocketError(std::string("Incorrect message."));
                    }
                    break;
                }
            }
        }
    }
    return NULL;
}

void ResponseThread::stop() {
    if (started && !detached) {
#if CC_TARGET_PLATFORM && defined(ANDROID)
        pthread_kill(_handle, SIGUSR1);
#else
        pthread_cancel(_handle);
#endif
        pthread_detach(_handle);
        detached = true;
    }
}

void * ResponseThread::wait() {
    void * status = NULL;
    if (started && !detached) {
        pthread_join(_handle, &status);
    }
    return status;
}

void ResponseThread::sleep(int secstr) {
    timeval timeout = { secstr / 1000, secstr % 1000};
    select(0, NULL, NULL, NULL, &timeout);
}

void ResponseThread::detach() {
    if (started && !detached) {
        pthread_detach(_handle);
    }
    detached = true;
}

void ResponseThread::setDelagate(SocketResponseThreadDelegate *delegate) {
    _delegate = delegate;
}