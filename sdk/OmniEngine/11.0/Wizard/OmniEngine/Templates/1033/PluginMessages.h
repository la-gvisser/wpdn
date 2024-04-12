// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "GlobalId.h"
#include "PeekMessage.h"
#include "Options.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Plugin Message
//
//	The plugin specific messages that a plugin sends and receives. All Plugin
//	Messages are sub-classed from Peek Message.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		MessageFactory
// =============================================================================

std::unique_ptr<CPeekMessage>	MessageFactory( const CPeekStream& inMessage );


// =============================================================================
//		CMessageGetAllOptions
// =============================================================================

class CMessageGetAllOptions
	:	public CPeekMessage
{
public:
	typedef struct _TNamedOptions {
		CGlobalId	fId;
		CPeekString	fName;
		COptions	fOptions;
	} TNamedOptions;
	typedef CPeekArray<TNamedOptions>	CNamedOptionsList;

public:
	static const int			s_nMessageType = 'MGAO';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagOptionsList;
	static const CPeekString	s_strTagOptions;
	static const CPeekString	s_strTagDefault;
	static const CPeekString	s_strTagName;

protected:
	// Message

	// Response
	CNamedOptionsList	m_ayNamedOptions;

public:
	;		CMessageGetAllOptions();
	;		CMessageGetAllOptions( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const CNamedOptionsList&	GetNamedOptions() const { return m_ayNamedOptions; }
	CNamedOptionsList&			GetNamedOptions() { return m_ayNamedOptions; }

	virtual bool	LoadMessage();
[!if !NOTIFY_UIHANDLER]
	virtual bool	LoadResponse( const CPeekStream& inResponse );
[!endif]

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
[!if !NOTIFY_UIHANDLER]
	virtual bool	StoreMessage( CPeekStream& outMessage );
[!endif]
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageGetOptions
// =============================================================================

class CMessageGetOptions
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'MGET';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagName;
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagOptions;

protected:
	// Message
	CGlobalId		m_idContext;

	// Response
	CPeekString		m_strName;
	COptions		m_Options;

public:
	;		CMessageGetOptions( CGlobalId inId );
	;		CMessageGetOptions( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const COptions&		GetOptions() const { return m_Options; }
	COptions&			GetOptions() { return m_Options; }

	virtual bool	LoadMessage();
[!if !NOTIFY_UIHANDLER]
	virtual bool	LoadResponse( const CPeekStream& inResponse );
[!endif]

	void			SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
[!if !NOTIFY_UIHANDLER]
	virtual bool	StoreMessage( CPeekStream& outMessage );
[!endif]
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]


// =============================================================================
//		CMessageTab
// =============================================================================

class CMessageTab
	: public CPeekMessage
{
public:
	static const int			s_nMessageType = 'CTAB';
	static const CPeekString	s_strMessageName;
	static const CPeekString	s_strTagMessage;

protected:
	// Message
	CPeekString	m_strMessage;

	// Response
	CPeekString	m_strResponse;

public:
	;		CMessageTab();
	;		CMessageTab( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const CPeekString&	GetMessage() const { return m_strMessage; }
	const CPeekString&	GetResponse() const { return m_strResponse; }

	virtual bool	LoadMessage();

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	void			SetMessage( const CPeekString& inMessage ) { m_strMessage = inMessage; }
	void			SetResponse( const CPeekString& inResponse ) { m_strResponse = inResponse; }

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};
[!endif]
[!if !NOTIFY_UIHANDLER]


// =============================================================================
//		CMessagePing
// =============================================================================

class CMessagePing
	: public CPeekMessage
{
public:
	static const int			s_nMessageType = 'PING';
	static const CPeekString	s_strMessageName;

protected:
	// Message

	// Response

public:
	;		CMessagePing();
	;		CMessagePing( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
	}

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageExample
// =============================================================================

class CMessageExample
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'MSGX';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagMessageLabel;
	static const CPeekString	s_strTagMessage;
	static const CPeekString	s_strTagResponse;

protected:
	// Message
	CPeekString		m_strMessage;
	CPeekStream		m_MessageData;

	// Response
	CPeekString		m_strResponse;
	CPeekStream		m_ResponseData;

public:
	static int					GetMessageType() { return s_nMessageType; }
	static const CPeekString&	GetMessageName() { return s_strMessageName; }
	static const CPeekString&	GetTagMessage() { return s_strTagMessage; }
	static const CPeekString&	GetTagResponse() { return s_strTagResponse; }

public:
	;		CMessageExample() : CPeekMessage( s_strMessageName ) {}
	;		CMessageExample( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const CPeekString&	GetMessage() const { return m_strMessage; }
	const CPeekStream&	GetMessageData() const { return m_MessageData; }
	const CPeekString&	GetResponse() const { return m_strResponse; }
	const CPeekStream&	GetResponseData() const { return m_ResponseData; }

	virtual bool		LoadMessage();
	virtual bool		LoadResponse( const CPeekStream& inResponse );

	void				SetMessage( const CPeekString& inMessage ) { m_strMessage = inMessage; }
	void				SetMessageData( UInt32 inCount, const UInt8* inData ) {
		m_MessageData.Write( inCount, inData );
	}
	void				SetResponse( const CPeekString& inResponse ) { m_strResponse = inResponse; }
	void				SetResponseData( UInt32 inCount, const UInt8* inData ) {
		m_ResponseData.Write( inCount, inData );
	}

	bool				StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }

	virtual bool		StoreMessage( CPeekStream& outMessage );
	virtual bool		StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};
[!endif]

#endif // IMPLEMENT_PLUGINMESSAGE
