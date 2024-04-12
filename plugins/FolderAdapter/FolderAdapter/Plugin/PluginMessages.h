// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

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
//		CMessageCreateAdapter
// =============================================================================

class CMessageCreateAdapter
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'MAKE';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagFolderAdapter;
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagMonitor;
	static const CPeekString	s_strTagMask;
	static const CPeekString	s_strTagSpeed;
	static const CPeekString	s_strTagSave;
	static const CPeekString	s_strTagResult;

protected:
	// Message
	CPeekString		m_strMonitor;
	CPeekString		m_strMask;
	CPeekString		m_strSpeed;
	CPeekString		m_strSave;

	// Response
	UInt32			m_nResult;

public:
	;		CMessageCreateAdapter();
	;		CMessageCreateAdapter( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	CPeekString		GetMonitor() const { return m_strMonitor; }
	CPeekString		GetMask() const { return m_strMask; }
	CPeekString		GetSpeed() const { return m_strSpeed; }
	CPeekString		GetSave() const { return m_strSave; }

	virtual bool	LoadMessage();

	void			SetResult( UInt32 inResult ) { m_nResult = inResult; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};


// =============================================================================
//		CMessageGetAdapter
// =============================================================================

class CMessageGetAdapter
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'GETA';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagFolderAdapter;
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagMonitor;
	static const CPeekString	s_strTagMask;
	static const CPeekString	s_strTagSpeed;
	static const CPeekString	s_strTagSave;

protected:
	// Message & Response
	CGlobalId		m_Id;

	// Response
	UInt32			m_nResult;
	CPeekString		m_strMonitor;
	CPeekString		m_strMask;
	SInt32			m_nSpeed;
	CPeekString		m_strSave;

public:
	;		CMessageGetAdapter();
	;		CMessageGetAdapter( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	CGlobalId		GetId() const { return m_Id; }

	virtual bool	LoadMessage();

	void			SetAdapterOptions( const CAdapterOptions& inAdapterOptions );

	void			SetResult( UInt32 inResult ) { m_nResult = inResult; }
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
	static const int			s_nMessageType = 'UPDT';
	static const CPeekString	s_strMessageName;

protected:
	static const CPeekString	s_strTagFolderAdapter;
	static const CPeekString	s_strTagId;
	static const CPeekString	s_strTagMonitor;
	static const CPeekString	s_strTagMask;
	static const CPeekString	s_strTagSpeed;
	static const CPeekString	s_strTagSave;
	static const CPeekString	s_strTagResult;

protected:
	// Message
	CGlobalId		m_Id;
	CPeekString		m_strMonitor;
	CPeekString		m_strMask;
	CPeekString		m_strSpeed;
	CPeekString		m_strSave;

	// Response
	UInt32			m_nResult;

public:
	;		CMessageModifyAdapter();
	;		CMessageModifyAdapter( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	CAdapterOptions	GetAdapterOptions() const;
	CGlobalId		GetId() const { return m_Id; }
	CPeekString		GetMonitor() const { return m_strMonitor; }
	CPeekString		GetMask() const { return m_strMask; }
	CPeekString		GetSpeed() const { return m_strSpeed; }
	CPeekString		GetSave() const { return m_strSave; }

	virtual bool	LoadMessage();

	void			SetResult( UInt32 inResult ) { m_nResult = inResult; }
	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};
