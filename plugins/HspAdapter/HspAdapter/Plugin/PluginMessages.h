// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekMessage.h"

#ifdef IMPLEMENT_PLUGINMESSAGE
#include "GlobalId.h"
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
//		CMessageCreateAdapter
// =============================================================================

class CMessageCreateAdapter
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'CADP';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagAdapterConfig;
	static const CPeekString	s_strTagResult;

protected:
	// Message
	CGlobalId		m_Id;

	// Response
	UInt32			m_nResult;

public:
	;		CMessageCreateAdapter();
	;		CMessageCreateAdapter( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	CGlobalId		GetId() const { return m_Id; }

	virtual bool	LoadMessage();

	void			SetResult( UInt32 inResult ) { m_nResult = inResult; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageGetAdapterOptions
// =============================================================================

class CMessageGetAdapterOptions
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'CGAO';
	static const CPeekString	s_strMessageName;
	static const CPeekString	s_strTagAdapterOptions;

protected:
	static const CPeekString	s_strTagId;

protected:
	// Message
	CGlobalId		m_Id;

public:
	;		CMessageGetAdapterOptions();
	;		CMessageGetAdapterOptions( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
	}

	void			SetId( const CGlobalId& inId ) { m_Id = inId; }

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageModifyAdapter
// =============================================================================

class CMessageModifyAdapter
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'MODA';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagAdapterConfig;
	static const CPeekString	s_strTagResult;

protected:
	// Message
	CGlobalId		m_Id;
	CPeekString		m_Name;

	// Response
	UInt32			m_nResult;

public:
	;		CMessageModifyAdapter();
	;		CMessageModifyAdapter( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	const CGlobalId&	GetId() const { return m_Id; }
	const CPeekString&	GetAdapterName() const { return m_Name; }

	virtual bool	LoadMessage();

	void			SetResult( UInt32 inResult ) { m_nResult = inResult; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};

#endif	// IMPLEMENT_PLUGINMESSAGE
