// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PeekMessage.h"
#include "Options.h"


// =============================================================================
//		MessageFactory
// =============================================================================

CPeekMessagePtr	MessageFactory( const CPeekStream& inMessage );


// =============================================================================
//		CMessageExample
// =============================================================================

class CMessageExample
	:	public CPeekMessage
{
protected:
	static int			s_nMessageType;
	static CPeekString	s_strMessageName;
	static CPeekString	s_strTagMessageLabel;
	static CPeekString	s_strTagMessage;
	static CPeekString	s_strTagResponse;

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
	;		CMessageExample::CMessageExample( const CPeekStream& inMessage )
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

protected:
	static int			s_nMessageType;
	static CPeekString	s_strMessageName;
	static CPeekString	s_strTagId;
	static CPeekString	s_strTagOptionsList;
	static CPeekString	s_strTagOptions;
	static CPeekString	s_strTagDefault;
	static CPeekString	s_strTagName;

protected:
	// Message

	// Response
	CNamedOptionsList	m_ayNamedOptions;

public:
	static int					GetMessageType() { return s_nMessageType; }
	static const CPeekString&	GetMessageName() { return s_strMessageName; }

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
//		CMessageSetOptions
// =============================================================================

class CMessageSetOptions
	:	public CPeekMessage
{
protected:
	static int			s_nMessageType;
	static CPeekString	s_strMessageName;
	static CPeekString	s_strTagName;
	static CPeekString	s_strTagId;
	static CPeekString	s_strTagOptions;

protected:
	// Message
	CGlobalId		m_idContext;
	CPeekString		m_strName;
	COptions		m_Options;

	// Response

public:
	static int					GetMessageType() { return s_nMessageType; }
	static const CPeekString&	GetMessageName() { return s_strMessageName; }

public:
	;		CMessageSetOptions( CGlobalId inId, const CPeekString& inName, const COptions& inOptins );
	;		CMessageSetOptions( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
		LoadMessage();
	}

	CGlobalId			GetContextId() const { return m_idContext; }
	const CPeekString&	GetContextName() const { return m_strName; }
	const COptions&		GetContextOptions() const { return m_Options; }

	virtual bool	LoadMessage();
	virtual bool	LoadResponse( const CPeekStream& inResponse );

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }
	virtual bool	StoreMessage( CPeekStream& outMessage );
	virtual bool	StoreResponse( CPeekStream& outResponse );

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};

#endif //  IMPLEMENT_PLUGINMESSAGE
