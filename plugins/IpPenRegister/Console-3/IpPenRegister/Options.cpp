// ============================================================================
//	Options.cpp
//		implementation of the COptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "Utils.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CString	g_strConfigFileName(
	_T( "\\plugins\\IpPenRegisterConfig.xml" ) );


// ============================================================================
//		Tags
// ============================================================================

namespace Tags
{
	const CString	kWPOmniPeekPlugins( _T( "WildPackets\\OmniPeek\\Plugins" ) );
	const CString	kAppData( _T( "APPDATA" ) );
	const CString	kConfigFileName( _T( "Config.xml" ) );
	const CString	kChangeLogFileName( _T( "ChangeLog.txt" ) );
	const CString	kDefaultVersion( _T( "1.0.0.0" ) );
	const CString	kVersionTemplate( _T( "%u.%u.%u.%u" ) );
}


// ============================================================================
//		XML Tags
// ============================================================================

namespace ConfigTags
{
	const CString	kElem_Option( _T( "IpPenRegister" ) );
	const CString	kElem_Version( _T( "Version" ) );
}

// ============================================================================
//		COptionsGeneral
// ============================================================================

// ----------------------------------------------------------------------------
//		Read
// ----------------------------------------------------------------------------

void
COptions::COptionsGeneral::Read(
	CPeekXmlElement&	inElement )
{
	if ( inElement == NULL ) return;

}


// ----------------------------------------------------------------------------
//		Write
// ----------------------------------------------------------------------------

void
COptions::COptionsGeneral::Write(
	CPeekXmlElement&	inElement )
{
	if ( inElement == NULL ) return;

}




// ============================================================================
//		COptions
// ============================================================================

// ----------------------------------------------------------------------------
//		LoadPrefs
// ----------------------------------------------------------------------------

void
COptions::LoadPrefs(
	CPeekXmlDocument&	inXmlDoc )
{
	if ( inXmlDoc != NULL ) {
		CPeekXmlElement	Root;

		Root = inXmlDoc.GetRootElement();
		if ( Root != NULL ) {
			CString	strVersion;
			Root.GetAttributeValue( ConfigTags::kElem_Version, strVersion );

			// To Do: Validate Version
			m_General.Read( Root );
		}
	}
}


// ----------------------------------------------------------------------------
//		StorePrefs
// ----------------------------------------------------------------------------

void
COptions::StorePrefs(
	CPeekXmlDocument&	inXmlDoc )
{
	CPeekXmlElement		Root = inXmlDoc.AddRootElement( ConfigTags::kElem_Option );
	if ( Root != NULL ) {
		CString	strVersion = Tags::kDefaultVersion;
		ENV_VERSION	evFile;
		ENV_VERSION	evProduct;
		if ( GetPluginVersion( evFile, evProduct) ) {
				strVersion.Format(
					Tags::kVersionTemplate,
					evFile.MajorVer,
					evFile.MinorVer,
					evFile.MajorRev,
					evFile.MinorRev );
		}
		Root.AddAttribute( ConfigTags::kElem_Version, strVersion );

		m_General.Write( Root );
	}
}


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

void
COptions::Init(
	CPeekPlugin*	inPlugin )
{
	ASSERT( inPlugin != NULL );
	if ( inPlugin == NULL ) return;

	m_pPlugin = inPlugin;

	//CString	strPath;
	//GetModuleFileName(
	//	m_pPlugin->GetInstanceHandle(),	strPath.GetBuffer( MAX_PATH ), MAX_PATH );
	//strPath.ReleaseBuffer();

	//LPCTSTR	pszPath = strPath.GetBuffer();
	//LPCTSTR	pszFileName = PathFindFileName( pszPath );
	//LPCTSTR	pszExtension = PathFindExtension( pszPath );
	//int		nPathLength = pszFileName - pszPath;
	//int		nFileNameLength = pszExtension - pszFileName;

	//CString	strConfigFileName;
	//CString	strChangeLogFileName;
	//strConfigFileName = strPath.Mid( nPathLength, nFileNameLength );
	//strChangeLogFileName = strConfigFileName;
	//strConfigFileName.Append( Tags::kConfigFileName );
	//strChangeLogFileName.Append( Tags::kChangeLogFileName );

	//if ( IsWindowsType( kWindows_Vista ) ) {
	//	// For Vista create a paths within the User's AppData\Roaming directory.
	//	CString	strAltPathName;
	//	strAltPathName.GetEnvironmentVariable( Tags::kAppData );
	//	if ( !strAltPathName.IsEmpty() ) {
	//		CString	strAltPath;
	//		strAltPath.Format(
	//			_T( "%s\\%s" ),
	//			strAltPathName,
	//			Tags::kWPOmniPeekPlugins );
	//		MakePath( strAltPath );

	//		CString	strAltConfigFileName;
	//		strAltConfigFileName.Format(
	//			_T( "%s\\%s" ),
	//			strAltPath,
	//			strConfigFileName );
	//		m_strConfigFileName = strAltConfigFileName;

	//		CString	strAltChageLogFileName;
	//		strAltChageLogFileName.Format(
	//			_T( "%s\\%s" ),
	//			strAltPath,
	//			strChangeLogFileName );
	//		m_strChangeLogFileName = strAltChageLogFileName;
	//	}
	//}
	//else {
	//	strPath = strPath.Left( nPathLength );

	//	m_strConfigFileName = strPath + strConfigFileName;
	//	m_strChangeLogFileName = strPath + strChangeLogFileName;
	//}
}


// ----------------------------------------------------------------------------
//		Read
// ----------------------------------------------------------------------------

void
COptions::Read()
{
	//HRESULT	hResult;
	//CPeekXmlDocument	spXmlDoc;
	//hResult = spXmlDoc.LoadFile( m_strConfigFileName );
	//if ( SUCCEEDED( hResult ) ) {
	//	LoadPrefs( spXmlDoc );
	//}
	//CheckDefaults();
}


// ----------------------------------------------------------------------------
//		Write
// ----------------------------------------------------------------------------

void
COptions::Write()
{
	//HRESULT	hResult;
	//CPeekXmlDocument	spXmlDoc;

	//StorePrefs( spXmlDoc );

	//hResult = spXmlDoc.PrettyFormat( _T( "  " ) );
	//ASSERT( SUCCEEDED( hResult ) );

	//hResult = spXmlDoc.StoreFile( m_strConfigFileName );
	//ASSERT( SUCCEEDED( hResult ) );
	//if ( FAILED( hResult ) ) return;
}
