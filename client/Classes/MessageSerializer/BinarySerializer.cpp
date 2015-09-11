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

#include "BinarySerializer.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "CommunicationMessageType.pb.h"
#include "MessageRequest.pb.h"
#include "MessageResponse.pb.h"

using namespace google::protobuf::io;

const std::string BinarySerializer::serialize(MessageRequest &message) {
    int resultSize = message.ByteSize() + 4;
    char *buffer = new char [resultSize];
    ArrayOutputStream aos(buffer, resultSize);
    CodedOutputStream outStream(&aos);
    outStream.WriteLittleEndian32(message.ByteSize());
    message.SerializeToCodedStream(&outStream);
    std::string res = std::string(buffer, outStream.ByteCount());
    delete buffer;
    return res;
}

MessageResponse* BinarySerializer::deserialize(const std::string &data) {
    MessageResponse *builder = new MessageResponse();

    if (sizeof(data) == 0) {
        builder->set_messagetype(eCommunicationMessageType::cmtNone);
        builder->set_messagebody(new char[0]);
        
        return builder;
    }
    
    builder->ParseFromString(data);
    return builder;
}
