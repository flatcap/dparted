#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include "socket.h"

class ServerSocket : private Socket
{
public:
	ServerSocket (int port);
	ServerSocket ();
	virtual ~ServerSocket();

	const ServerSocket& operator << (const std::string&) const;
	const ServerSocket& operator >> (std::string&) const;

	void accept (ServerSocket&);
};

#endif // _SERVERSOCKET_H_

