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
	_sock = sock;
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
	_sock = s;
	return (*this);
}

ODSocket::operator SOCKET() {
	return _sock;
}
//create a socket object win/lin is the same
// af:
bool ODSocket::Create(int af, int type, int protocol) {
	_sock = socket(af, type, protocol);
	return (_sock != INVALID_SOCKET);
}

bool ODSocket::Connect(const char* hostname, unsigned short port) {
    struct hostent *he;
	struct sockaddr_in svraddr;
    he = gethostbyname(hostname);
    memcpy(&svraddr.sin_addr, he->h_addr_list[0], he->h_length);
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(port);
	int ret = connect(_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
    _isConnected = (ret != SOCKET_ERROR);
    return _isConnected;
}

bool ODSocket::Bind(unsigned short port) {
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt))	< 0)
        return false;

	int ret = bind(_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	return (ret != SOCKET_ERROR);
}
//for server
bool ODSocket::Listen(int backlog) {
	int ret = listen(_sock, backlog);
	return (ret != SOCKET_ERROR);
}

bool ODSocket::Accept(ODSocket& s, char* fromip) {
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(_sock, (struct sockaddr*) &cliaddr, &addrlen);
	if (sock == SOCKET_ERROR) {
		return false;
	}

	s = sock;
	if (fromip != NULL)
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

int ODSocket::Select() {
    if(_sock == INVALID_SOCKET) {
        return -1;
    }
    
	FD_ZERO(&_fdR);
	FD_SET(_sock, &_fdR);
	struct timeval mytimeout;
	mytimeout.tv_sec = 3;
	mytimeout.tv_usec = 0;
	int result = select(_sock + 1, &_fdR, NULL, NULL, NULL);
	if(result == -1) {
		return -1;
	} else {
		if(FD_ISSET(_sock, &_fdR)) {
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
		bytes = send(_sock, buf + count, len - count, flags);
		if (bytes == -1 || bytes == 0)
			return -1;
		count += bytes;
	}

    printf("SocketClient::Send(%d)\n", count);

	return count;
}

int ODSocket::Send(const std::string &data) {
    int len = data.size();
    auto buf = data.c_str();
    return this->Send(buf, len, 0);
}

int ODSocket::Recv(char* buf, int len, int flags) {
	return recv(_sock, buf, len, flags);
}

bool ODSocket::isConnected() {
    return _isConnected;
}

int ODSocket::Close() {
    _isConnected = false;
#ifdef WIN32
	return closesocket(m_sock);
#else
	return close(_sock);
#endif
    _sock = INVALID_SOCKET;
}

int ODSocket::GetError() {
#ifdef WIN32
	return (WSAGetLastError());
#else
	return -1;
#endif
}

bool ODSocket::DnsParse(const char* domain) {
	struct hostent* p;
	if ((p = gethostbyname(domain)) == NULL)
		return false;

	return true;
}
