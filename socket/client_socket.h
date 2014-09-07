#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include "socket.h"

class ClientSocket : private Socket
{
public:
	ClientSocket (std::string host, int port);
	virtual ~ClientSocket();

	const ClientSocket& operator << (const std::string&) const;
	const ClientSocket& operator >> (std::string&) const;
};

#endif // _CLIENTSOCKET_H_

