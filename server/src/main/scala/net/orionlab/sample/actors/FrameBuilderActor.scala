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

object FrameBuilderActor {

  import akka.actor.{ActorLogging, Actor, Props}
  import akka.util.ByteString

  case class BuildFrame(data: ByteString)

  case class CompleteMessage(data: ByteString)

  def props(bufferSize: Int) = {
    Props(new FrameBuilderActor(bufferSize))
  }

  class FrameBuilderActor(bufferSize: Int) extends Actor with ActorLogging {
    private val headerSize = 4
    private var messageLength = 0
    private var messageBuffer = ByteString.empty

    def receive = {
      case BuildFrame(data) => parse(data)
    }

    private def parse(data: ByteString) {
      if (data.isEmpty) {
        messageLength = 0
        messageBuffer = ByteString.empty
      } else {
        if (messageLength == 0) {
          messageLength = data.iterator.getLongPart(headerSize)(java.nio.ByteOrder.LITTLE_ENDIAN).toInt
          val newData = data.drop(headerSize)
          messageBuffer = ByteString.empty
          parse(newData)
        } else {
          val canTakeLen = if (data.length <= bufferSize) data.length else bufferSize
          val needTakeLen = if (messageBuffer.length + canTakeLen > messageLength) messageBuffer.length + canTakeLen - messageLength else canTakeLen
          messageBuffer ++= data.take(needTakeLen)
          if (messageBuffer.length == messageLength) {
            context.parent ! CompleteMessage(messageBuffer)
            messageLength = 0
          }
          val newData = data.drop(needTakeLen)
          parse(newData)
        }
      }
    }
  }

}
