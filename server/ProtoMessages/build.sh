#!/bin/bash

rm -d -R ../../client/Classes/ProtoMessages/*
rm -d -R ../src/main/java/ProtoMessages/*

for f in *.proto
do
/usr/local/lib/platform/i386-sim/bin/protoc --java_out=../src/main/java/ "$f";
/usr/local/lib/platform/i386-sim/bin/protoc --cpp_out=../../client/Classes/ProtoMessages/ "$f";
done