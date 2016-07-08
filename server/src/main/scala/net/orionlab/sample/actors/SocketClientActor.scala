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

import akka.actor.{Actor, ActorLogging, ActorRef, Props, Terminated}
import ProtoMessages.MessageRequestBase.MessageRequest
import ProtoMessages.MessageResponseBase.MessageResponse

object SocketClientActor {

  case class IncomingMessage(message: MessageRequest)

  case class OutgoingMessage(message: MessageResponse)

  def props(connection: ActorRef) = {
    Props(new SocketClientActor(connection))
  }
}

private class SocketClientActor(connection: ActorRef) extends Actor with ActorLogging {

  import akka.io.Tcp
  import Tcp._
  import com.google.protobuf.MessageLite.Builder
  import net.orionlab.sample.serializer.BinarySerializer
  import com.google.protobuf.ByteString
  import scala.util._
  import net.orionlab.sample.actors.SocketClientActor.IncomingMessage
  import ProtoMessages.CommunicationMessageTypeBase.eCommunicationMessageType
  import ProtoMessages.MessageResponsePongBase
  import net.orionlab.sample.actors.MessageExtractorActor.{CompleteMessage, ExtractMessage}

  val dateFormatter = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss")
  val actorId = System.nanoTime()
  val messageExtractor = context.actorOf(MessageExtractorActor.props(256))
  var userActor = Option.empty[ActorRef]

  log.info(s"ClientConnected $connection")
  context.watch(connection)

  def receive = {
    case Received(data) =>
      log.info("received request from client")
      messageExtractor ! ExtractMessage(data.toArray)

    case CompleteMessage(data) =>
      BinarySerializer.deserialize(data) match {
        case None => log.warning(s"Cant send empty message to UserActor.")
        case Some(message) =>
          self ! IncomingMessage(message)
      }

    case PeerClosed =>
      log.info(s"ClientDisconnect $connection")
      context.stop(self)

    /**
      * stop self when remote terminated
      */
    case msg: Terminated => context.stop(self)

    /**
      * Handle messages came from client socket
      */
    case msgReceive: IncomingMessage ⇒
      log.info("received deserialized message")
      Try {
        msgReceive.message.getMessageType match {
          case eCommunicationMessageType.cmtPing =>
            if (msgReceive.message.getMessageBody.isEmpty) {
              log.error(s"${self.path} received empty MessageBody for MessageType='${msgReceive.message.getMessageType.name()}'")
            } else {
              val msgResponse = MessageResponsePongBase.MessageResponsePong.newBuilder()
              msgResponse.setSomeText("Pong from server %s".format(dateFormatter.format(new java.util.Date)))
              sendSocketMessage(eCommunicationMessageType.cmtPong, msgResponse)
            }
          case any => log.warning(s"Unhandled messageType($any)")
        }
      } match {
        case Success(x) ⇒
        case Failure(x) ⇒ log.error(x, "")
      }

    case any => log.warning(s"Unhandled message <$any>")
  }

  def sendSocketMessage(messageType: eCommunicationMessageType, data: Option[Array[Byte]]) {
    Option(connection) match {
      case None => log.error(s"Channel can not be null in '${getClass.getName}'")
      case Some(actor) =>
        val msgResponse = MessageResponse.newBuilder()
        msgResponse.setMessageType(messageType)
        msgResponse.setMessageBody(if (data.isDefined && data.get.length > 0) ByteString.copyFrom(data.get) else ByteString.EMPTY)
        msgResponse.setErrorCode(0)
        BinarySerializer.serialize(msgResponse.build()).foreach {
          serializedMessage => actor ! Write(akka.util.ByteString(serializedMessage))
        }
    }
  }

  def sendSocketMessage(messageType: eCommunicationMessageType, builder: Builder) {
    sendSocketMessage(messageType, if (builder == null) None else Some(builder.build().toByteArray))
  }
}

