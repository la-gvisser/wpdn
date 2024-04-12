// ============================================================================
//	ActiveLists.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaseOptionsList.h"
#include "afxdtctl.h"

// =============================================================================
//		CActiveRadiusPortsList
// =============================================================================

class CActiveRadiusPortsList
	: public CAtlArray <UInt16>
{
public:

	CActiveRadiusPortsList() {}
	~CActiveRadiusPortsList() {
		Reset();
	}

	void	RebuildList( CCaseOptionsList& inCaseOptionsList );

	void	Reset() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ) {
			RemoveAt( i );
		}
		RemoveAll();
	}
	UInt16	GetRadiusPort( UInt16 inPort1, UInt16 inPort2 ) {
			for ( size_t i = 0; i < GetCount(); i++ ) {
				UInt16 thePort = GetAt( i );
				if ( thePort == inPort1 || thePort == inPort2 ) {
					return thePort;
				}
			}
			return 0;
	}
	bool	IsInList( UInt16 inPort ) {
		for ( size_t i = 0; i < GetCount(); i++ ) {
			UInt16 thePort = GetAt( i );
			if ( thePort == inPort ) {
				return true;
			}
		}
		return false;
	}
	bool	IsThereAnActivePort() {
		if ( GetCount() > 0 ) return true;
		return false;
	}
	bool	IsMatch( UInt16 thePort ){
		for ( size_t i = 0; i < GetCount(); i++ ) {
			if ( GetAt( i ) == thePort ) return true;
		}
		return false;
	}
};


// =============================================================================
//		CActiveUser
// =============================================================================

class CActiveUser
{
public:

	CActiveUser() { ASSERT(0); } // Never called but required for compilation
	CActiveUser( const CPeekString& theUserName ) 
		: m_UserName( theUserName ) {}
	CActiveUser( const CActiveUser& theActiveUser ) {
		m_UserName = theActiveUser.m_UserName;
	}

	bool operator==( CActiveUser& inActiveUser ) {
		if ( m_UserName == inActiveUser.m_UserName ) return true;
		return false;
	}
	CPeekString GetUserName() {
		return m_UserName;
	};

protected:
	CPeekString				m_UserName;
};


// =============================================================================
//		CActiveUsersList
// =============================================================================

class CActiveUsersList
	: public CAtlArray <CActiveUser>
{
public:

	CActiveUsersList() {}
	~CActiveUsersList() {
		Reset();
	}

	void RebuildList( CCaseOptionsList& theCaseOptionsList );

	void Reset() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ) {
			RemoveAt( i );
		}
		RemoveAll();
	}

//	bool IsUserNameOfInterest( const CPeekString& theUserName ) {
//		return IsUserActive( theUserName );
//	}

	bool IsUserAnActiveTarget( const CPeekString& inUserName ){
		for ( size_t i = 0; i < GetCount(); i++ ) {
			CActiveUser& theActiveUser = GetAt( i );
			CPeekString theUserListName = theActiveUser.GetUserName();
			if ( theUserListName.CompareNoCase( inUserName ) == 0 ) return true;
		}
		return false;
	}
};


// =============================================================================
//		CActiveTimeSpan
// =============================================================================

class CActiveTimeSpan
{
public:

	CActiveTimeSpan()
		: m_EarliestStartTime( 0 )
		, m_LatestStopTime( 0 ){}

	UInt64 GetCurrentSystemTime() {
		UInt64	nCurrentTime;
		::GetSystemTimeAsFileTime( (LPFILETIME) &nCurrentTime );
		return nCurrentTime;
	}
	void SetEarliestStartTime( UInt64 theStartTime ){
		if ( theStartTime < m_EarliestStartTime || m_EarliestStartTime == 0 ) {
			m_EarliestStartTime = theStartTime;
		}
	}
	void SetLatestStopTime( UInt64 theEndTime ){
		if ( theEndTime > m_LatestStopTime || m_LatestStopTime == 0 ) {
			m_LatestStopTime = theEndTime;
		}
	}
	bool IsTimeActive() {
		UInt64 now = GetCurrentSystemTime();
		return ( now < m_LatestStopTime );
	}
	void InitializeActiveTimeSpan() {
		m_EarliestStartTime = 0;
		m_LatestStopTime = 0;
	}
#ifdef SCREEN_INITIAL_START_STOP_TIMES
	bool IsTimeOfInterest(){
		UInt64 theTimeStamp = ::GetTimeStamp();
		if ( theTimeStamp >= m_EarliestStartTime  && theTimeStamp <= m_LatestStopTime ) return true;
		return false;
	}
#endif

protected:
	UInt64 m_EarliestStartTime;
	UInt64 m_LatestStopTime;
};
