// =============================================================================
//	PluginXml.h
// =============================================================================
//	Copyright (c) 1997-2006 WildPackets, Inc. All rights reserved.

#pragma once

#include "Plugin.h"

using namespace Plugin;

class CXmlDocument;


// =============================================================================
//		CXmlElement
// =============================================================================

class CXmlElement
{
	friend class CXmlDocument;

protected:
	void*			m_pvElement;
	CXmlDocument*	m_pDocument;

	void		ReleasePtr();
	void		SetParent( CXmlDocument* pParent ) { m_pDocument = pParent; }
	void		SetPtr( void* p );

public:
	;			CXmlElement( CXmlDocument* pDoc = NULL, void* p = NULL );
	virtual		~CXmlElement();

	int			AddChild( CString strLabel, CXmlElement& xmlChild );
	int			AddChild( CString strLabel, CString strValue );
	int			AddChild( CString strLabel, bool bValue ) {
		CString	strValue = (bValue) ? L"1" : L"0";
		return AddChild( strLabel, strValue );
	}
	int			AddChild( CString strLabel, UInt32 nValue ) {
		CString	strValue;
		strValue.Format( L"%u", nValue );
		return AddChild( strLabel, strValue );
	}
	int			AddChildEnabled( CString strLabel, bool bEnabled );

	int			GetChild( CString strLabel, CXmlElement& xmlChild );
	int			GetChildValue( CString strLabel, UInt32 nValue );
	int			GetDocument( CXmlDocument** ppDocument ) {
		*ppDocument = m_pDocument;
		return PEEK_PLUGIN_SUCCESS;
	}

	int			IsChildEnabled( CString strLabel, bool& bEnabled );
	bool		IsNull() { return (m_pvElement == NULL); }

	int			Set( CString strValue );
	int			Set( UInt32 nValue );
	
};


// =============================================================================
//		CXmlDocument
// =============================================================================

class CXmlDocument
{
	friend	class  CXmlElement;

protected:
	void*		m_pvDocument;

protected:
	void		ReleasePtr();
	void		SetPtr( void* p );

public:
	;			CXmlDocument( void* p = NULL );
	virtual		~CXmlDocument();

	int			CreateElement( CString strLabel, CXmlElement& xmlElement );

	void*		GetDocument() { return m_pvDocument; }

	bool		IsNull() { return (m_pvDocument == NULL); }

	int			Load(const wchar_t*	pszFile);

	int			Save(const wchar_t*	pszFile);
};
