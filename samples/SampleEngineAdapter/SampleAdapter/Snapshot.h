// =============================================================================
//	Snapshot.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "Plugin.h"


// =============================================================================
//		CSnapshot
// =============================================================================

class	CSnapshot
{
protected:
	void*		m_pvSnapshot;

public:
	;			CSnapshot( void* p = NULL );
	virtual		~CSnapshot();

	int			GetCount( SInt32& nCount );
	int			GetId( GUID& Id );
	int			GetItem( SInt32 nItem, GUID& Id );
	int			GetItemLabel( const GUID& Id, CString& strLabel );
	int			GetItemParent( const GUID& Id, GUID& idParent );
	int			GetItemValue( const GUID& id, Plugin::SummaryStat& Stat );
	int			GetName( CString& strName );

	int			HasItem( const GUID& Id, bool& bHasItem );

	void		ReleasePtr();

	int			SetItem( const GUID& Id, const GUID& guidParent, CString strLabel,
					Plugin::SummaryStat* pSummaryStat );
	void		SetPtr( void* p );

	void		UpdateSummaryStat( const GUID& Id, Plugin::SummaryStatEntry& Stat,
					UInt32 nStringId, bool bNeedLabels );
};
