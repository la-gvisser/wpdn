// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

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
//		CMessageCreate
// =============================================================================

class CMessageCreate
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'MCRT';
	static const CPeekString	s_strMessageName;
	static const CPeekString	s_strTagAdapter;
	static const CPeekString	s_strTagName;
	static const CPeekString	s_strTagResult;

protected:
	// Message
	CPeekString	m_strAdapterName;

	// Response
	CPeekString	m_strResult;

public:
	;		CMessageCreate();
	;		CMessageCreate( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const CPeekString&	GetAdapterName() const { return m_strAdapterName; }
 	const CPeekString&	GetResult() const { return m_strResult; }

	virtual bool	LoadMessage();

	void			SetResponse( const CPeekString& inResponse) { m_strResult = inResponse; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageList
// =============================================================================

class CMessageList
	:	public CPeekMessage
{
public:
	typedef struct _TIdName {
		CGlobalId	fId;
		CPeekString	fName;
	} TIdName;
	typedef CPeekArray<TIdName>	CIdNameList;

public:
	static const int			s_nMessageType = 'MLST';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagAdapterList;
	static const CPeekString	s_strTagAdapter;
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagName;

protected:
	// Message

	// Response
	CIdNameList		m_ayAdapterList;

public:
	;		CMessageList();
	;		CMessageList( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
	}

	const CIdNameList&	GetAdapterList() const { return m_ayAdapterList; }
	CIdNameList&		GetAdapterList() { return m_ayAdapterList; }

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	void			SetResponse( const CPeekString& inResponse);
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};

#endif // IMPLEMENT_PLUGINMESSAGE
