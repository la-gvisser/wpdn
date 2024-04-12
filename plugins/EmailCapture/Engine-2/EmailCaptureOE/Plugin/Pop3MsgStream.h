// ============================================================================
//	Pop3MsgStream.h
//		interface for the CPop3MsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "MsgStream.h"
#include "PenInfo.h"


// ============================================================================
//		CPop3MsgStream
// ============================================================================

class CPop3MsgStream
	:	public CMsgStream
{
protected:
	bool		Detect( const CTargetAddress& inTargetAddress ) {
		return m_PenInfoPtr->HasTarget( inTargetAddress );
	}

	void		FullAnalysis( const CCase& inCase );
	void		FullAnalysis( const CCase& inCase, const CTargetAddress& inTargetAddress );
	
	CPeekStringA&	GetMsgStringA();
	
	void		PenAnalysis( const CCase& inCase );
	void		PenAnalysis( const CCase& inCase, const CTargetAddress& inTargetAddress );
	
	bool		VerifyMsg();

public:
	;			CPop3MsgStream( const CIpAddressPortPair& inIpPortPair,
					COmniEngineContext& inContext, UInt32 inIndex );
	;			~CPop3MsgStream() {}

	bool		IsValid() { return VerifyMsg(); }
	void		Analyze( const CCase& inCase );
	void		Analyze( const CCase& inCase, const CTargetAddress& inTargetAddress );
};


// ============================================================================
//		CPop3MsgStreamList
// ============================================================================

class CPop3MsgStreamList
	:	public TStreamList<CPop3MsgStream>
{
};


// ============================================================================
//		CPop3MsgStreamListPtr
// ============================================================================

typedef class TLockedPtr<CPop3MsgStreamList>	CPop3MsgStreamListPtr;


// ============================================================================
//		CSafePop3MsgStreamList
// ============================================================================

class CSafePop3MsgStreamList
{
protected:
	CPop3MsgStreamList		m_MsgStreams;

public:
	;		CSafePop3MsgStreamList() {}

	;		operator CPop3MsgStreamListPtr() {
		return CPop3MsgStreamListPtr( &m_MsgStreams );
	}
};
