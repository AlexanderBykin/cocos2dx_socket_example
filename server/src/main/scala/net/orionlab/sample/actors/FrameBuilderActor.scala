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

import com.google.protobuf.CodedInputStream

object FrameBuilderActor {

  import akka.actor.{ActorLogging, Actor, Props}

  case class BuildFrame(data: Array[Byte])

  case class CompleteMessage(data: Array[Byte])

  def props(bufferSize: Int) = {
    Props(new FrameBuilderActor(bufferSize))
  }

  class FrameBuilderActor(bufferSize: Int) extends Actor with ActorLogging {
    private var messageLength = 0

    def receive = {
      case BuildFrame(data) => parse(data)
    }

    private def parse(data: Array[Byte]) {
      if (data.isEmpty) {
        messageLength = 0
      } else {
        val cis = CodedInputStream.newInstance(data)
        if (messageLength == 0) {
          messageLength = cis.readRawLittleEndian32()
          //          log.info(s">>> Message(${data.map("%02X" format _).mkString(" ")})\n" +
          //            s">>> MessageLength(${messageLength})\n" +
          //            s">>> dataLenBefore(${data.length})\n" +
          //            s">>> dataLenAfter(${cis.getBytesUntilLimit})")
        }
        if (cis.getBytesUntilLimit >= messageLength) {
          context.parent ! CompleteMessage(cis.readRawBytes(messageLength))
        }
        messageLength = 0
        if (cis.getBytesUntilLimit > 0) {
          self ! BuildFrame(cis.readRawBytes(cis.getBytesUntilLimit))
        }
      }
    }
  }

}
