// ============================================================================
//	PenFileEx.cpp
//		implementation of the CPenFileEx class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PenFileEx.h"
#include "Options.h"


// ============================================================================
//		CPenFileEx
// ============================================================================

// ----------------------------------------------------------------------------
//		Update
// ----------------------------------------------------------------------------

bool
CPenFileEx::Update(
	COptions*	inOptions )
{
	const COptions::COptionsCases&	CaseList( inOptions->GetCases() );

	size_t	nCaseIndex( kIndex_Invalid );
	if ( !CaseList.Find( m_CaseId, nCaseIndex ) ) return false;

	const CCase&			Case( CaseList.GetAt( nCaseIndex ) );
	const CCaseSettings&	Settings( Case.GetSettings() );
	const CReportMode&		Mode( Settings.GetReportMode() );

	m_MaxFileAge.Set( kSeconds, Mode.GetSaveAge() );
	m_nMaxFileSize = Mode.GetSaveSize();

#if 0
	int	nType( GetPenFileType( m_strFileName ) );
#else
	int	nType( 0 );
#endif
	if ( nType == kFileTypeTxt ) {
		if ( !Mode.IsPenText() ) return false;
	}
	else if ( nType == kFileTypeXml ) {
		if ( !Mode.IsPenXml() ) return false;
	}

	return (Mode.GetMode() == kMode_Pen );
}
