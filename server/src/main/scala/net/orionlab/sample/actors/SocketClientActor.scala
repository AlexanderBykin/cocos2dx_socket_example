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

import com.google.protobuf.MessageLite.Builder

object SocketClientActor {

  import net.orionlab.sample.actors.FrameBuilderActor.{CompleteMessage, BuildFrame}
  import akka.actor.{Props, ActorLogging, Actor, ActorRef}
  import akka.io.Tcp
  import ProtoMessages.MessageRequestBase.MessageRequest
  import ProtoMessages.MessageResponseBase.MessageResponse
  import net.orionlab.sample.network.BinarySerializer
  import com.google.protobuf.ByteString
  import scala.util._
  import ProtoMessages.CommunicationMessageTypeBase.eCommunicationMessageType
  import ProtoMessages.MessageResponsePongBase

  case class IncomingMessage(message: MessageRequest)

  case class OutgoingMessage(message: MessageResponse)

  def props(connection: ActorRef) = {
    Props(new SocketClientActor(connection))
  }

  class SocketClientActor(connection: ActorRef) extends Actor with ActorLogging {

    import Tcp._

    val actorId = System.nanoTime()
    val frameBuilder = context.actorOf(FrameBuilderActor.props(256))
    var userActor: Option[ActorRef] = None

    log.info(s"ClientConnected $connection")

    def receive = {
      case Received(data) =>
        log.info("received request from client")
        frameBuilder ! BuildFrame(data)

      case CompleteMessage(data) =>
        BinarySerializer.Deserialize(data.toArray) match {
          case None => log.info(s"Cant send empty message to UserActor.")
          case Some(message) =>
            self ! IncomingMessage(message)
        }

      case OutgoingMessage(message) =>
        log.info("send response to client ")
        connection ! Write(akka.util.ByteString(BinarySerializer.Serialize(message)))

      case PeerClosed =>
        log.info(s"ClientDisconnect $connection")
        context.stop(self)

      // handle client messages
      case msgReceive: IncomingMessage ⇒
        log.info("received deserialized message")
        Try {
          msgReceive.message.getMessageType match {
            case eCommunicationMessageType.cmtPing =>
              if (msgReceive.message.getMessageBody.isEmpty) {
                log.error(s"${self.path} received empty MessageBody for MessageType='${msgReceive.message.getMessageType.name()}'")
              } else {
                val msgResponse = MessageResponsePongBase.MessageResponsePong.newBuilder()
                msgResponse.setSomeText("Pong from server")
                sendMessageEx(self, eCommunicationMessageType.cmtPong, msgResponse)
              }
            case any => log.error(s"Unhandled messageType($any)")
          }
        } match {
          case Success(x) ⇒
          case Failure(x) ⇒ log.error(x, "")
        }

      case any => log.info(s"Unhandled event $any")
    }

    def sendMessageEx(clientActor: ActorRef, messageType: eCommunicationMessageType, data: Option[Array[Byte]]) {
      Option(clientActor) match {
        case None => log.error(s"Channel can not be null in ${this}''")
        case Some(actor) =>
          val msgResponse = MessageResponse.newBuilder()
          msgResponse.setMessageType(messageType)
          msgResponse.setMessageBody(if (data.isDefined && data.get.length > 0) ByteString.copyFrom(data.get) else ByteString.EMPTY)
          msgResponse.setErrorCode(0)
          actor ! OutgoingMessage(msgResponse.build())
      }
    }

    def sendMessageEx(clientActor: ActorRef, messageType: eCommunicationMessageType, response: Builder) {
      sendMessageEx(clientActor, messageType, if (response == null) None else Some(response.build().toByteArray))
    }

  }

}

