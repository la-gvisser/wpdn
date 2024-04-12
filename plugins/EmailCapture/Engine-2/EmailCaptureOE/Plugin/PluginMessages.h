// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "GlobalId.h"
#include "PeekMessage.h"
#include "Options.h"
#include <vector>


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
	typedef std::vector<TNamedOptions>	CNamedOptionsList;

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

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
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

	void			SetOptions( const COptions* inOptions ) { m_Options = *inOptions; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


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

#endif // IMPLEMENT_PLUGINMESSAGE
