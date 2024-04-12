// ============================================================================
//	BLMessage.h
//		interface for the CBLMessage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2007. All rights reserved.

#pragma once

class COptions;

namespace BLMessage {
	enum {
		kBLM_BroadcasterDied = 'BDIE',
		kBLM_Unknown = 0,
		kBLM_NameTableUpdate,
		kBLM_Notify,
		kBLM_OptionsUpdate,
		kBLM_OptionsChanged,
		kBLM_OptionsActiveChanged
	};
};

using namespace BLMessage;

#undef GetMessage


// =============================================================================
//		CBLMessage
// =============================================================================

class CBLMessage
{
	int			m_nId;
	void*		m_pMessage;

public:

	;			CBLMessage( int inId = 0, void* inMessage = NULL )
					: m_nId( inId ), m_pMessage( inMessage ) {}
	virtual		~CBLMessage() {}

	int			GetId() const { return m_nId; }
	const void*	GetMessage() const { return m_pMessage; }
};


// =============================================================================
//		CBLStringMessage
// =============================================================================

class CBLStingMessage
	:	public CBLMessage
{
protected:
	CString		m_strMessage;

public:
	;			CBLStingMessage( int inId, CString inMessage )
					: CBLMessage( inId ), m_strMessage( inMessage ) {}

	CString		GetMessage() const { return m_strMessage; }
};


// =============================================================================
//		CBLNameTableUpdate
// =============================================================================

class CBLNameTableUpdate
	:	public CBLStingMessage
{
public:
	enum { BLID = kBLM_NameTableUpdate };

	;	CBLNameTableUpdate( CString inMessage )
			: CBLStingMessage( CBLNameTableUpdate::BLID, inMessage) {}
};


// =============================================================================
//		CBLNotify
// =============================================================================

class CBLNotify
	:	public CBLStingMessage
{
public:
	enum { BLID = kBLM_Notify };

	;	CBLNotify( CString inMessage )
			: CBLStingMessage( CBLNotify::BLID, inMessage ) {}
};


// =============================================================================
//		CBLOptionsUpdate
// =============================================================================

class CBLOptionsUpdate
	:	public CBLMessage
{
protected:
	COptions*	m_pOptions;

public:
	enum { BLID = kBLM_OptionsUpdate };

	;			CBLOptionsUpdate( COptions* inOptions )
					: CBLMessage( CBLOptionsUpdate::BLID ), m_pOptions( inOptions ) {}

	COptions*	GetMessage() const { return m_pOptions; }
};


// =============================================================================
//		CBLOptionsActiveChanged
// =============================================================================

class CBLOptionsActiveChanged
	:	public CBLMessage
{
protected:
	COptions*	m_pOptions;

public:
	enum { BLID = kBLM_OptionsActiveChanged };

	;			CBLOptionsActiveChanged( COptions* inOptions )
					: CBLMessage( CBLOptionsActiveChanged::BLID ), m_pOptions( inOptions ) {}

	COptions*	GetMessage() const { return m_pOptions; }
};
