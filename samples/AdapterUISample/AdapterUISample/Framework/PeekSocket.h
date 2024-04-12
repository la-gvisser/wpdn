// =============================================================================
//	PeekSocket.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif // _WIN_32

#ifdef TARGET_OS_LINUX
#include <sys/socket.h>

#define INVALID_SOCKET		(-1)
#define SD_BOTH				0
#define closesocket(s)	close(s)
typedef int		SOCKET;

#endif // TARGET_OS_LINUX


// =============================================================================
//		CPeekSocket
// =============================================================================

class CPeekSocket
{
	SOCKET	m_Socket;

public:
	;		CPeekSocket() : m_Socket( INVALID_SOCKET ) {}
	;		~CPeekSocket() {
		Close();
	}

	CPeekSocket&	operator=( SOCKET inSocket ) {
		m_Socket = inSocket;
		return *this;
	}
	;		operator SOCKET() { return m_Socket; }

	bool	Bind( CPort inPort );

	void	Close() {
		if ( IsOpen() ) {
#ifdef _WIN32
			::shutdown( m_Socket, SD_BOTH );
			::closesocket( m_Socket );
#else
			shutdown( m_Socket, SD_BOTH );
#endif
			m_Socket = INVALID_SOCKET;
		}
	}

	bool	IsOpen() const { return (m_Socket != INVALID_SOCKET); }

	bool	Open( UInt16 inPort ) {
		m_Socket = inPort;
		return true;
	}
};
