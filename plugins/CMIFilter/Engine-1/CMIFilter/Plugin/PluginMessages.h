// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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

CPeekMessagePtr	MessageFactory( const CPeekStream& inMessage );


// =============================================================================
//		CMessagePing
// =============================================================================

class CMessagePing
	:	public CPeekMessage
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
	virtual bool	LoadResponse( const CPeekStream& inResponse );

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreMessage( CPeekStream& outMessage );
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
	virtual bool	LoadResponse( const CPeekStream& inResponse );

	void			SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreMessage( CPeekStream& outMessage );
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};

#endif // IMPLEMENT_PLUGINMESSAGE
