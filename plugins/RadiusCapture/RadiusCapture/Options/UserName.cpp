// ============================================================================
//	UserName.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "UserName.h"


///////////////////////////////////////////////////////////////////////////////
//		CUserName
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Get
// -----------------------------------------------------------------------------

void
CUserName::Get(
	BOOL*			outCheck,
	CPeekString&	outName )
{
	if ( outCheck ) *outCheck = m_bCheck;
	outName = m_strName;
}


// -----------------------------------------------------------------------------
//		Put
// -----------------------------------------------------------------------------

void
CUserName::Put(
	BOOL				inCheck,
	const CPeekString&	inName )
{
	m_bCheck = inCheck;
	m_strName = inName;
}

// -----------------------------------------------------------------------------
//		IsMatch
// -----------------------------------------------------------------------------

bool
CUserName::IsMatch(
	const CPeekString&	inUserName ) const
{
	if ( m_bCheck ) {
		return (m_strName.CompareNoCase( inUserName ) == 0);
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
//		CUserItemArray
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		GetFirst
// -----------------------------------------------------------------------------

int
CUserItemArray::GetFirst() const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		if ( GetAt( i ).m_bCheck ) {
			return static_cast<int>( i );
		}
	}
	return -1;
}


// -----------------------------------------------------------------------------
//		GetNext
// -----------------------------------------------------------------------------

int
CUserItemArray::GetNext(
	int			inPos,
	CUserName&	outItem ) const
{
	if ( (inPos < 0) || (inPos > (int) GetCount()) )  return -1;

	outItem = GetAt( inPos );

	for ( size_t i = (inPos + 1); i < GetCount(); i++ ) {
		if ( GetAt( i ).m_bCheck ) {
			return static_cast<int>( i );
		}
	}
	return -1;
}


///////////////////////////////////////////////////////////////////////////////
//		CUserNameList
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Contains
// -----------------------------------------------------------------------------

bool
CUserNameList::Contains(
	const CPeekString&	inStr ) const
{
	for ( size_t i = 0; i < m_List.GetCount(); i++ ) {
		if ( m_List[i].IsMatch( inStr ) ) {
			return true;
		}
	}
	return false;
}


// -----------------------------------------------------------------------------
//		HasUserName
// -----------------------------------------------------------------------------

bool
CUserNameList::HasUserName(
	const CPeekString&	inStr,
	bool&			outChecked ) const
{
	for ( size_t i = 0; i < m_List.GetCount(); i++ ) {
		if ( m_List[i].CompareUserName( inStr ) ) {
			outChecked = m_List[i].IsEnabled();
			return true;
		}
	}
	return false;
}

// -----------------------------------------------------------------------------
//		HasAssociatedName
// -----------------------------------------------------------------------------

bool
CUserNameList::HasAssociatedName(
   const CPeekString&	inStr ) const
{
	for ( size_t i = 0; i < m_List.GetCount(); i++ ) {
		if ( m_List[i].CompareUserName( inStr ) ) {
			return true;
		}
	}
	return false;
}

// -----------------------------------------------------------------------------
//		UserIsActiveTarget
// -----------------------------------------------------------------------------

bool
CUserNameList::UserIsActiveTarget(
	const CPeekString&	inStr ) const
{
	const size_t nCount = m_List.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		if ( m_List[i].CompareUserName( inStr ) ) {
			if ( m_List[i].IsEnabled() ) {
				return true;
			}
		}
	}
	return false;
}


// -----------------------------------------------------------------------------
//		AreEnabledUsers
// -----------------------------------------------------------------------------

bool
CUserNameList::AreEnabledUsers()
{
	for ( size_t i = 0; i < m_List.GetCount(); i++ ) {
		if ( m_List[i].IsEnabled() ) {
			return true;
		}
	}
	return false;
}

// -----------------------------------------------------------------------------
//		GetTargetListString
// -----------------------------------------------------------------------------

CPeekString
CUserNameList::GetTargetListString() {
	CPeekString strTargetList = L"";
	bool	bHaveMember = false;
	for ( size_t i = 0; i < m_List.GetCount(); i++ ) {
		if ( m_List[i].IsEnabled() ) {
			if ( bHaveMember ) strTargetList += _T(", ");
			strTargetList += m_List[i].GetName();
			bHaveMember = true;
		}
	}
	return strTargetList;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CUserNameList::Model(
	CPeekDataElement& ioElement )
{
	const bool bIsLoading = ioElement.IsLoading();
	const bool bIsStoring = ioElement.IsStoring();

	size_t nCount( 0 );

	if ( bIsStoring ) {
		nCount = GetCount();
	}
	else {
		nCount = ioElement.GetChildCount();
		Reset();
	}

	CPeekString	strName;
	bool		bEnabled( false );

	for ( size_t i = 0; i < nCount; i++ ) {

		if ( bIsStoring ) {
			bEnabled = m_List[i].IsEnabled();
			strName = m_List[i].GetName();
		}

		CPeekDataElement elemUser( L"User", ioElement, i );
		if ( elemUser ) {
			elemUser.Enabled( bEnabled );
			elemUser.Attribute( L"Name", strName );
		}

		if ( bIsLoading ) {
			m_List.Add();
			m_List[i].SetEnabled( bEnabled );
			m_List[i].SetName( strName );
		}
	}

	return true;
}
