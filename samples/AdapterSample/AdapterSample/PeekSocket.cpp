// =============================================================================
//	PeekSocket.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekSocket.h"


// =============================================================================
//		CPeekSocket
// =============================================================================

// -----------------------------------------------------------------------------
//		Bind
// -----------------------------------------------------------------------------

bool
CPeekSocket::Bind(
	CPort	inPort )
{
	Close();

	m_Socket = socket( PF_INET, SOCK_DGRAM, 0 );
	if ( !IsOpen() ) return false;

	// Set the socket options
	int no = 1;
	int nError = setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&no, sizeof( no ) );
	ASSERT( nError == 0 );
	if ( nError != 0 ) return false;

	// bind socket to any address on CPort
	SOCKADDR_IN	sockAddr;
	memset( &sockAddr, 0, sizeof( sockAddr ) );
	sockAddr.sin_family = AF_INET;
 	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons( (u_short)inPort );

	nError = ::bind( m_Socket, (SOCKADDR*)&sockAddr, sizeof( sockAddr ) );
	ASSERT( nError == 0 );

	if ( nError != 0 ) {
		DWORD			nLastError = WSAGetLastError();
		CPeekOutString	osError;
		osError << L"Socket Error: " << nLastError << std::endl;
		// AfxMessageBox( osError.c_str );
		return false;
	}

	// make the socket nonblocking
	u_long	nArg = 1;
	nError = ::ioctlsocket( m_Socket, FIONBIO, &nArg );
	ASSERT( nError == 0 );
	return true;
}
