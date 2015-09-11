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

#include "SceneSample.h"
#include "SocketThread.h"
#include "MessageRequestPing.pb.h"
#include "MessageResponsePong.pb.h"

USING_NS_CC;
using namespace ui;

Scene* SceneSample::scene() {
    auto node = new (std::nothrow) SceneSample();
    if(node && node->init()) {
        auto scene = Scene::create();
        scene->autorelease();
        scene->addChild(node, 999, 999);
        return scene;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool SceneSample::init() {
    if(!Layer::init()) {
        return false;
    }
    
    auto miButton = MenuItemLabel::create(Label::createWithSystemFont("Ping", "Arial", 28), CC_CALLBACK_1(SceneSample::onMenuCallback, this));
    auto menu = Menu::create(miButton, nullptr);
    menu->setPosition(Vec2(100, 100));
    this->addChild(menu);
    
    return true;
}

void SceneSample::onMenuCallback(cocos2d::Ref *ref) {
    this->sendPing();
}

void SceneSample::sendPing() {
    MessageRequestPing msg;
    msg.set_sometext("request send from client");
    int ret = SocketThread::GetInstance()->getSocket().Send(this->wrapMessage(eCommunicationMessageType::cmtPing, msg));
    log("send ping message to server(%d)", ret);
}

void SceneSample::onProtoMessageReceive(MessageResponse &message) {
    printf("response\n");
    switch (message.messagetype()) {
        case eCommunicationMessageType::cmtPong: {
            if(!message.messagebody().empty()) {
                MessageResponsePong msg;
                msg.ParseFromString(message.messagebody());
                log("response from server '%s'", msg.sometext().c_str());
            } else {
                log("Error: Received empty response body.");
            }
            break;
        }
            
        default:
            break;
    }
}
