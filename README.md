Example of usage Socket at Cocos2d-x-3.8.1

# About
I create an online multiplayer game, but in network programming I am bad, long time searching around the internet to use socket on the mobile device, i tried many different solutions, until I met a good man named Odison ( I hope that's your real name :) ) he sent me a good example of a ready source code and now i share it here, I have added the implementation of the server and you can use it as you wish.

# Setup
1. download and install Java JDK >= 1.7 http://www.oracle.com/technetwork/java/javase/downloads/index.html
2. download and install Maven https://maven.apache.org/
3. download and install Gradle http://gradle.org/
4. clone Cocos2dx https://github.com/cocos2d/cocos2d-x and then just copy it into client folder as cocos2d
5. clone Protobuf https://github.com/google/protobuf, the working commit is [7ecc19f] because latest have bug at Lite version of Java sources
6. (optional) build Protobuf protoc compiler
7. copy build-protobuf-iphonesim.sh into Protobuf directory and run it, this file builds static library of Protobuf, in my case it is plased to '/usr/local/lib/platform/x86_64-sim/lib/'
8. create linked directory of Protobuf into '/opt/protobuf' 
9. build Protobuf for Java (it is because at socket server we use Java) goto protobuf/java and run 'mvn install -P lite -Dmaven.test.skip' then 'mvn install:install-file -Dfile=./target/protobuf-java-3.0.0-alpha-4-pre.jar -Dclassifier=lite'
10. goto server and run 'gradle installApp' this builds socket server
11. run socket server from 'server/build/install/socket_server/bin/socket_server'
12. run socket client from 'client/proj.ios_mac/SocketClient.xcodeproj'

Sockets is crossplatform and works fine on Apple iOS and Google Android.