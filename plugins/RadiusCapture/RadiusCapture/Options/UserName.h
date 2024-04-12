// ============================================================================
//	UserName.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekDataModeler.h"

#define kPref_Users	_T( "Users" )
#define kPref_User	_T( "User" )

///////////////////////////////////////////////////////////////////////////////
//		CUserName
///////////////////////////////////////////////////////////////////////////////

class CUserName
{

	friend class CUserItemArray;

public:
	CUserName() {}
	~CUserName() {}

	bool operator==( const CUserName& inUserName ) const {
		if ( CompareUserName( inUserName.m_strName ) == false ) return false;
		if ( m_bCheck != inUserName.m_bCheck ) return false;
		return true;
	}
	bool operator!=( const CUserName& inUserName ) const {
		return !operator==(inUserName);
	}

	void Get( BOOL* outCheck, CPeekString& outName );
	void Put( BOOL inCheck, const CPeekString& inName );
	bool IsMatch( const CPeekString& inUserName ) const;

	void SetName( CPeekString strName ) {
		m_strName = strName;
	}
	bool IsEnabled() const {
		return (m_bCheck != FALSE);
	}
	void SetEnabled( bool bEnabled ) {
		m_bCheck = bEnabled;
	}
	bool CompareUserName( const CPeekString& inUserName ) const {
		return (m_strName.CompareNoCase( inUserName ) == 0);
	}

	const CPeekString&	GetName() const {
		return m_strName;
	}

protected:
	BOOL			m_bCheck;
	CPeekString		m_strName;

};

// -----------------------------------------------------------------------------
//		CUserItemArray
// -----------------------------------------------------------------------------

class CUserItemArray
	:	public CAtlArray<CUserName/*, CUserItemTraits*/>
{
public:
	;		CUserItemArray(){}
	;		CUserItemArray( const CUserItemArray& in ){ Copy( in ); }
	;		~CUserItemArray() {}
	void	operator=( const CUserItemArray& in ){ Copy( in ); }
	int		GetFirst() const;
	int		GetNext( int inPos, CUserName& outItem ) const;
};


///////////////////////////////////////////////////////////////////////////////
//		CUserNameList
///////////////////////////////////////////////////////////////////////////////

class CUserNameList
{

public:	
	CUserNameList() {}
	~CUserNameList() {}

	operator CUserItemArray() {
		return m_List;
	}
	void	operator=( const CUserItemArray& in ) {
		Reset();
		m_List.Copy( in );
	}
	void	operator=( const CUserNameList& in ) {
		Reset();
		m_List.Copy( in.m_List );
	}
	bool	operator==( const CUserNameList& in ) const {
		if ( GetCount() != in.GetCount() ) return false;
		for ( size_t i = 0; i < in.GetCount(); i++ ) {
			const CUserName& inUserName = in.m_List.GetAt( i );
			const CUserName& myUserName = m_List.GetAt( i );
			if ( inUserName != myUserName ) return false;
		}
		return true;
	}
	bool	operator!=( const CUserNameList& in ) const {
		return !operator==( in );
	}

	bool	Model( CPeekDataElement& ioElement );

	void	SetCaptureAll( bool inCaptureAll ) { m_bCaptureAll = inCaptureAll; }
	CPeekString GetTargetListString();
	bool	Contains( const CPeekString& inStr ) const;
	bool	HasUserName( const CPeekString& inStr, bool& outChecked ) const;
	bool	HasAssociatedName( const CPeekString& inStr ) const;
	bool	UserIsActiveTarget( const CPeekString& inStr ) const;
	bool	AreEnabledUsers();

	bool	SetList( const CUserItemArray& inList ) {
			m_List = inList;
			return true;
	}
	bool	CaptureAll() const {
		return m_bCaptureAll;
	}
	size_t	GetCount() const {
		return m_List.GetCount();
	}
	const CUserName& GetAt( size_t i ) {
		return m_List.GetAt( i );
	}
	void	Reset() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ){
			m_List.RemoveAt( i );
		}
		m_List.RemoveAll();
	}

protected:
	bool			m_bCaptureAll;
	CUserItemArray	m_List;

};
