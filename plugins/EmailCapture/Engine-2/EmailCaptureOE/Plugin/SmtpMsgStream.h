// ============================================================================
//	SmtpMsgStream.h
//		interface for the CSmtpMsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "MsgStream.h"

class CTargetList;


// ============================================================================
//		CSmtpMsgStream
// ============================================================================

class CSmtpMsgStream
	:	public CMsgStream
{
protected:
	void		FullAnalysis( const CCase& inCase );
	void		FullAnalysis( const CCase& inCase, const CTargetAddress& inTargetAddress );

	CPeekStringA&	GetMsgStringA();
	
	void		PenAnalysis( const CCase& inCase );
	void		PenAnalysis( const CCase& inCase, const CTargetAddress& inTargetAddress );
	
	bool		VerifyTextMsg();
	bool		VerifyBinaryMsg();
	bool		VerifyMsg();

public:
	;			CSmtpMsgStream( const CIpAddressPortPair& inIpPortPair,
					COmniEngineContext& inContext, size_t inIndex, const CMsgAddress& inTarget );
	;			~CSmtpMsgStream() {}

	static bool	Detect( const CCaseList& inCaseList, const CMsgPacket* inMsgPacket,
					CMsgAddress& outTarget, COmniEngineContext* pContext, size_t& outIndex );

	bool		IsValid() { return VerifyMsg(); }
	void		Analyze( const CCase& inCase );
	void		Analyze( const CCase& inCase, const CTargetAddress& inTargetAddress );
};


// ============================================================================
//		CSmtpMsgStreamList
// ============================================================================

class CSmtpMsgStreamList
	:	public TStreamList<CSmtpMsgStream>
{
};


// ============================================================================
//		CSmtpMsgStreamListPtr
// ============================================================================

typedef class TLockedPtr<CSmtpMsgStreamList>	CSmtpMsgStreamListPtr;


// ============================================================================
//		CSafeSmtpMsgStreamList
// ============================================================================

class CSafeSmtpMsgStreamList
{
protected:
	CSmtpMsgStreamList	m_MsgStreams;

public:
	;		CSafeSmtpMsgStreamList() {}

	;	operator CSmtpMsgStreamListPtr() {
		return CSmtpMsgStreamListPtr( &m_MsgStreams );
	}
};
