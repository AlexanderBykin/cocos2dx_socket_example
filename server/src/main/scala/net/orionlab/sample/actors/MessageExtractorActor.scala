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

package net.orionlab.sample.actors

import akka.actor.{ActorLogging, Actor, Props}

/**
  * Message body extractor from frames that came from TCP by length
  * Note: Akka.TCP itself trying to make it possible to pack frames as it sends, but that is not guarantied, so we
  * need to care of that.
  */
object MessageExtractorActor {

  /**
    * Frames as array of bytes that came from TCP client socket
    *
    * @param data byte array
    */
  case class ExtractMessage(data: Array[Byte])

  /**
    * Complete extracted message body by length
    *
    * @param data message body that next will be deserialized
    */
  case class CompleteMessage(data: Array[Byte])

  def props(bufferSize: Int) = {
    Props(new MessageExtractorActor(bufferSize))
  }
}

private class MessageExtractorActor(bufferSize: Int) extends Actor with ActorLogging {

  import java.nio.{ByteBuffer, ByteOrder}
  import MessageExtractorActor.{ExtractMessage, CompleteMessage}

  var messageLength = 0

  def receive = {
    case ExtractMessage(data) => extractMessage(data)
  }

  def extractMessage(data: Array[Byte]) {
    if (data.isEmpty) {
      messageLength = 0
    } else {
      var tmpData = data
      if (messageLength == 0 && tmpData.length >= 4) {
        messageLength = ByteBuffer.wrap(tmpData.take(4)).order(ByteOrder.LITTLE_ENDIAN).getInt
        tmpData = tmpData.drop(4)
      }
      if (tmpData.length >= messageLength) {
        val res = tmpData.take(messageLength)
        tmpData = tmpData.drop(messageLength)
        context.parent ! CompleteMessage(res)
      }
      messageLength = 0
      if (tmpData.length > 0) {
        self ! ExtractMessage(tmpData)
      }
    }
  }
}

