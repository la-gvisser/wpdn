// ============================================================================
//	PortNumber.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


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
	CPortNumber( bool inEnabled, CString inPort )
		:	m_bEnabled( inEnabled )
		,	m_nPort( (UInt16) _tstol( inPort ) ) {
	}
	~CPortNumber() {}

	CPortNumber( CString inStr ) {
		int nCurPos = 0;
		m_bEnabled = (_ttoi( inStr.Tokenize( _T(","), nCurPos ) ) != 0);
		m_nPort = (UInt16) _ttol( inStr.Tokenize( _T(","), nCurPos ) );
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
	static bool	Validate( CString& str ) {
		if ( str.GetLength() == 0 ) return false;

		long	nValue = _tstol( str );
		if ( nValue < 0 ) return false;
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
	void	Get( bool& outEnabled, CString& outPort ) {
		outEnabled = m_bEnabled;
		outPort.Format( _T("%u"), (UInt32) m_nPort );
	}
	CString	Format() {
		CString	str;
		str.Format( _T("%d,%u"), m_bEnabled, m_nPort );
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
	CString GetFormattedString() {
		CString theString;
		theString.Format( _T("%d"), m_nPort );
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

	bool operator==( CPortNumberList& inOptions ) {
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
	bool operator!=( CPortNumberList& inOptions ) {
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
		for ( size_t i = 0; i < GetCount(); i++ ) {
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
		for ( size_t i = 0; i < GetCount(); i++ ) {
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
	CString GetFormattedActivePortsString() {
		CString theString;
		bool bEnabledPortDisplayed = false;
		for ( size_t i = 0; i < GetCount(); i++ ) {
			CPortNumber* pPortNumber = GetAt( i );
			if ( pPortNumber->IsEnabled() ) {
				if ( bEnabledPortDisplayed ) theString += ", ";
				theString += pPortNumber->GetFormattedString();
				bEnabledPortDisplayed = true;
			}
		}
		return theString;
	}
	bool AreEnabledPorts(){
		for ( size_t i = 0; i < GetCount(); i++ ) {
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
		for ( size_t i = 0; i < inList.GetCount(); i++ ) {
			CPortNumber* theNewPortNumber = new CPortNumber;
			CPortNumber* theInPortNumber = inList.GetAt( i );
			*theNewPortNumber = *theInPortNumber;
			Add( theNewPortNumber );
		}
	}
};
