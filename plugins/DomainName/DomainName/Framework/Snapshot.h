// =============================================================================
//	Snapshot.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "Peek.h"
#include "peekstats.h"
#include "GlobalId.h"


// =============================================================================
//		CSummaryStat
// =============================================================================

class CSummaryStat
{
protected:
	Peek::SummaryStat	m_Stat;

public:
	;			CSummaryStat() { Reset(); }
	;			CSummaryStat( const Peek::SummaryStat& inOther ) : m_Stat( inOther ) {}
	;			~CSummaryStat() {}

	CPeekString	Format() const;

	Peek::SummaryStatType	GetType() const { return m_Stat.ValueType; }
	Peek::SummaryStatValue	GetValue() const { return m_Stat.Value; }

	void		Reset() { memset( &m_Stat, 0, sizeof( m_Stat ) ); }
};


// =============================================================================
//		CSnapshot
// =============================================================================

class CSnapshot
{
protected:
	CHePtr<ISummaryStatsSnapshot>	m_spSnapshot;

protected:
	void			ReleasePtr() { m_spSnapshot.Release(); }
	void			SetPtr( void* inPtr ) {
		m_spSnapshot = reinterpret_cast<ISummaryStatsSnapshot*>( inPtr );
	}

public:
	;			CSnapshot( void* p = NULL ) { SetPtr( p ); }
	virtual		~CSnapshot() { ReleasePtr(); }

	SInt32		GetCount() const {
		SInt32	theCount = -1;
		if ( IsValid() ) {
			m_spSnapshot->GetCount( &theCount );
		}
		return theCount;
	}
	CGlobalId	GetId() const {
		CGlobalId	theId;
		if ( IsValid() ) {
			HeID	id;
			Peek::ThrowIfFailed( m_spSnapshot->GetID( &id ) );
			theId = id;
		}
		return theId;
	}
	CGlobalId	GetItem( SInt32 inItem ) const {
		CGlobalId	theId;
		if ( IsValid() ) {
			HeID	id;
			Peek::ThrowIfFailed( m_spSnapshot->GetItem( inItem, &id ) );
			theId = id;
		}
		return theId;
	}
	CPeekString	GetItemLabel( const GUID& inId ) const {
		CHeBSTR	bstrLabel;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spSnapshot->GetItemLabel( reinterpret_cast<const HeID&>( inId ), &bstrLabel.m_str ) );
		}
		return CPeekString( bstrLabel );
	}
	CGlobalId	GetItemParent( const GUID& inId ) const {
		CGlobalId	theParentId;
		if ( IsValid() ) {
			HeID	id;
			Peek::ThrowIfFailed( m_spSnapshot->GetItemParent( reinterpret_cast<const HeID&>( inId ), &id ) );
			theParentId = id;
		}
		return theParentId;
	}
	Peek::SummaryStat	GetItemValue( const GUID& inId ) const {
		Peek::SummaryStat	theStat;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spSnapshot->GetItemValue( reinterpret_cast<const HeID&>( inId ), reinterpret_cast<PeekSummaryStat*>( &theStat ) ) );
		}
		return theStat;

	}
	CPeekString	GetName() const {
		CHeBSTR	bstrLabel;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spSnapshot->GetName( &bstrLabel.m_str ) );
		}
		return CPeekString( bstrLabel );
	}

	bool		HasItem( const GUID& inId ) const {
		BOOL	theResult = FALSE;
		if ( IsValid() ) {
			Helium::HeCID	cid( *(reinterpret_cast<const Helium::HeCID*>( &inId )) );
			Peek::ThrowIfFailed( m_spSnapshot->HasItem( cid, &theResult ) );
		}
		return (theResult != FALSE);
	}

	bool		IsNotValid() const { return (m_spSnapshot == NULL); }
	bool		IsValid() const { return (m_spSnapshot != NULL); }

	void		SetItem( const GUID& inId, const GUID& inParentId, PCTSTR inLabel, const Peek::SummaryStat* inSummaryStat );

	void		UpdateSummaryStat( const GUID& inParentId, const Peek::SummaryStatEntry& inStatEntry, UInt32 nStringId, bool inNeedLabel );
};
