Example of usage Socket at Cocos2dx

# About
I create an online multiplayer game, but with network programming I have bad, long searching around the internet ready code to use socket on the mobile device tried many different solutions, until I met a good man named Odison ( I hope that's your real name :) ) he sent me to a good example of a ready source code, and it is now here, I have added the implementation of the server and you can use it as you wish.

# Setup
1. download and install Java JDK >= 1.7 http://www.oracle.com/technetwork/java/javase/downloads/index.html
2. download and install Maven https://maven.apache.org/
3. download and install Gradle http://gradle.org/
4. clone Cocos2dx https://github.com/cocos2d/cocos2d-x
5. create liked directory of Cocos2dx to '/opt/cocos2dx'
6. clone Protobuf https://github.com/google/protobuf, the working commit is [7ecc19f] because latest have bug at Lite version of Java sources
7. (optional) build Protobuf protoc compiler
8. copy build-protobuf-iphonesim.sh into Protobuf directory and run it, this file builds static library of Protobuf, in my case it is plased to '/usr/local/lib/platform/x86_64-sim/lib/'
9. create linked directory of Protobuf into '/opt/protobuf' 
10. build Protobuf for Java (it is because at socket server we use Java) goto protobuf/java and run 'mvn install -P lite -Dmaven.test.skip' then 'mvn install:install-file -Dfile=./target/protobuf-java-3.0.0-alpha-4-pre.jar -Dclassifier=lite'
11. goto server and run 'gradle installApp' this builds socket server
12. run socket server from 'server/build/install/socket_server/bin/socket_server'
13. run socket client from 'client/proj.ios_mac/SocketClient.xcodeproj'

Currently i setup client only for Apple iOS, send me pull request for Android.
