// ============================================================================
//	PenFileEx.h
//		interface for the CPenFileEx class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "FileMap.h"
#include "Case.h"

class COptions;


// ============================================================================
//		CPenFileEx
// ============================================================================

class CPenFileEx
	:	public CFileItem
{
protected:
	CCase::tCaseId	m_CaseId;
	CPeekString		m_strTargetName;
	CPeekTime		m_MaxFileAge;
	UInt64			m_nMaxFileSize;

public:
	;		CPenFileEx( CCase::tCaseId inId, const CPeekString& inTarget )
		:	m_CaseId( inId )
		,	m_strTargetName( inTarget )
		,	m_MaxFileAge( kMaxUInt64 )
		,	m_nMaxFileSize( kMaxUInt64 )
	{}
	virtual	~CPenFileEx() { Close(); }

	virtual bool	DoMonitor() { return (IsFileOld() || IsFileBig());	}
	void	GetFileMax( CPeekTime& outMaxAge, UInt64& outMaxSize ) const {
		outMaxAge = m_MaxFileAge;
		outMaxSize = m_nMaxFileSize;
	}
	CPeekString	GetTargetName() const { return m_strTargetName; }
	bool	IsFileBig() const {
		return (CFileItem::GetLength() > m_nMaxFileSize);
	}
	bool	IsFileOld() const {
		CPeekTime	nCurrentTime( true );
		CPeekTime	nCreateTime( GetFileCreate() );
		UInt64	nFileAge( (nCurrentTime - nCreateTime) / 10000000L );	// 100 nanoseconds to seconds
		return (nFileAge > m_MaxFileAge);
	}
	void	OpenInProgress( const CPeekString& inFileName, UINT inFlags, CPeekTime inMaxAge, UInt64 inMaxSize ) {
		m_MaxFileAge = inMaxAge;
		m_nMaxFileSize = inMaxSize;
		CFileItem::OpenInProgress( inFileName, inFlags );
	}
	bool	Update( COptions* );
};
