// =============================================================================
//	Snapshot.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "Peek.h"
#include "peekstats.h"
#include "GlobalId.h"


// =============================================================================
//		CSummaryStat
// =============================================================================

class CSummaryStat
{
	friend class CSummaryStatItem;

protected:
	Peek::SummaryStat	m_Stat;

public:
	;			CSummaryStat() { Reset(); }
	;			CSummaryStat( const Peek::SummaryStat& inOther ) : m_Stat( inOther ) {}
	;			~CSummaryStat() {}

#ifdef _WIN32
	CPeekString	Format() const;
#endif

	Peek::SummaryStatType	GetType() const { return m_Stat.Type; }
	Peek::SummaryStatValue	GetValue() const { return m_Stat.Value; }

	void		Reset() { memset( &m_Stat, 0, sizeof( m_Stat ) ); }
};


// =============================================================================
//		CSummaryStatItem
// =============================================================================

class CSummaryStatItem
{
	friend class CSnapshot;

protected:
	CHePtr<ISummaryStatsItem2>	m_spItem;

protected:
	void			ReleasePtr() { m_spItem.Release(); }
	void			SetPtr( void* inPtr ) {
		m_spItem = reinterpret_cast<ISummaryStatsItem2*>( inPtr );
	}

public:
	;			CSummaryStatItem( void* p = nullptr ) { SetPtr( p ); }
	virtual		~CSummaryStatItem() { ReleasePtr(); }

	CGlobalId	GetId() {
		CGlobalId	id;
		if ( IsValid () ) {
			Helium::HeID	idHelium;
			Peek::ThrowIfFailed( m_spItem->GetID( &idHelium ) );
			id = idHelium;
		}
		return id;
	}
	CPeekString	GetLabel() {
		CHeBSTR	bstrLabel;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spItem->GetLabel( &bstrLabel.m_str ) );
		}
		return CPeekString( bstrLabel );
	}
	CGlobalId	GetParent() {
		CGlobalId	id;
		if ( IsValid () ) {
			Helium::HeID	idHelium;
			Peek::ThrowIfFailed( m_spItem->GetParent( &idHelium ) );
			id = idHelium;
		}
		return id;
	}
	CSummaryStat	GetValue() {
		CSummaryStat	stat;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spItem->GetValue(
				reinterpret_cast<PeekSummaryStat2*>( &stat.m_Stat ) ) );
		}
		return stat;
	}

	void	SetLabel( const CPeekString& inLabel ) {
		if ( IsValid() ) {
			CHeBSTR	bstrLabel( inLabel.c_str() );
			Peek::ThrowIfFailed( m_spItem->SetLabel( bstrLabel ) );
		}
	}
	void	SetValue( const CSummaryStat& inValue ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spItem->SetValue(
				reinterpret_cast<const PeekSummaryStat2*>( &inValue.m_Stat ) ) );
		}
	}

	bool		IsNotValid() const { return (m_spItem.get() == nullptr); }
	bool		IsValid() const { return (m_spItem.get() != nullptr); }
};


// =============================================================================
//		CSnapshot
// =============================================================================

class CSnapshot
{
	friend class CStatsHandler;

protected:
	CHePtr<ISummaryStatsSnapshot2>	m_spSnapshot;

protected:
	ISummaryStatsSnapshot2*	GetPtr() { return m_spSnapshot; }
	void					ReleasePtr() { m_spSnapshot.Release(); }
	void					SetPtr( void* inPtr ) {
		m_spSnapshot = reinterpret_cast<ISummaryStatsSnapshot2*>( inPtr );
	}

public:
	;			CSnapshot( void* p = nullptr ) { SetPtr( p ); }
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
			Peek::ThrowIfFailed( m_spSnapshot->GetItemLabel(
				reinterpret_cast<const HeID&>( inId ), &bstrLabel.m_str ) );
		}
		return CPeekString( bstrLabel );
	}
	CGlobalId	GetItemParent( const GUID& inId ) const {
		CGlobalId	theParentId;
		if ( IsValid() ) {
			HeID	id;
			Peek::ThrowIfFailed( m_spSnapshot->GetItemParent(
				reinterpret_cast<const HeID&>( inId ), &id ) );
			theParentId = id;
		}
		return theParentId;
	}
 	Peek::SummaryStat	GetItemValue( const GUID& inId ) const {
 		Peek::SummaryStat	theStat;
 		if ( IsValid() ) {
 			Peek::ThrowIfFailed( m_spSnapshot->GetItemValue(
				reinterpret_cast<const HeID&>( inId ),
				reinterpret_cast<PeekSummaryStat2*>( &theStat ) ) );
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
	CSummaryStatItem	GetStatsItem( const GUID& inId ) const {
		CSummaryStatItem	item;
		if ( IsValid() ) {
			Helium::HeCID	cid( *(reinterpret_cast<const Helium::HeCID*>( &inId )) );
			CHePtr<ISummaryStatsItem2>	spStatsItem;
			Peek::ThrowIfFailed( m_spSnapshot->GetStatsItem( cid, &spStatsItem.p ) );
			item.SetPtr( spStatsItem.get() );
		}
		return item;
	}

	bool		HasItem( const GUID& inId ) const {
		BOOL	theResult = FALSE;
		if ( IsValid() ) {
			Helium::HeCID	cid( *(reinterpret_cast<const Helium::HeCID*>( &inId )) );
			Peek::ThrowIfFailed( m_spSnapshot->HasItem( cid, &theResult ) );
		}
		return (theResult != FALSE);
	}

	bool		IsNotValid() const { return (m_spSnapshot.get() == nullptr); }
	bool		IsValid() const { return (m_spSnapshot.get() != nullptr); }

	void		SetItem( const GUID& inId, const GUID& inParentId, const wchar_t* inLabel,
					const Peek::SummaryStat* inSummaryStat );

	void		UpdateSummaryStat( const GUID& inParentId, const Peek::SummaryStatEntry& inStatEntry );
};
