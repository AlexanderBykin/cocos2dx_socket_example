//
//  BinarySerializer.cpp
//  BrrClient
//
//  Created by Александр Шниперсон on 20.08.15.
//
//

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
    MessageResponse *builder;

    if (sizeof(data) == 0) {
        builder->set_messagetype(eCommunicationMessageType::cmtNone);
        builder->set_messagebody(new char[0]);
        
        return builder;
    }
    
    builder->ParseFromString(data);
    return builder;
}
