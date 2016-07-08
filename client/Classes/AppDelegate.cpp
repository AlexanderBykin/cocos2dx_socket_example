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

#include "AppDelegate.h"
#include "ProtoMessageDelegate.h"
#include "SocketThread.h"
#include "SceneSample.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs() {
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::create("SocketClient");
        director->setOpenGLView(glview);
    }
    
    // Tell cocos our design resolution and our resolution policy
    director->getOpenGLView()->setDesignResolutionSize(1024, 768, ResolutionPolicy::FIXED_HEIGHT);
    
    // Get the actual screen size
    Size frameSize = glview->getFrameSize();
    
    // turn on display FPS
    //director->setDisplayStats(true);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    register_all_packages();
    
    Director::getInstance()->runWithScene(SceneSample::scene());
    
    SocketThread::getInstance()->setDelegate(this);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void AppDelegate::onSocketMessageReceive(const std::string &data) {
    if(auto currentScene = Director::getInstance()->getRunningScene()) {
        if(auto transitionScene = dynamic_cast<TransitionScene*>(currentScene)) {
            if(auto protoScene = dynamic_cast<ProtoMessageDelegate*>(transitionScene->getInScene()->getChildByTag(ProtoMessageDelegate::ProtoMessageSceneID))) {
                protoScene->onProtoMessageReceive(data);
            }
        }
        else if(auto protoScene = dynamic_cast<ProtoMessageDelegate*>(currentScene->getChildByTag(ProtoMessageDelegate::ProtoMessageSceneID))) {
            protoScene->onProtoMessageReceive(data);
        } else {
            log("onSocketMessageReceive ProtoScene is NULL");
        }
    } else {
        log("onSocketMessageReceive ProtoScene is NULL");
    }
}

void AppDelegate::onSocketError(const std::string &msg) {
    CCLOG("%s", msg.c_str());
}
