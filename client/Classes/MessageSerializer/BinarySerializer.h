//
//  BinarySerializer.h
//  BrrClient
//
//  Created by Александр Шниперсон on 20.08.15.
//
//

#ifndef __BrrClient__BinarySerializer__
#define __BrrClient__BinarySerializer__

#include "IMessageSerializer.h"

class BinarySerializer: public IMessageSerializer {
    const std::string serialize(MessageRequest &message);
    MessageResponse *deserialize(const std::string &data);
};

#endif /* defined(__BrrClient__BinarySerializer__) */
