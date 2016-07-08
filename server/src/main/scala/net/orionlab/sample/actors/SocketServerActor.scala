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

object SocketServerActor {
  def props(host: String, port: Int) = {
    Props(new SocketServerActor(host, port))
  }
}

private class SocketServerActor(host: String, port: Int) extends Actor with ActorLogging {

  import akka.io.{IO, Tcp}
  import Tcp._
  import java.net.InetSocketAddress
  import context.system

  log.info("SocketServerActor created\nTrying to bind socket.")
  IO(Tcp) ! Bind(self, new InetSocketAddress(host, port))

  def receive = {
    case Bound(localAddress) => log.info(s"Server listening on $localAddress")

    case CommandFailed(_: Bind) => context stop self

    case Connected(remote, local) =>
      val sb = remote.getAddress.getAddress.addString(new StringBuilder(), "tcp-", "_", "x").append(remote.getPort)
      val actorName = local.getAddress.getAddress.addString(sb, "-", "_", "x").append(local.getPort).toString()
      val connection = sender()
      val handler = context.actorOf(SocketClientActor.props(connection), actorName)
      connection ! Register(handler)

    case any => log.info(s">>> Unhandled message '$any'")
  }
}
