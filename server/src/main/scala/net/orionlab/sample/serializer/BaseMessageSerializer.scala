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

package net.orionlab.sample.serializer

/**
  * Base trait for Serialize and Deserialize of Protobuf objects
  */
trait BaseMessageSerializer {

  import ProtoMessages.MessageResponseBase.MessageResponse
  import ProtoMessages.MessageRequestBase.MessageRequest
  import org.slf4j.LoggerFactory

  protected val log = LoggerFactory.getLogger(getClass.getName)

  /**
    * Serialize protobuf object to byte array
    *
    * @param message MessageResponse that need to be serialized to byte array
    * @return Some Array[Byte] if message successful serialized else None
    */
  def serialize(message: MessageResponse): Option[Array[Byte]]

  /**
    * Deserialize protobuf object from byte array
    *
    * @param message byte array represents protobuf object
    * @return Some MessageResponse if message successful deserialized else None
    */
  def deserialize(message: Array[Byte]): Option[MessageRequest]
}
