// =============================================================================
//	PeekMessage.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

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
[!if !NOTIFY_UIHANDLER]
	virtual bool	LoadResponse( const CPeekStream& inResponse ) { return Load( inResponse ); }
[!endif]

	void			Reset() {
		ReleaseModeler();
		ReleaseData();
	}

	void			SetBinaryData( const CPeekStream& inData );
	void			SetBinaryData( size_t inLength, UInt8* inData );
	bool			StartResponse( const CPeekString& inName );
	// Load the data members into a modeler and store the modeler into the Stream.
[!if !NOTIFY_UIHANDLER]
	virtual bool	StoreMessage( CPeekStream& outMessage ) { return Store( outMessage ); }
[!endif]
	virtual bool	StoreResponse( CPeekStream& outResponse ) { return Store( outResponse ); }

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// ==============================================================================
//		CPeekMessagePtr
// ==============================================================================

typedef std::shared_ptr<CPeekMessage>	CPeekMessagePtr;
[!if !NOTIFY_UIHANDLER]


#ifdef _WIN32
// ==============================================================================
//		CPeekMessageProcess
// ==============================================================================

class CPeekMessageProcess
{
public:
	virtual 		~CPeekMessageProcess() {}

	virtual void	ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
						const CPeekStream& inResponse ) = 0;
};

typedef void (CPeekMessageProcess::*FnResponseProcessor)(
				HeResult, CPeekMessagePtr, const CPeekStream& );


// ==============================================================================
//		CPeekMessageItem
// ==============================================================================

class CPeekMessageItem
{
protected:
	UInt32					m_dwTransId;
	CPeekMessagePtr			m_spMessage;
	CPeekMessageProcess*	m_pMsgProc;
	FnResponseProcessor		m_pfnResponseProc;

public:
	;		CPeekMessageItem()
		: m_dwTransId( kMaxUInt32 )
		, m_pMsgProc( nullptr )
		, m_pfnResponseProc( nullptr )
	{}
	;		CPeekMessageItem( UInt32 inTransId, CPeekMessagePtr inMessage,
				CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc )
		: m_dwTransId( inTransId )
		, m_spMessage( inMessage )
		, m_pMsgProc( inMsgProc )
		, m_pfnResponseProc( inResponseProc )
	{}

	void	CallResponseProcessor( HeResult inResult, CPeekStream& inResponse ) {
		if ( IsNotValid() ) return;
		if ( m_pMsgProc == nullptr ) return;
		m_pMsgProc->ProcessPluginResponse( inResult, m_spMessage, inResponse );
	}
	CPeekMessage*			GetMessage() const { return m_spMessage.get(); }
	UInt32					GetTransId() const { return m_dwTransId; }
	CPeekMessageProcess*	GetMsgProc() const { return m_pMsgProc; }
	FnResponseProcessor		GetResponseProcessor() const { return m_pfnResponseProc; }

	bool	IsNotValid() const { return (m_spMessage.get() == nullptr); }
	bool	IsValid() const { return (m_pMsgProc != nullptr); }
};


// ==============================================================================
//		CPeekMessageQueue
// ==============================================================================

class CPeekMessageQueue
	: public std::map<DWORD, CPeekMessageItem>
	, public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>

{
protected:
	static const UInt32		s_nDefaultTimeout = 60;	// one minute.

protected:
	CPeekPlugin*			m_pPlugin;
	CPeekConsoleContext*	m_pContext;

public:
	;		CPeekMessageQueue( CPeekPlugin* inPlugin )
		:	m_pPlugin( inPlugin )
		,	m_pContext( nullptr )
	{
		_ASSERTE( (m_pPlugin != nullptr) || (m_pContext != nullptr) );
	}
	;		CPeekMessageQueue( CPeekConsoleContext* inContext )
		:	m_pPlugin( nullptr )
		,	m_pContext( inContext )
	{
		_ASSERTE( (m_pPlugin != nullptr) || (m_pContext != nullptr) );
	}

	void	Add( DWORD inTransId, CPeekMessagePtr inMessage,
				CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc ) {
		std::map<DWORD, CPeekMessageItem>::operator[]( inTransId ) =
			CPeekMessageItem( inTransId, inMessage, inMsgProc, inResponseProc );
	}

	CPeekMessageItem&	GetAt( DWORD inTransId ) {
		return std::map<DWORD, CPeekMessageItem>::operator[]( inTransId );
	}

	bool	PostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );

	void	RemoveAll() { clear(); }
	void	RemoveAt( DWORD inTransId ) { erase( inTransId ); }
};
#endif // _WIN32
[!endif]

#endif // IMPLEMENT_PLUGINMESSAGE
