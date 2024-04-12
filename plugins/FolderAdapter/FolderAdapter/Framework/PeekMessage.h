// =============================================================================
//	PeekMessage.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PeekStrings.h"
#include "PeekDataModeler.h"
#include "PeekContext.h"
#include "heresult.h"
#include <map>
#include <memory>

class CPeekPlugin;
class COmniEngineContext;
class CPeekStream;

#ifdef _WIN32
class CPeekConsoleContext;
class COmniConsoleContext;
#endif // _WIN32


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Message
//
//	A Peek Message is the base class for all messages that the Console side of
//	the plugin sends to the Engine side of the plugin.
//	The message may contain both Modeled XML information and unformatted binary
//	information. The message content, optional, is sent from the Console to the
//	Engine. The	response content, also optional, is returned from the Engine to
//	the Console. Messages may be sent to the Omni Plugin or a specific Omni
//	Engine Context.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		Data Structures
// =============================================================================

#include "wppushpack.h"

struct tPeekMessageHeader {
	UInt64	fDataModelerLength;
	UInt64	fBinaryDataLength;

	size_t	GetBinaryDataLength() const { return static_cast<size_t>( fBinaryDataLength & kMaxSizeT ); }
	size_t	GetDataModelerLength() const { return static_cast<size_t>( fDataModelerLength & kMaxSizeT ); }
	void	Init() { fDataModelerLength = 0; fBinaryDataLength = 0; }
	void	SetBinaryDataLength( size_t inLength ) { fBinaryDataLength = inLength; }
	void	SetDataModelerLength( size_t inLength ) { fDataModelerLength = inLength; }
	size_t	TotalLength() const { return (GetDataModelerLength() + GetBinaryDataLength()); }
} WP_PACK_STRUCT;
typedef struct tPeekMessageHeader	tPeekMessageHeader;

#include "wppoppack.h"


// ==============================================================================
//		CPeekMessage
//
//	Engine:
//		new CPeekMessage( CPeekStream& inMessage );
//		CPeekMessage::ProcessPluginMessage( outResponse );
//			Load()
// ==============================================================================

class CPeekMessage
{
public:
	static const int			s_nMessageType = 'PMSG';
	static const CPeekString	s_strMessageName;

public:
	typedef std::unique_ptr<CPeekDataModeler>	CPeekDataModelerPtr;

protected:
	CPeekDataModelerPtr	m_spModeler;
	size_t				m_nDataModelerLength;
	size_t				m_nBinaryDataLength;
	UInt8*				m_pBinaryData;
	bool				m_bBinaryDataOwner;

protected:
	bool	Load( const CPeekStream& inMessage );

	void	ReleaseData() {
		if ( m_pBinaryData && m_bBinaryDataOwner ) {
			free( m_pBinaryData );
		}
		m_pBinaryData = nullptr;
		m_nBinaryDataLength = 0;
		m_bBinaryDataOwner = false;
	}
	void	ReleaseModeler() {
		m_spModeler.reset();
		m_nDataModelerLength = 0;
	}

	bool	Store( CPeekStream& outMessage );

public:
	// Constructor to process a message.
	;		CPeekMessage()
		:	m_nDataModelerLength( 0 )
		,	m_nBinaryDataLength( 0 )
		,	m_pBinaryData( nullptr )
		,	m_bBinaryDataOwner( false )
	{
	}
	// Constructor for new message to post.
	;			CPeekMessage( const CPeekString& inName )
		:	m_nDataModelerLength( 0 )
		,	m_nBinaryDataLength( 0 )
		,	m_pBinaryData( nullptr )
		,	m_bBinaryDataOwner( false )
	{
		m_spModeler = std::unique_ptr<CPeekDataModeler>( new CPeekDataModeler( inName ) );
	}
	// Constructor to process a message.
	;		CPeekMessage( const CPeekStream& inMessage )
		:	m_nDataModelerLength( 0 )
		,	m_nBinaryDataLength( 0 )
		,	m_pBinaryData( nullptr )
		,	m_bBinaryDataOwner( false )
	{
		LoadMessage( inMessage );
	}

	virtual	~CPeekMessage() {
		Reset();
	}

	size_t			GetDataModelerLen() { return m_nDataModelerLength; }
	CPeekStream		GetBinaryData() const {
		CPeekStream	psData;
		psData.Write( m_nBinaryDataLength, m_pBinaryData );
		psData.Rewind();
		return psData;
	}
	size_t			GetBinaryDataLength() const { return m_nBinaryDataLength; }
	size_t			GetLength() const {
		return (m_nDataModelerLength + m_nBinaryDataLength);
	}
	size_t			GetModelerLength() { return m_nDataModelerLength; }
	CPeekString		GetModelerRoot();

	bool			IsBinary() { return (m_pBinaryData != nullptr); }
	bool			IsModeled() { return (m_spModeler.get() != nullptr); }

	// Load a stream into a modeler and then extract the data members.
	virtual bool	LoadMessage( const CPeekStream& inMessage ) { return Load( inMessage ); }

	void			Reset() {
		ReleaseModeler();
		ReleaseData();
	}

	void			SetBinaryData( const CPeekStream& inData );
	void			SetBinaryData( size_t inLength, UInt8* inData );
	bool			StartResponse( const CPeekString& inName );
	// Load the data members into a modeler and store the modeler into the Stream.
	virtual bool	StoreResponse( CPeekStream& outResponse ) { return Store( outResponse ); }

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// ==============================================================================
//		CPeekMessagePtr
// ==============================================================================

typedef std::shared_ptr<CPeekMessage>	CPeekMessagePtr;

#endif // IMPLEMENT_PLUGINMESSAGE
