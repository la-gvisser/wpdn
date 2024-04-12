// =============================================================================
//	Snapshot.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Snapshot.h"
#include "peekcore.h"
#include "peekstats.h"

using namespace Plugin;


// =============================================================================
//		CSnapshot
// =============================================================================

CSnapshot::CSnapshot(
	void*	p /*= NULL*/ )
	:	m_pvSnapshot( NULL )
{
	SetPtr( p );
}

CSnapshot::~CSnapshot()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		GetCount
// -----------------------------------------------------------------------------

int
CSnapshot::GetCount(
	SInt32&	nCount )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	return pSnapshot->GetCount( &nCount );
}


// -----------------------------------------------------------------------------
//		GetId
// -----------------------------------------------------------------------------

int
CSnapshot::GetId(
	GUID&	Id )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	return pSnapshot->GetID( reinterpret_cast<HeID*>( &Id ) );
}


// -----------------------------------------------------------------------------
//		GetItem
// -----------------------------------------------------------------------------

int
CSnapshot::GetItem(
	SInt32	nItem,
	GUID&	Id )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	return pSnapshot->GetItem( nItem, reinterpret_cast<HeID*>( &Id ) );
}


// -----------------------------------------------------------------------------
//		GetItemLabel
// -----------------------------------------------------------------------------

int
CSnapshot::GetItemLabel(
	const GUID&		Id,
	CString&		strLabel )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	CHeBSTR	bstrLabel;
	if ( HE_FAILED( pSnapshot->GetItemLabel( reinterpret_cast<const HeID&>( Id ), &bstrLabel.m_str ) ) ) return PEEK_PLUGIN_FAILURE;
	strLabel = bstrLabel;
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		GetItemParent
// -----------------------------------------------------------------------------

int
CSnapshot::GetItemParent(
	const GUID&		Id,
	GUID&			idParent )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	return pSnapshot->GetItemParent( reinterpret_cast<const HeID&>( Id ), reinterpret_cast<HeID*>( &idParent ) );
}


// -----------------------------------------------------------------------------
//		GetItemValue
// -----------------------------------------------------------------------------

int
CSnapshot::GetItemValue(
	const GUID&		Id,
	SummaryStat&	Stat )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	pSnapshot->GetItemValue( reinterpret_cast<const HeID&>( Id ), reinterpret_cast<PeekSummaryStat*>( &Stat ) );
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

int
CSnapshot::GetName(
	CString&	strName )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	CHeBSTR	bstrName;
	if ( HE_FAILED( pSnapshot->GetName( &bstrName.m_str ) ) ) return PEEK_PLUGIN_FAILURE;
	strName = bstrName;
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		HasItem
// -----------------------------------------------------------------------------

int
CSnapshot::HasItem(
	const GUID& id,
	bool&		bHasItem )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	HeCID	cid( *(reinterpret_cast<const HeCID*>( &id )) );
	BOOL	bResult = FALSE;
	pSnapshot->HasItem( cid, &bResult );
	bHasItem = !(!bResult); // (bResult) ? true : false; !(!bResult); !(bResult == FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CSnapshot::ReleasePtr()
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot ) {
		pSnapshot->Release();
		m_pvSnapshot = NULL;
	}
}


// -----------------------------------------------------------------------------
//		SetItem
// -----------------------------------------------------------------------------

int
CSnapshot::SetItem(
	const GUID&		id,
	const GUID&		guidParent,
	CString			strLabel,
	SummaryStat*	pSummaryStat )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return PEEK_PLUGIN_FAILURE;

	HeCID	cid( *(reinterpret_cast<const HeCID*>( &id )) );
	HeCID	cidParent( *(reinterpret_cast<const HeCID*>( &guidParent )) );
	CHeBSTR	bstrLabel( strLabel.GetBuffer() );
	return pSnapshot->SetItem( cid, cidParent, bstrLabel, reinterpret_cast<PeekSummaryStat*>( pSummaryStat ) );
}


// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CSnapshot::SetPtr(
	void* p )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( p ) );
	if ( pSnapshot ) {
		m_pvSnapshot = p;
		pSnapshot->AddRef();
	}
}


// -----------------------------------------------------------------------------
//		UpdateSummaryStat
// -----------------------------------------------------------------------------

void
CSnapshot::UpdateSummaryStat(
	const GUID&			guidParent,
	SummaryStatEntry&	Stat,
	UInt32				nLabelId,
	bool				bNeedLabel )
{
	ISummaryStatsSnapshot*	pSnapshot( reinterpret_cast<ISummaryStatsSnapshot*>( m_pvSnapshot ) );
	if ( pSnapshot == NULL ) return;

	HeCID	cid( *(reinterpret_cast<const HeCID*>( &Stat.Id )) );
	HeCID	cidParent( *(reinterpret_cast<const HeCID*>( &guidParent )) );

	CHeBSTR	bstrLabel;
	if ( bNeedLabel ) {
		CAtlString	str;
		str.LoadString( nLabelId );
		bstrLabel = str;
	}

	pSnapshot->SetItem( cid, cidParent, bstrLabel, reinterpret_cast<PeekSummaryStat*>( &Stat.Stat ) );
}
