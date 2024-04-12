// ============================================================================
//	PortNumber.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "GlobalConstants.h"

// =============================================================================
//		CPortNumber
// =============================================================================

class CPortNumber
{

public:
	CPortNumber() : m_bEnabled( false ) {}
	CPortNumber( bool inEnabled, UInt16 inPort )
		:	m_bEnabled( inEnabled )
		,	m_nPort( inPort ) {
	}
	CPortNumber( bool inEnabled, CPeekString inPort )
		:	m_bEnabled( inEnabled )
		,	m_nPort( (UInt16) _tstol( inPort ) ) {
	}
	~CPortNumber() {}

	CPortNumber( CPeekString inStr ) {
		CString theStr( inStr );
		int nCurPos = 0;
		m_bEnabled = (_ttoi( theStr.Tokenize( L",", nCurPos ) ) != 0);
		m_nPort = (UInt16) _ttol( theStr.Tokenize( L",", nCurPos ) );
	}

	bool operator==( CPortNumber& inOptions ) {
		if ( m_nPort != inOptions.m_nPort || m_bEnabled != inOptions.m_bEnabled ) {
			return false;
		}
		return true;
	}
	bool operator!=( CPortNumber& inOptions ) {
		return !operator==(inOptions);
	}
	static bool	Validate( CPeekString& str ) {
		if ( str.GetLength() == 0 ) return false;

		long	nValue = _tstol( str );
		if ( nValue <= 0 ) return false;
		if ( nValue > 0x0000FFFF ) return false;

		return true;
	}
	void	Enable() {
		m_bEnabled = true;
	}
	void	Disable() {
		m_bEnabled = false;
	}
	void	SetEnable( bool in ) {
		m_bEnabled = in;
	}
	bool	IsEnabled() const {
		return m_bEnabled;
	}
	void	SetPort( UInt16 in ) {
		m_nPort = in;
	}
	UInt16	GetPort() const {
		return m_nPort;
	}
	void	Get( bool& outEnabled, CPeekString& outPort ) {
		outEnabled = m_bEnabled;
		CPeekOutString thePort;
		thePort << m_nPort;
		outPort = thePort;
	}
	CPeekString	Format() {
		CPeekOutString	str;
		str << m_bEnabled << m_nPort;
		return str;
	}
	CPortNumber& operator=( const CPortNumber& pIn ) {
		m_bEnabled = pIn.m_bEnabled;
		m_nPort = pIn.m_nPort;
		return *this;
	}
	bool	IsEnabledPort( UInt16 inPort ) {
		return (m_bEnabled) ? (m_nPort == inPort) : false;
	}
	CPeekString GetFormattedString() {
		CPeekOutString theString;
		theString << m_nPort;
		return theString;
	}

protected:
	bool	m_bEnabled;
	UInt16	m_nPort;

};


// =============================================================================
//		CPortNumberList
// =============================================================================

class CPortNumberList
	:	public CAtlArray<CPortNumber*>
{
public:
	;	CPortNumberList() {}
	;	CPortNumberList( CPortNumberList& in ) { CopyList( in ); }
	;	~CPortNumberList() {
			Clean();
		}

	void	Initialize() {
		Clean();
		Add( new CPortNumber( true, kDefaultPortThree ) );	
		Add( new CPortNumber( true, kDefaultPortFour ) );	
		Add( new CPortNumber( true, kDefaultPortOne ) );
		Add( new CPortNumber( true, kDefaultPortTwo ) );	
	}

	bool operator==( const CPortNumberList& inOptions ) {
		const size_t nCount = GetCount();
		if ( nCount != inOptions.GetCount() ) {
			return false;
		}
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* pMyPortNumber = GetAt( i );
			CPortNumber* pInPortNumber = inOptions.GetAt( i );
			if ( *pMyPortNumber != *pInPortNumber) {
				return false;
			}
		}
		return true;
	}
	bool operator!=( const CPortNumberList& inOptions ) {
		return !operator==(inOptions);
	}
	CPortNumberList& operator=( const CPortNumberList& inList ) {
		CopyList( inList );
		return *this;
	}

	void Clean() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ) {
			CPortNumber* pPortNumber = GetAt( i );
			ASSERT( pPortNumber );
			if ( pPortNumber ) {
				delete pPortNumber;
				RemoveAt( i );
			}
		}
		RemoveAll();
	}
	bool PortIsActive( UInt16 inPort1, UInt16 inPort2 ) {
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* pPortNumber = GetAt( i );
			ASSERT( pPortNumber );
			const UInt16 thePort = pPortNumber->GetPort();
			if (  thePort == inPort1 || thePort == inPort2 ) {
				if ( pPortNumber->IsEnabled() ) {
					return true;
				}
			}
		}
		return false;
	}
	bool PortIsActive( UInt16 inPort ) {
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* pPortNumber = GetAt( i );
			ASSERT( pPortNumber );
			const UInt16 thePort = pPortNumber->GetPort();
			if (  thePort == inPort ) {
				if ( pPortNumber->IsEnabled() ) {
					return true;
				}
			}
		}
		return false;
	}
	CPeekString GetFormattedActivePortsString() {
		CPeekString theString;
		bool bEnabledPortDisplayed = false;
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* pPortNumber = GetAt( i );
			if ( pPortNumber->IsEnabled() ) {
				if ( bEnabledPortDisplayed ) theString += L", ";
				theString += pPortNumber->GetFormattedString();
				bEnabledPortDisplayed = true;
			}
		}
		return theString;
	}
	bool AreEnabledPorts(){
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* pPortNumber = GetAt( i );
			if ( pPortNumber->IsEnabled() ) {
				return true;
			}
		}
		return false;
	}

protected:
	void CopyList( const CPortNumberList& inList ) {
		Clean();
		const size_t nCount = inList.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPortNumber* theNewPortNumber = new CPortNumber;
			CPortNumber* theInPortNumber = inList.GetAt( i );
			*theNewPortNumber = *theInPortNumber;
			Add( theNewPortNumber );
		}
	}
};
