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
#include "ProtoMessages/MessageRequestPing.pb.h"
#include "ProtoMessages/MessageResponsePong.pb.h"

using namespace ui;

Scene* SceneSample::scene() {
    auto node = new (std::nothrow) SceneSample();
    if(node && node->init()) {
        auto scene = Scene::create();
        scene->autorelease();
        scene->addChild(node, 999, ProtoMessageDelegate::ProtoMessageSceneID);
        return scene;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool SceneSample::init() {
    if(!LayerColor::initWithColor(Color4B(0, 120, 0, 255))) {
        return false;
    }
    
    auto miButton = MenuItemLabel::create(Label::createWithSystemFont("Click Me!", "Arial", 28), CC_CALLBACK_1(SceneSample::onMenuCallback, this));
    auto menu = Menu::create(miButton, nullptr);
    menu->setPosition(Vec2(100, 100));
    this->addChild(menu);
    
    if (!SocketThread::getInstance()->getSocket()->isConnected()) {
        SocketThread::getInstance()->start();
    }
    
    return true;
}

void SceneSample::onMenuCallback(cocos2d::Ref *ref) {
    this->sendPing();
}

// api calls

void SceneSample::sendPing() {
    this->removeChildByTag(10);
    MessageRequestPing msg;
    msg.set_sometext("request send from client");
    int ret = SocketThread::getInstance()->getSocket()->Send(this->wrapMessage(eCommunicationMessageType::cmtPing, msg));
    log("send ping message to server(%d)", ret);
}

// api callbacks

void SceneSample::processProtoMessage(const eCommunicationMessageType messageType, const std::string &messageBody) {
    switch (messageType) {
        case eCommunicationMessageType::cmtPong: {
            this->callbackPong(messageBody);
            break;
        }
            
        default:
            break;
    }
}

void SceneSample::callbackPong(const std::string &data) {
    if(data.empty()) {
        log("Error: Received empty response body.");
        return;
    }
    
    MessageResponsePong msg;
    msg.ParseFromString(data);
    
    Size size = Director::getInstance()->getVisibleSize();
    auto lbl = Label::createWithSystemFont(msg.sometext(), "Arial", 35);
    lbl->setPosition(Vec2(size.width/2, size.height/2));
    this->addChild(lbl, 10, 10);
    
    log("response from server '%s'", msg.sometext().c_str());
}
