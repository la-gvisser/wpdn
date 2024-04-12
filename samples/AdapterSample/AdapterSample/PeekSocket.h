// =============================================================================
//	PeekSocket.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"


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
			::shutdown( m_Socket, SD_BOTH );
			::closesocket( m_Socket );
			m_Socket = INVALID_SOCKET;
		}
	}

	bool	IsOpen() const { return (m_Socket != INVALID_SOCKET); }

	bool	Open( UInt16 inPort ) {
		m_Socket = inPort;
	}
};
