//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//
// Portable socket class.
// description: this sock is suit both windows and linux
// design: odison
// e-mail: odison@126.com
//

#include <stdio.h>
#include "ODSocket.h"

#ifdef WIN32
#pragma comment(lib, "wsock32")
#endif

ODSocket::ODSocket(SOCKET sock) {
	m_sock = sock;
}

ODSocket::~ODSocket() {
}

int ODSocket::Init() {
#ifdef WIN32
	/*
	 http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx

	 typedef struct WSAData {
	 WORD wVersion;								//winsock version
	 WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
	 char szDescription[WSADESCRIPTION_LEN+1];
	 char szSystemStatus[WSASYSSTATUS_LEN+1];
	 unsigned short iMaxSockets;
	 unsigned short iMaxUdpDg;
	 char FAR * lpVendorInfo;
	 }WSADATA, *LPWSADATA;
	 */
	WSADATA wsaData;
	//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8))
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData); //win sock start up
	if (ret) {
//		cerr << "Initilize winsock error !" << endl;
		return -1;
	}
#endif

	return 0;
}
//this is just for windows
int ODSocket::Clean() {
#ifdef WIN32
	return (WSACleanup());
#endif
	return 0;
}

ODSocket& ODSocket::operator =(SOCKET s) {
	m_sock = s;
	return (*this);
}

ODSocket::operator SOCKET() {
	return m_sock;
}
//create a socket object win/lin is the same
// af:
bool ODSocket::Create(int af, int type, int protocol) {
	m_sock = socket(af, type, protocol);
	return (m_sock != INVALID_SOCKET);
}

bool ODSocket::Connect(const char* ip, unsigned short port) {
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip);
	svraddr.sin_port = htons(port);
	int ret = connect(m_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	return (ret != SOCKET_ERROR);
}

bool ODSocket::Bind(unsigned short port) {
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt))	< 0)
        return false;

	int ret = bind(m_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	return (ret != SOCKET_ERROR);
}
//for server
bool ODSocket::Listen(int backlog) {
	int ret = listen(m_sock, backlog);
	return (ret != SOCKET_ERROR);
}

bool ODSocket::Accept(ODSocket& s, char* fromip) {
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(m_sock, (struct sockaddr*) &cliaddr, &addrlen);
	if (sock == SOCKET_ERROR) {
		return false;
	}

	s = sock;
	if (fromip != NULL)
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

int ODSocket::Select() {
    if(m_sock == INVALID_SOCKET) {
        return -1;
    }
    
	FD_ZERO(&fdR);
	FD_SET(m_sock, &fdR);
	struct timeval mytimeout;
	mytimeout.tv_sec = 3;
	mytimeout.tv_usec = 0;
	int result = select(m_sock + 1, &fdR, NULL, NULL, NULL);
	if(result == -1) {
		return -1;
	} else {
		if(FD_ISSET(m_sock, &fdR)) {
			return -2;
		} else {
			return -3;
		}
	}
}

int ODSocket::Send(const char* buf, int len, int flags) {
    if(!isConnected()) {
        return 0;
    }

    int bytes;
	int count = 0;

	while (count < len) {
		bytes = send(m_sock, buf + count, len - count, flags);
		if (bytes == -1 || bytes == 0)
			return -1;
		count += bytes;
	}

	return count;
}

int ODSocket::Send(const std::string &data) {
    int len = data.size();
    auto buf = data.c_str();
    return this->Send(buf, len, 0);
}

int ODSocket::Recv(char* buf, int len, int flags) {
	return recv(m_sock, buf, len, flags);
}

bool ODSocket::isConnected() {
    return m_sock != INVALID_SOCKET;
}

int ODSocket::Close() {
#ifdef WIN32
	return closesocket(m_sock);
#else
	return close(m_sock);
#endif
}

int ODSocket::GetError() {
#ifdef WIN32
	return (WSAGetLastError());
#else
	return -1;
#endif
}

bool ODSocket::DnsParse(const char* domain, char* ip) {
	struct hostent* p;
	if ((p = gethostbyname(domain)) == NULL)
		return false;

	sprintf(ip, "%u.%u.%u.%u", (unsigned char) p->h_addr_list[0][0],
			(unsigned char) p->h_addr_list[0][1],
			(unsigned char) p->h_addr_list[0][2],
			(unsigned char) p->h_addr_list[0][3]);

	return true;
}
