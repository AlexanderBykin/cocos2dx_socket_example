//
// source taken from: blog.csdn.net/zzhboy/article/details/9878941
//
// Portable socket class.
// description: this sock is suit both windows and linux
// design: odison
// e-mail: odison@126.com
//

#ifndef _ODSOCKET_H_
#define _ODSOCKET_H_

#ifdef WIN32
	#include <winsock2.h>
	typedef int				socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
    #include <string>
	typedef int				SOCKET;

	//#pragma region define win32 const variable in linux
	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
	//#pragma endregion
#endif

class ODSocket {

public:
    // Create socket object for snd/recv data
    ODSocket(int af, int type, int protocol = 0);
    ~ODSocket();

	// Connect socket
	bool Connect(const char* ip, unsigned short port);
	
    #pragma region server
	// Bind socket
	bool Bind(unsigned short port);

	// Listen socket
	bool Listen(int backlog = 5);

	// Accept socket
	bool Accept(ODSocket& s, char* fromip = NULL);
	#pragma endregion
	
    int Select();
	// Send socket
	int Send(const char* buf, int len, int flags = 0);

    int Send(const std::string &data);
    
	// Recv socket
	long Recv(char* buf, int len, int flags = 0);

    bool isConnected();
    
	// Close socket
	int Close();

	// Get errno
	int GetError();

	#pragma region just for win32
	// Init winsock DLL
	static int Init();
	// Clean winsock DLL
	static int Clean();
	#pragma endregion

    bool hasInternetConnection(const char* host, int port);

	ODSocket& operator = (SOCKET s);

	operator SOCKET ();

protected:
    SOCKET _sock;
    fd_set _fdR;
    bool _isConnected = false;
};

#endif
