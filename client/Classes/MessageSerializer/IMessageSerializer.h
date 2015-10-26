//
//  IMessageSerializer.h
//  BrrClient
//
//  Created by Александр Шниперсон on 20.08.15.
//
//

#ifndef __BrrClient__IMessageSerializer__
#define __BrrClient__IMessageSerializer__

//#include "MessageRequest.pb.h"
//#include "MessageResponse.pb.h"

#include <string>

class MessageRequest;
class MessageResponse;

class IMessageSerializer {
public:
    virtual const std::string serialize(MessageRequest &message) = 0;
    virtual MessageResponse *deserialize(const std::string &data) = 0;
};

#endif /* defined(__BrrClient__IMessageSerializer__) */
