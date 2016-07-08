// The MIT License (MIT)
//
// Copyright (c) 2015 Alexander Shniperson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef _ProtoMessageDelegate_h_
#define _ProtoMessageDelegate_h_

#include "ProtoMessages/MessageRequest.pb.h"
#include "ProtoMessages/MessageResponse.pb.h"
#include "MessageSerializer/BaseMessageSerializer.h"
#include "MessageSerializer/BinarySerializer.h"

class ProtoMessageDelegate {
public:
    static const int ProtoMessageSceneID = 999;

    /*
     * This method is enqueue protobuf messages called by scheduler
     */
    void enqueuProtoMessage() {
        auto message = _protoMessageQueue.front();
        _protoMessageQueue.pop();
        processProtoMessage(message.messagetype(), message.messagebody());
    }
    
    /*
     * This method place all incoming messages into queue because cocos2dx OpenGL is not thread safe,
     * so we need to care about that and synchronize with cocos2dx trhead
     * put messages into queue and then enqueue with cocos2dx Scheduler
     */
    void onProtoMessageReceive(const std::string &data) {
        try {
            MessageResponse message = _serializer->deserialize(data);
            _protoMessageQueue.push(message);
            cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(CC_CALLBACK_0(ProtoMessageDelegate::enqueuProtoMessage, this));
        } catch (...) {
        }
    }
    
    /*
     * This method must be implemented in Every scene that uses communication with sockets
     */
    virtual void processProtoMessage(const eCommunicationMessageType messageType, const std::string &messageBody) = 0;
    
    /*
     * This method just wrap our message to MessageRequest and serialize to byte array
     */
    const std::string wrapMessage(eCommunicationMessageType msgType, google::protobuf::MessageLite &msgBody) {
        MessageRequest msg;
        msg.set_messagetype(msgType);
        msg.set_messagebody(msgBody.SerializeAsString());
        return _serializer->serialize(msg);
    }
private:
    BaseMessageSerializer *_serializer = new BinarySerializer();
    std::queue<MessageResponse> _protoMessageQueue;
};

#endif
