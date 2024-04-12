// ============================================================================
//	Domain.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "IpHeaders.h"

// ============================================================================
//		CDomain
// ============================================================================

class CDomain :
	public CPeekString
{
public:
	;		CDomain() {}
	;		CDomain( CPeekString inName ) : CPeekString( inName ) {}
	;		~CDomain() {}

	bool	IsMatch( CPeekString inAddress ) const;
	bool	IsWildCard() const { return (GetAt( 0 ) == L'*'); }
	bool	Validate() const;
};


// ============================================================================
//		CDomainList
// ============================================================================

class CDomainList
	:	public CAtlArray<CDomain>
{
public:
	;		CDomainList() {}
	;		~CDomainList() {}

	CDomainList& operator=( const CDomainList& inOther ) {
		Copy( inOther );
		return *this;
	}

	bool	IsEqual( const CDomainList& inOther ) {
		const size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CDomain& theDomain( GetAt( i ) );
			if ( inOther.GetAt(i).CompareNoCase( theDomain ) != 0 ) {
				return false;
			}
		}
		return true;
	}

	bool	Find( CPeekString inName, size_t& outIndex ) const {
		const size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CDomain& theDomain( GetAt( i ) );
			if ( inName.CompareNoCase( theDomain ) == 0 ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	bool	IsMatch( CPeekString inName ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CDomain& theDomain( GetAt( i ) );
			if ( theDomain.IsMatch( inName ) ) return true;
		}
		return false;
	}

	void	Reset() {
		RemoveAll();
	}
};


// ============================================================================
//		CDomainCounted
// ============================================================================

class CDomainCounted
{
public:
	enum kType_Count {
		kType_Query,
		kType_Response
	};

protected:
	CDomain		m_Name;
	UInt32		m_nQueryCount;
	UInt32		m_nResponseCount;

public:
	;			CDomainCounted() : m_nQueryCount( 0 ), m_nResponseCount( 0 ) {}
	;			CDomainCounted( CPeekString inName ) : m_Name( inName ), m_nQueryCount( 0 ), m_nResponseCount( 0 ) {}
	;			~CDomainCounted() {}

	CDomainCounted& operator=( const CDomainCounted& inOther ) {
		Copy( inOther );
		return *this;
	}

	void		Copy( const CDomainCounted& inOther ) {
		m_Name = inOther.m_Name;
		m_nQueryCount = inOther.m_nQueryCount;
		m_nResponseCount = inOther.m_nResponseCount;
	}

	void		Increment( kType_Count inType ) {
		if ( inType == kType_Query ) {
			m_nQueryCount++;
		}
		else {
			m_nResponseCount++;
		}
	}

	UInt32		GetQueryCount() const { return m_nQueryCount; }
	CPeekString	GetName() const { return m_Name; }
	UInt32		GetResponseCount() const { return m_nResponseCount; }


	bool		IsMatch( CPeekString inAddress ) const;

	void		SetName( CPeekString& inName ) { m_Name = inName; }

	// For console side view tab display:
	void		SetQueryCount( UInt32 inCount ) { m_nQueryCount = inCount; }
	void		SetResponseCount( UInt32 inCount ) { m_nResponseCount = inCount; }
};


// ============================================================================
//		CDomainCountedList
// ============================================================================

class CDomainCountedList
	:	public CAtlArray<CDomainCounted>
{
public:
	;		CDomainCountedList() {}
	;		~CDomainCountedList() {}

	CDomainCountedList& operator=( const CDomainCountedList& inOther ) {
		Copy( inOther );
		return *this;
	}
	CDomainCountedList& operator=( const CDomainList& inOther );

	size_t	AddUnique( CPeekString inName ) {
		size_t	nIndex = 0;
		if ( Find( inName, nIndex ) ) {
			CDomainCounted&	theDomain( GetAt( nIndex ) );
			if ( inName.Compare( theDomain.GetName() ) != 0 ) {
				theDomain.SetName( inName );
			}
			return nIndex;
		}
		return static_cast<int>( Add( inName ) );
	}

	bool	Find( CPeekString inName, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CDomainCounted& theDomain( GetAt( i ) );
			if ( inName.CompareNoCase( theDomain.GetName() ) == 0 ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	bool	IsMatch( CPeekString inName, CDomainCounted::kType_Count inType ) {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CDomainCounted& theDomain( GetAt( i ) );
			if ( theDomain.IsMatch( inName ) ) {
				theDomain.Increment( inType );
				return true;
			}
		}
		return false;
	}

	void	PruneList( const CDomainList& inNames );

	void	Reset() {
		RemoveAll();
	}
};


// ============================================================================
//		CResolvedDomain
// ============================================================================

class CResolvedDomain
{

protected:
	CPeekString			m_strName;
	CIpAddressList		m_AddressList;
	mutable CPeekString	m_strAddressList;

public:
	;			CResolvedDomain() {}
	;			CResolvedDomain( const CResolvedDomain& inOther ) { Copy( inOther ); }
	;			~CResolvedDomain() {}

	CResolvedDomain&	operator=( const CResolvedDomain& inOther ) { Copy( inOther ); return *this; }

	void			Add( const CIpAddress& inItem ) {
		m_AddressList.Add( inItem );
		EmptyAddressListString(); 
	}

//	void			AppendAddresses( CIpAddressList& outList ) { outList.Append( m_AddressList ); }

	void			Copy( const CResolvedDomain& inOther ) {
		m_strName = inOther.m_strName;
		m_AddressList.Copy( inOther.m_AddressList );
		m_strAddressList = inOther.FormatAddressList();	// force the Other to generate the string.
	}

	void			EmptyAddressListString() { m_strAddressList.Empty(); }

	CPeekString	FormatAddressList() const {
		if ( m_strAddressList.IsEmpty() ) {
			m_strAddressList = m_AddressList.Format();
		}
		return m_strAddressList;
	}

	const CIpAddressList&	GetAddressList() const { return m_AddressList; }

	CPeekString		GetAddressListString() const { return m_strAddressList; }
	size_t			GetCount() const { return m_AddressList.GetCount(); }
	CIpAddress		GetItem( size_t inIndex ) { return m_AddressList.GetAt( inIndex ); }
	CPeekString		GetName() const { return m_strName; }


	bool			HasAddress( CIpAddress inAddress ) const { size_t i; return m_AddressList.Find( inAddress, i ); }

	bool			IsEmpty() const { m_AddressList.IsEmpty(); }

	void			SetAddressList( const CIpAddressList& inList ) { 
		m_AddressList.Copy( inList ); 
		EmptyAddressListString(); 
	}
	void			SetAddressString( CPeekString& inAddressString ){
		m_strAddressList = inAddressString;
	}
	void			SetName( CPeekString inName ) { m_strName = inName; }
};


// ============================================================================
//		CResolvedDomainList
// ============================================================================

class CResolvedDomainList
	:	public CAtlArray<CResolvedDomain>
{
protected:
public:
	;		CResolvedDomainList() {}
	;		~CResolvedDomainList() {}

	CResolvedDomainList& operator=( const CResolvedDomainList& inOther ) {
		Copy( inOther );
		return *this;
	}

	void	AddAddress( const CIpAddress& inAddress ) {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CResolvedDomain&	theDomain( GetAt( i ) );
			theDomain.Add( inAddress );
		}
	}

	void	EmptyStrings() {
		const size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CResolvedDomain&	theDomain( GetAt( i ) );
			theDomain.EmptyAddressListString();
		}
	}

	bool	Find( const CPeekString& inName, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CResolvedDomain&	theDomain( GetAt( i ) );
			if ( inName.CompareNoCase( theDomain.GetName() ) == 0 ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	void	GetAddresses( CIpAddressList& outList ) {
		outList.RemoveAll();
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CResolvedDomain&	theDomain( GetAt( i ) );
			outList.AddUnique( theDomain.GetAddressList() );
		}
	}

	bool	HasAddress( const CIpAddress& inAddress ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CResolvedDomain&	theDomain( GetAt( i ) );
			if ( theDomain.HasAddress( inAddress ) ) return true;
		}
		return false;
	}

	void	PruneList( const CDomainList& inNames );

	void	Reset() {
		RemoveAll();
	}

	size_t	UpdateAddUnique( const CResolvedDomain& inResolved );
};
