// ============================================================================
//	TargetAddress.h
//		interface for the CTargetAddress.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"
#include "FileEx.h"
#include <vector>

class CPeekDataModeler;
class CPeekDataElement;


// ============================================================================
//		CTargetAddress
// ============================================================================

class CTargetAddress
{
friend class CTargetAddressTraits;

public:
	enum AddrType {
		kTypeDomain,
		kTypeAddress
	};

protected:
	BOOL			m_bEnabled;
	AddrType		m_Type;
	CPeekStringX	m_strAddressX;

public:
	static bool	Validate( const CPeekStringA& inAddress ) {
		CPeekString str( inAddress );
		return Validate( str );
	}
	static bool	Validate( const CPeekString& inAddress );

public:
	bool		CompareAddress( const CPeekStringA& inAddress ) const {
		return (m_strAddressX.CompareNoCase( inAddress ) == 0);
	}

	const CPeekStringX&	Get( BOOL& outCheck ) const;
	const CPeekStringX&	GetAddress() const { return m_strAddressX; }
	AddrType			GetType() const { return m_Type; }

	CPeekString		Format() const;
	CPeekStringA	FormatA() const;

	bool		IsEnabled() const { return (m_bEnabled != FALSE); }
	bool		IsMatch( const CPeekString& inAddress ) const;

	void		Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;

	bool		Model( CPeekDataElement& ioAddress );

	void		Put( BOOL inCheck, CPeekStringX inAddress );
};


// ============================================================================
//		CTargetAddressList
// ============================================================================

class CTargetAddressList
	:	public std::vector<CTargetAddress>
{
	friend class CAddressPage;

protected:
	BOOL		m_bCaptureAll;

public:
	;		CTargetAddressList();
	;		CTargetAddressList( const CTargetAddressList& in ) { Copy( in ); }

	void	operator=( const CTargetAddressList& in ) { Copy( in ); }

	CPeekStringA	FormatLongA() const;
	CPeekString		Format() const;
	CPeekString		FormatCount() const;

	bool	Contains( const CPeekString& inStr ) const;
	void	Copy( const CTargetAddressList& in ) {
		m_bCaptureAll = in.m_bCaptureAll;
		assign( in.begin(), in.end() );
	}

	UInt32				GetCount32() const { return (size() < kIndex_Invalid) ? (size() & 0x0FFFFFFFF) : 0; }
	bool				GetFirst( CTargetAddress& outItem, size_t& outPos ) const;
	bool				GetNext( CTargetAddress& outItem, size_t& ioPos ) const;

	bool	HasAddress( const CPeekStringA& inStr, bool& outChecked ) const;

	bool	IsAddressOfInterest( const CPeekString& inStr ) const;
	bool	IsCaptureAll() const { return (m_bCaptureAll != FALSE); }
	bool	IsMatch( const CTargetAddressList& inAddressList ) const;

	void	Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;
	void	LogUpdate( const CPeekStringA& inPreAmble, const CTargetAddressList& inAddressList, CFileEx& inFile ) const;

	bool	Model( CPeekDataElement& ioAddressList );

	void	SetCaptureAll( bool bEnable ) { m_bCaptureAll = bEnable; }
};
