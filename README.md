Example of usage Socket at Cocos2d-x >= v3.8.1

# About
I create an online multiplayer game, but in network programming i am not so good.
Long time searching around the Internet for example usage sockets at mobile devices, i tried many different solutions that does not work until i met a good man named Odison ( I hope that's your real name :) ) he sent me a good example of source code, i little bit refactored it and now i share it here, i have added implementation of the server and you can use it as you wish.

# Setup
1. download and install Java JDK >= 1.8 http://www.oracle.com/technetwork/java/javase/downloads/index.html
2. download and install Maven https://maven.apache.org/
3. download and install Gradle http://gradle.org/
4. clone Cocos2dx https://github.com/cocos2d/cocos2d-x and then just copy it into client folder as cocos2d
5. clone Protobuf https://github.com/google/protobuf
6. (optional) build Protobuf protoc compiler
   copy build-protobuf-iphonesim.sh into Protobuf directory and run it, this file builds static library for iOS, in my case it is placed at '/usr/local/platform/i386-sim/lib/'
   create symbolic link to Protoc compiler from '/usr/local/platform/i386-sim/bin/protoc' into '/usr/local/bin/protoc'
9. build Protobuf for Java (it is because at socket server we use Protobuf Java) goto Protobuf/java/lite and run 'mvn install'
10. goto server and run 'gradle installDist' this builds socket server
11. run socket server from 'server/build/install/socket_server/bin/socket_server'
12. run socket client from 'client/proj.ios_mac/SocketClient.xcodeproj'

Sockets is crossplatform and works fine on Apple iOS and Google Android.