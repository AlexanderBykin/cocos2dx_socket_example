//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//

#include "SocketResponseThread.h"
#include "SocketThread.h"

ResponseThread* ResponseThread::m_pInstance = new ResponseThread();

ResponseThread* ResponseThread::GetInstance() {
    return m_pInstance;
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
        CC_BREAK_IF(errCode!=0);
        errCode = pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
        if (errCode != 0) {
            pthread_attr_destroy(&attributes);
            break;
        }
        errCode = pthread_create(&handle, &attributes, threadFunc, this);
        started = true;
    } while (0);
    return errCode;
}

void* ResponseThread::threadFunc(void *arg) {
    ResponseThread* thred = (ResponseThread*)arg;
    ODSocket csocket = SocketThread::GetInstance()->getSocket();
    if(SocketThread::GetInstance()->state == 0) {
        while(true) {
            int selectRes = csocket.Select();
            if(selectRes == -2) {
                char recvBuf[4];
                int i = csocket.Recv(recvBuf, 4, 0);
                if (i == 4) {
                    char dc3[4] = {recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3]};
                    int msgBodyLength = *(int*)&dc3[0];
                    char* msgBody = NULL;
                    if(msgBodyLength > 0) {
                        msgBody = new char[msgBodyLength];
                        csocket.Recv(msgBody, msgBodyLength, 0);
                        auto messageResult = std::string(msgBody, msgBodyLength);
                        delete msgBody;
                        if(thred->_delegate) {
                            thred->_delegate->onSocketMessageReceive(messageResult);
                        }
                    }
                } else {
                    if(thred->_delegate) {
                        thred->_delegate->onSocketError("Incorrect message.\n");
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
        pthread_cancel(handle);
        pthread_detach(handle);
        detached = true;
    }
}

void * ResponseThread::wait() {
    void * status = NULL;
    if (started && !detached) {
        pthread_join(handle, &status);
    }
    return status;
}

void ResponseThread::sleep(int secstr) {
    timeval timeout = { secstr / 1000, secstr % 1000};
    select(0, NULL, NULL, NULL, &timeout);
}

void ResponseThread::detach() {
    if (started && !detached) {
        pthread_detach(handle);
    }
    detached = true;
}

void ResponseThread::setDelagate(SocketResponseThreadDelegate *delegate) {
    _delegate = delegate;
}