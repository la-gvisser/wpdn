// =============================================================================
//	PeekProxy.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "heunk.h"
#include "peekcore.h"
#include "peeknames.h"
#include "peeknotify.h"
#include "Protospecs.h"
#include "Peek.h"
#include "iomniengineplugin.h"
#include "omnigraphs.h"
#include "peekfilters.h"
#include "Packet.h"
#include "GlobalId.h"
#include "PeekContextProxy.h"
#include "PeekStrings.h"
#include "NotifyService.h"

#define MAX_NAME_LENGTH		251


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Various wrappers of core Omni objects.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CUnknown
// =============================================================================

class CUnknown
{
protected:
	IHeUnknown*		m_pUnk;

public:
	;		CUnknown( IHeUnknown* inUnk = nullptr ) : m_pUnk( inUnk ) {}
	;		CUnknown( CHePtr<IHeUnknown> inUnk ) : m_pUnk( inUnk ) {}

	operator IHeUnknown*() { return m_pUnk; }
};


// =============================================================================
//		CPeekCaptures
// =============================================================================

class CPeekCaptures
{
	friend class CPeekSetup;

protected:
	CHePtr<IPeekCaptures>	m_spPeekCaptures;

protected:
	IPeekCaptures*	GetPtr() { return m_spPeekCaptures; }
	void			ReleasePtr() { m_spPeekCaptures.Release(); }
	void			SetPtr( IPeekCaptures* inCaptures ) {
		m_spPeekCaptures = inCaptures;
	}

public:
	;		CPeekCaptures() {}
	;		~CPeekCaptures() {}

	bool	IsValid() const { return (m_spPeekCaptures != nullptr); }
	bool	IsNotValid() const { return (m_spPeekCaptures == nullptr); }

	SInt32	GetCount() const {
		SInt32	nCount = 0;
		if ( IsValid() ) {
			m_spPeekCaptures->GetCount( &nCount );
		}
		return nCount;
	}
	CPeekCapture	GetItem( size_t inIndex ) {
		CPeekCapture	Capture;
		if ( IsValid() & (inIndex >= 0) ) {
			IPeekCapture*	pCapture = nullptr;
			int				nIndex = (inIndex & 0x0FFFFFFFF);
			m_spPeekCaptures->GetItem( nIndex, &pCapture );
			if ( pCapture != nullptr ) {
				Capture.SetPtr( pCapture );
			}
		}
		return Capture;
	}
};


// =============================================================================
//		CAppConfig
// =============================================================================

class CAppConfig
{
	friend class CPeekEngineProxy;
	friend class CPeekProxy;

protected:
	CHePtr<IAppConfig>	m_spAppConfig;

protected:
	IAppConfig*	GetPtr() { return m_spAppConfig; }
	void		ReleasePtr() { m_spAppConfig.Release(); }
	void		SetPtr( IAppConfig* inAppConfig ) {
		m_spAppConfig = inAppConfig;
	}
	void			SetPtr( Helium::IHeServiceProvider* inServiceProvider ) {
		if ( m_spAppConfig == nullptr ) {
			inServiceProvider->QueryService(
				IAppConfig::GetIID(), &m_spAppConfig.p );
		}
	}

public:
	;		CAppConfig( Helium::IHeUnknown* inUnkSite )
	{
		CHeQIPtr<Helium::IHeServiceProvider>	spServiceProvider( inUnkSite );
		Peek::ThrowIfNull( spServiceProvider );
		SetPtr( spServiceProvider );
	}
	;		~CAppConfig() {}

	CPeekString	GetConfigurationDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetConfigDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	CPeekString	GetLibraryDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetLibDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	CPeekString	GetResourceDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetResourceDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	CPeekString	GetDocumentDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetDocDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	CPeekString	GetLogDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetLogDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	CPeekString	GetLogFilePath() const {
		CHeBSTR	bstrPath;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetLogFilePath( &bstrPath.m_str ) );
		}
		return CPeekString( bstrPath );
	}
	CPeekString	GetDataDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetDataDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}
	// List of storage locations for application data.
	// Absolute paths separated by a semicolon.
	CPeekString	GetDataDirectoryList() const {
		CHeBSTR	bstrDirectoryList;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetDataDirList( &bstrDirectoryList.m_str ) );
		}
		return CPeekString( bstrDirectoryList );
	}
	CPeekString	GetDatabasePath() const {
		CHeBSTR	bstrPath;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetDatabasePath( &bstrPath.m_str ) );
		}
		return CPeekString( bstrPath );
	}
	CPeekString	GetPacketHeaderInfoStoragePath() const {
		CHeBSTR	bstrPath;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetPacketHeaderInfoStoragePath( &bstrPath.m_str ) );
		}
		return CPeekString( bstrPath );
	}
	CPeekString	GetSecurityFilePath() const {
		CHeBSTR	bstrPath;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetSecurityFilePath( &bstrPath.m_str ) );
		}
		return CPeekString( bstrPath );
	}
	CPeekString	GetIndexDatabasePath() const {
		CHeBSTR	bstrPath;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetIndexDatabasePath( &bstrPath.m_str ) );
		}
		return CPeekString( bstrPath );
	}
	CPeekString	GetDatabaseDirectory() const {
		CHeBSTR	bstrDirectory;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAppConfig->GetDatabaseDir( &bstrDirectory.m_str ) );
		}
		return CPeekString( bstrDirectory );
	}

	bool	IsValid() const { return (m_spAppConfig != nullptr); }
	bool	IsNotValid() const { return (m_spAppConfig == nullptr); }
};


// =============================================================================
//		CPeekSetup
// =============================================================================

class CPeekSetup
{
	friend class CPeekProxy;

protected:
	CHePtr<IPeekSetup>	m_spPeekSetup;

protected:
	IPeekSetup*		GetPtr() { return m_spPeekSetup; }
	void			ReleasePtr() { m_spPeekSetup.Release(); }
	void			SetPtr( IPeekSetup* inPeekSetup ) {
		m_spPeekSetup = inPeekSetup;
	}
	void			SetPtr( IHeUnknown* inUnknown ) {
		SetPtr( reinterpret_cast<IPeekSetup*>( inUnknown ) );
	}

public:
	;		CPeekSetup() {}
	;		~CPeekSetup() {}

	bool	IsValid() const { return (m_spPeekSetup != nullptr); }
	bool	IsNotValid() const { return (m_spPeekSetup == nullptr); }

	void			DeleteCapture( CPeekCapture inCapture ) {
		if ( IsValid() && inCapture.IsValid() ) {
			m_spPeekSetup->DeleteCapture( inCapture.GetPtr() );
		}
	}

	CPeekCaptures	GetCaptures() {
		CPeekCaptures	Captures;
		if ( IsValid() ) {
			IPeekCaptures*	pCaptures( nullptr );
			HeResult	hr = m_spPeekSetup->GetCaptures( &pCaptures );
			if ( HE_SUCCEEDED( hr ) ) {
				Captures.SetPtr( pCaptures );
			}
		}
		return Captures;
	}

	CPeekCapture	NewCapture() {
		CPeekCapture	Capture;
		if ( IsValid() ) {
			Helium::HEVARIANT	vtCapture;
			IPeekCapture*		pCapture( nullptr );
			HeResult	hr = m_spPeekSetup->NewCapture( vtCapture, &pCapture );
			if ( HE_SUCCEEDED( hr ) ) {
				Capture.SetPtr( pCapture );
			}
		}
		return Capture;
	}
};


#ifdef _WIN32
// =============================================================================
//		CConsoleSend
// =============================================================================

class CConsoleSend
{
	friend class CRemotePlugin;
	friend class CPeekPlugin;

protected:
	IConsoleSend*	m_pConsoleSend;

protected:
	IConsoleSend*	GetPtr() { return m_pConsoleSend; }
	void			ReleasePtr() { m_pConsoleSend = nullptr; }
	void			SetPtr( IConsoleSend* inConsoleSend ) {
		m_pConsoleSend = inConsoleSend;
	}

public:
	;		CConsoleSend() : m_pConsoleSend( nullptr ) {}
	;		~CConsoleSend() {}

	bool	operator==( const IConsoleSend* inConsoleSend ) const{
		return (inConsoleSend == m_pConsoleSend);
	}

	bool	IsValid() const { return (m_pConsoleSend != nullptr); }
	bool	IsNotValid() const { return (m_pConsoleSend == nullptr); }

	UInt32	Send( CGlobalId inPluginId, IPluginRecv* inReceive, UInt32 inLength,
		UInt8* inMessage, UInt32 inTimeout )
	{
		UInt32	nTransactionId = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_pConsoleSend->Send( inPluginId, inReceive,
				inLength, inMessage, inTimeout, &nTransactionId ) );
		}
		return nTransactionId;
	}

	UInt32	SendPluginMessage( CGlobalId inPluginId, CGlobalId inCaptureId,
		IPluginRecv* inReceive, UInt32 inLength, UInt8* inMessage,
		UInt32 inTimeout )
	{
		UInt32	nTransactionId = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_pConsoleSend->SendPluginMessage( inPluginId,
				inCaptureId, inReceive, inLength, inMessage, inTimeout, &nTransactionId ) );
		}
		return nTransactionId;
	}
};


// =============================================================================
//		CConsoleUI
// =============================================================================

class CConsoleUI
{
	friend class CRemotePlugin;
	friend class CPeekConsoleContext;

protected:
	CHePtr<IConsoleUI>	m_spConsoleUI;

protected:
	IConsoleUI*	GetPtr() { return m_spConsoleUI; }
	void		ReleasePtr() { m_spConsoleUI.Release(); }
	void		SetPtr( IConsoleUI* inConsoleUI ) { m_spConsoleUI = inConsoleUI; }
	void		SetPtr( IHeUnknown* inUnknown ) {
		SetPtr( reinterpret_cast<IConsoleUI*>( inUnknown ) );
	}

public:
	;		CConsoleUI() {}
	;		~CConsoleUI() {}

	HWND		AddTabWindow( const CPeekString& inInsertPoint,  bool bInsertAfter,
					const CPeekString& inCommand, const CPeekString& inWindowClass,
					const CPeekString& inWindowName ) {
		HWND	hWnd = nullptr;
		if ( IsValid() ) {
			CHeBSTR	bstrInsertPoint( inInsertPoint );
			CHeBSTR	bstrCommand( inCommand );
			CHeBSTR	bstrClass( inWindowClass );
			CHeBSTR	bstrName( inWindowName );
			Peek::ThrowIfFailed( m_spConsoleUI->AddTabWindow( bstrInsertPoint,
				bInsertAfter, bstrCommand, bstrClass, bstrName, &hWnd ) );
		}
		return hWnd;
	}
	HWND		AddTabWindow( const CPeekString& inCommand, const CPeekString& inWindowClass,
					const CPeekString& inWindowName ) {
		HWND	hWnd = nullptr;
		if ( IsValid() ) {
			CHeBSTR	bstrCommand( inCommand );
			CHeBSTR	bstrClass( inWindowClass );
			CHeBSTR	bstrName( inWindowName );
			Peek::ThrowIfFailed( m_spConsoleUI->AddTabWindow( nullptr,
				TRUE, bstrCommand, bstrClass, bstrName, &hWnd ) );
		}
		return hWnd;
	}
	IUnknown*	AddTabControl( const CPeekString& inInsertPoint, bool bInsertAfter,
					const CPeekString& inCommand, const CPeekString& inControlClass,
					const CPeekString& inWindowName ) {
		IUnknown*	pControl( nullptr );
		if ( IsValid() ) {
			CHeBSTR	bstrInsertPoint( inInsertPoint );
			CHeBSTR	bstrCommand( inCommand );
			CHeBSTR	bstrClass( inControlClass );
			CHeBSTR	bstrName( inWindowName );
			Peek::ThrowIfFailed( m_spConsoleUI->AddTabControl(
				bstrInsertPoint, bInsertAfter, bstrCommand, bstrClass,
				bstrName, &pControl ) );
		}
		return pControl;
	}

	void		DoRefresh( const CPeekString& inTab ) {
		if ( IsValid() ) {
			CHeBSTR	bstrTab( inTab );
			Peek::ThrowIfFailed( m_spConsoleUI->DoRefresh( bstrTab ) );
		}
	}

	bool	IsValid() const { return (m_spConsoleUI != nullptr); }
	bool	IsNotValid() const { return (m_spConsoleUI == nullptr); }
};


// =============================================================================
//		CEngineInfo
// =============================================================================

class CEngineInfo
{
	friend class CPeekPlugin;

protected:
	IEngineInfo*	m_pEngineInfo;

protected:
	IEngineInfo*	GetPtr() { return m_pEngineInfo; }
	void			ReleasePtr() { m_pEngineInfo = nullptr; }
	void			SetPtr( IEngineInfo* inEngineInfo ) {
		m_pEngineInfo = inEngineInfo;
	}

public:
	;		CEngineInfo() : m_pEngineInfo( nullptr ) {}
	;		~CEngineInfo() {}

	bool	operator==( const IEngineInfo* inConsoleSend ) const{
		return (inConsoleSend == m_pEngineInfo);
	}

	CPeekString	GetAddress() const {
		CHeBSTR	bstrAddress;
		Peek::ThrowIfFailed( m_pEngineInfo->GetAddress( &bstrAddress.m_str ) );
		return CPeekString( bstrAddress );
	}
	CPeekString	GetAuthentication() {
		CHeBSTR	bstrAuthentication;
		Peek::ThrowIfFailed( m_pEngineInfo->GetAuthentication( &bstrAuthentication.m_str ) );
		return CPeekString( bstrAuthentication );
	}
	CPeekString	GetDataFolder() {
		CHeBSTR	bstrDataFolder;
		Peek::ThrowIfFailed( m_pEngineInfo->GetDataFolder( &bstrDataFolder.m_str ) );
		return CPeekString( bstrDataFolder );
	}
	CPeekString	GetDomain() {
		CHeBSTR	bstrDomain;
		Peek::ThrowIfFailed( m_pEngineInfo->GetDomain( &bstrDomain.m_str ) );
		return CPeekString( bstrDomain );
	}
	CPeekString	GetEngineType() {
		CHeBSTR	bstrEngineType;
		Peek::ThrowIfFailed( m_pEngineInfo->GetEngineType( &bstrEngineType.m_str ) );
		return CPeekString( bstrEngineType );
	}
	CPeekString	GetName() {
		CHeBSTR	bstrName;
		Peek::ThrowIfFailed( m_pEngineInfo->GetName( &bstrName.m_str ) );
		return CPeekString( bstrName );
	}
	CPeekString	GetOperatingSystem() {
		CHeBSTR	bstrOperatingSystem;
		Peek::ThrowIfFailed( m_pEngineInfo->GetOperatingSystem( &bstrOperatingSystem.m_str ) );
		return CPeekString( bstrOperatingSystem );
	}
	UInt16		GetPort() {
		UInt16	nPort = 0;
		Peek::ThrowIfFailed( m_pEngineInfo->GetPort( &nPort ) );
		return nPort;
	}
	CPeekString	GetUserName() {
		CHeBSTR	bstrUserName;
		Peek::ThrowIfFailed( m_pEngineInfo->GetUserName( &bstrUserName.m_str ) );
		return CPeekString( bstrUserName );
	}

	bool	IsValid() const { return (m_pEngineInfo != nullptr); }
	bool	IsNotValid() const { return (m_pEngineInfo == nullptr); }
};
#endif /* _WIN32 */


// =============================================================================
//		CMediaUtils
// =============================================================================

class CMediaUtils
{
	friend class CPeekProxy;

protected:
	CHePtr<IMediaUtils>	m_spMediaUtils;

	IMediaUtils*	GetPtr() { return m_spMediaUtils; }
	void			ReleasePtr() { m_spMediaUtils.Release(); }
	void			SetPtr( IMediaUtils* inMediaUtils ) {
		m_spMediaUtils = inMediaUtils;
	}

public:
	;		CMediaUtils( IMediaUtils* inMediaUtils = nullptr )
		: m_spMediaUtils( inMediaUtils ) {}
	;		~CMediaUtils() {}

	CPeekString	GetMediaString( TMediaType inType, TMediaSubType inSubType ) {
		CHeBSTR	bstrMedia;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaString(
			inType, inSubType, &bstrMedia.m_str ) );
		return CPeekString( bstrMedia );
	}
	CPeekString	GetMediaTypeString( TMediaType inType ) {
		CHeBSTR	bstrMediaType;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaTypeString(
			inType, &bstrMediaType.m_str ) );
		return CPeekString( bstrMediaType );
	}
	CPeekString	GetMediaSubTypeString( TMediaSubType inSubType ) {
		CHeBSTR	bstrSubType;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaSubTypeString(
			inSubType, &bstrSubType.m_str ) );
		return CPeekString( bstrSubType );
	}
	CPeekString	GetMediaSpecClassString( TMediaSpecClass inSpecType ) {
		CHeBSTR	bstrClass;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaSpecClassString(
			inSpecType, &bstrClass.m_str ) );
		return CPeekString( bstrClass );
	}
	CPeekString	GetMediaSpecTypeString( TMediaSpecType inSubType ) {
		CHeBSTR	bstrType;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaSpecTypeString(
			inSubType, &bstrType.m_str ) );
		return CPeekString( bstrType );
	}
	CPeekString	GetMediaSpecTypePrefix( TMediaSpecType inSubType ) {
		CHeBSTR	bstrPrefix;
		Peek::ThrowIfFailed( m_spMediaUtils->GetMediaSpecTypePrefix(
			inSubType, &bstrPrefix.m_str ) );
		return CPeekString( bstrPrefix );
	}
	CPeekString	GetMediaSpecTypeDefault( TMediaSpecType inSubType ) {
		CHeBSTR	bstrType;
		Peek::ThrowIfFailed(
			m_spMediaUtils->GetMediaSpecTypeDefault( inSubType, &bstrType.m_str ) );
		return CPeekString( bstrType );
	}

	bool		IsNotValid() const { return (m_spMediaUtils == nullptr); }
	bool		IsValid() const { return (m_spMediaUtils != nullptr); }

	CPeekString	SpecToString( const TMediaSpec& inSpec ) {
		CHeBSTR	bstrText;
		if ( m_spMediaUtils != nullptr ) {
			Peek::ThrowIfFailed( m_spMediaUtils->SpecToString(
				&inSpec, &bstrText.m_str ) );
			return CPeekString( bstrText );
		}
		return CPeekString( L"" );
	}

	TMediaSpec	StringToSpec( const CPeekString& inSpec,
					TMediaSpecType inSpecType ) {
		TMediaSpec	MediaSpec;
		CHeBSTR		bstrSpec = inSpec;
		Peek::ThrowIfFailed( m_spMediaUtils->StringToSpec(
			bstrSpec, inSpecType, &MediaSpec ) );
		return MediaSpec;
	}
};


// =============================================================================
//		CLogService
// =============================================================================

class CLogService
{
	friend class CPeekProxy;

protected:
	IHeUnknown*		m_pUnkSite;

protected:
	void	SetSite( IHeUnknown* inUnkSite ) {
		m_pUnkSite = inUnkSite;
	}

public:
	;		CLogService() : m_pUnkSite( nullptr ) {}
	;		~CLogService() {}

	void	DoLogMessage( CGlobalId& inContextId, CGlobalId& inSourceId,
				UInt32 inSourceKey, UInt64 inTimeStamp, int inSeverity,
				const CPeekString& inShortMessage,
				const CPeekString& inLongMessage );

	bool	IsNotValid() const { return (m_pUnkSite == nullptr); }
	bool	IsValid() const { return (m_pUnkSite != nullptr ); }
};


// =============================================================================
//		CNameTable
// =============================================================================

class CNameTable
{
	friend class CPeekProxy;
	friend class CPeekConsoleProxy;
	friend class CPeekEngineProxy;

protected:
	CHePtr<INameTable>		m_spNameTable;

	INameTable*	GetPtr() { return m_spNameTable; }
	void		ReleasePtr() { m_spNameTable.Release(); }
	void		SetPtr( INameTable* inNameTable ) { m_spNameTable = inNameTable; }
	void		SetPtr( Helium::IHeServiceProvider* inServiceProvider ) {
		if ( !IsValid() ) {
			inServiceProvider->QueryService( INameTable::GetIID(), &m_spNameTable.p );
		}
	}

public:
	;		CNameTable( INameTable* inNameTable = nullptr )
		: m_spNameTable( inNameTable ) {}
	;		~CNameTable() {}

	bool	IsNotValid() const { return (m_spNameTable == nullptr); }
	bool	IsValid() const { return (m_spNameTable != nullptr); }

	// Get an entry by index.
	bool	NameTableGetItem( TMediaSpecClass msc, SInt32 nIndex,
					PeekNameEntry* pEntry ) {
		bool bResult = false;
		if ( IsValid() ) {
			bResult = HE_SUCCEEDED( m_spNameTable->GetItem( msc, nIndex, pEntry ) );
		}
		return bResult;
	}
	// Get the count of name entries in the table.
	SInt32		NameTableGetCount( TMediaSpecClass msc ) {
		SInt32		nCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->GetCount( msc, &nCount ) );
		}
		return nCount;
	}
	// Arbitrary counter that increments when you add/remove/edit.
	SInt32		NameTableGetChangeCount() {
		SInt32		nChangeCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->GetChangeCount( &nChangeCount ) );
		}
		return nChangeCount;
	}
	// Add a name entry. Options are PeekNameAddOptions.
	void		NameTableAddEntry( const Peek::NameEntry& Entry, UInt32 nOptions ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->Add(
				reinterpret_cast<const PeekNameEntry*>( &Entry ), nOptions ) );
		}
	}
	// Remove entry with this media spec.
	void		NameTableRemoveEntry( const TMediaSpec& Spec) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->Remove( &Spec ) );
		}
	}
	// Remove all entries.
	void		NameTableRemoveAll() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->RemoveAll() );
		}
	}
	// Purge old entries.
	void		NameTablePurge() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->Purge() );
		}
	}
	// Merge with another name table.
	void		NameTableMerge( INameTable* pNameTable, UInt32 nOptions ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spNameTable->Merge( pNameTable, nOptions ) );
		}
	}
};


// =============================================================================
//		CLookupNameEntry
// =============================================================================

class CLookupNameEntry
{
	friend class CPeekProxy;
	friend class CPeekConsoleProxy;
	friend class CPeekEngineProxy;

protected:
	CHePtr<ILookupNameEntry>		m_spLookupNameEntry;

	ILookupNameEntry*	GetPtr() { return m_spLookupNameEntry; }
	void				ReleasePtr() { m_spLookupNameEntry.Release(); }
	void				SetPtr( ILookupNameEntry* inLookup ) {
		m_spLookupNameEntry = inLookup;
	}
	void		SetPtr( Helium::IHeServiceProvider* inServiceProvider ) {
		if ( !IsValid() ) {
			const HeID	sidNameTable = NameTable_SID;
			inServiceProvider->QueryService( sidNameTable, &m_spLookupNameEntry.p );
		}
	}

public:
	;		CLookupNameEntry( ILookupNameEntry* inLookupNameEntry )
						: m_spLookupNameEntry( inLookupNameEntry ) {}
	;		CLookupNameEntry() : m_spLookupNameEntry( nullptr ) {}
	;		~CLookupNameEntry() {}

	bool	IsNotValid() const { return (m_spLookupNameEntry == nullptr); }
	bool	IsValid() const { return (m_spLookupNameEntry != nullptr); }

	// Lookup an entry in a name table.
	bool	LookupNameTableEntry( TMediaSpec& Spec, Peek::NameEntry& Entry ) {
		bool	bResult = false;
		if ( m_spLookupNameEntry ) {
			BOOL	b = FALSE;
			Peek::ThrowIfFailed( m_spLookupNameEntry->LookupNameEntry(
				&Spec, reinterpret_cast<PeekNameEntry*>( &Entry ), &b ) );
			bResult = (b != FALSE);
		}
		return bResult;
	}
};


// =============================================================================
//		CProtospecs
// =============================================================================

class CProtospecs
{
public:
	;		CProtospecs() {}
	;		~CProtospecs() {}

	CPeekString	GetHierarchicalName( PROTOSPEC_INSTID inInstanceID ) {
		wchar_t		szName[MAX_NAME_LENGTH];
		int			cchName = MAX_NAME_LENGTH;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSHierarchicalName(
			inInstanceID, szName, cchName ) );
		return CPeekString( szName );
	}
	CPeekString	GetShortName( PROTOSPEC_INSTID inInstanceID ) {
		wchar_t		szName[MAX_NAME_LENGTH];
		int			cchName = MAX_NAME_LENGTH;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSShortName(
			inInstanceID, szName, cchName ) );
		return CPeekString( szName );
	}
	CPeekString	GetLongName( PROTOSPEC_INSTID inInstanceID ) {
		wchar_t		szName[MAX_NAME_LENGTH];
		int			cchName = MAX_NAME_LENGTH;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSLongName(
			inInstanceID, szName, cchName ) );
		return CPeekString( szName );
	}
	CPeekString	GetDescription( PROTOSPEC_INSTID inInstanceID ) {
		wchar_t		szDescription[500];
		int			cchDescription = 500;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSDescription(
			inInstanceID, szDescription, cchDescription ) );
		return CPeekString( szDescription );
	}
	COLORREF	GetColor( PROTOSPEC_INSTID inInstanceID ) {
		COLORREF	cr = 0;
		Peek::ThrowIfFailed( ProtoSpecs::GetColor( inInstanceID, cr ) );
		return cr;
	}
	COLORREF	GetNamedColor( const CPeekString& strName ) {
		COLORREF	cr = 0;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSNamedColor( strName, cr ) );
		return cr;
	}
	CPeekString		GetColorName( COLORREF cr ) {
		wchar_t		szName[MAX_NAME_LENGTH];
		int			cchName = MAX_NAME_LENGTH;
		Peek::ThrowIfFailed( ProtoSpecs::GetPSLongName( cr, szName, cchName ) );
		return CPeekString( szName );
	}
	PROTOSPEC_INSTID	GetPacketProtoSpec( TMediaType inMediaType,
							TMediaSubType inMediaSubType, UInt32 inPacketFlags,
							const UInt8* inPacket, UInt16 inPacketSize ) {
		PROTOSPEC_INSTID	Id;
		Peek::ThrowIfFailed( ProtoSpecs::GetPacketProtoSpec( inMediaType,
			inMediaSubType, inPacketFlags, inPacket, inPacketSize, Id ) );
		return Id;
	}

	SInt32		IsDescendentOf( PROTOSPEC_INSTID inInstanceID,
					PROTOSPEC_INSTID* outMatchParentInstID,
					const PROTOSPEC_INSTID* inParentIDs,
					SInt32 inParentCount ) {
		SInt32	nMatchParentIndex = -1;
		Peek::ThrowIfFailed( ProtoSpecs::IsDescendentOf( inInstanceID,
			outMatchParentInstID, inParentIDs, inParentCount, &nMatchParentIndex ) );
		return nMatchParentIndex;
	}
	PROTOSPEC_INSTID	GetSibling( PROTOSPEC_INSTID inInstanceID ) {
		PROTOSPEC_INSTID	Sibling;
		Peek::ThrowIfFailed( ProtoSpecs::GetSibling( inInstanceID, Sibling ) );
		return Sibling;
	}
	PROTOSPEC_INSTID	GetChild( PROTOSPEC_INSTID inInstanceID ) {
		PROTOSPEC_INSTID	Child;
		Peek::ThrowIfFailed( ProtoSpecs::GetChild( inInstanceID, Child ) );
		return Child;
	}
	bool		BuildParentArray( PROTOSPEC_INSTID nInstanceID,
					PROTOSPEC_INSTID* payInstIDBuff, int &nBuffCount ) {
		HeResult	hr = HE_S_OK;
		Peek::ThrowIfFailed( (hr = ProtoSpecs::BuildParentArray(
			nInstanceID, payInstIDBuff, nBuffCount )) );
		return HE_SUCCEEDED( hr );
	}
	PROTOSPEC_INSTID	GetDataLayer( TMediaType inMediaType,
							TMediaSubType inMediaSubType, const UInt8* inPacket,
							UInt16 inPacketSize, PROTOSPEC_INSTID inID,
							const UInt8** ppData ) {
		PROTOSPEC_INSTID	Source;
		Peek::ThrowIfFailed( ProtoSpecs::GetDataLayer( inMediaType, inMediaSubType,
			inPacket, inPacketSize, inID, Source, ppData ) );
		return Source;
	}
	PROTOSPEC_INSTID	GetHeaderLayer( TMediaType inMediaType,
							TMediaSubType inMediaSubType, const UInt8* inPacket,
							UInt16 inPacketSize, PROTOSPEC_INSTID inID,
							const UInt8** ppData ) {
		PROTOSPEC_INSTID	Source;
		Peek::ThrowIfFailed( ProtoSpecs::GetHeaderLayer( inMediaType,
			inMediaSubType, inPacket, inPacketSize, inID, Source, ppData ) );
		return Source;
	}
	void	IsRootProtoSpec( PROTOSPEC_INSTID inID ) {
		Peek::ThrowIfFailed( ProtoSpecs::IsRootPSpec( inID ) );
	}
	//HeResult	BuildParentInfo( TMediaType inMediaType, TMediaSubType inMediaSubType, const UInt8* inPacket,
	//				UInt16 inPacketSize, PROTOSPEC_INSTID inID, SProtospecInfo* payInfo, int& nInfoCount ) {
	//	UInt32	hr = HE_S_OK;
	//	Peek::ThrowIfFailed( (hr = ProtoSpecs::BuildParentInfo( inMediaType, inMediaSubType, inPacket, inPacketSize, inID, payInfo, nInfoCount )) );
	//	return hr;
	//}
};


#if (0)
// =============================================================================
//		CQueryLog
// =============================================================================

class CQueryLog
{
	friend class CPeekProxy;
	friend class CPeekEngineContext;
	friend class COmniEngineContext;

private:
	CHePtr<IQueryLog>		m_spQueryLog;

protected:
	;		CQueryLog( IQueryLog* inQueryLog ) : m_spQueryLog( inQueryLog ) {}
	;		CQueryLog() : m_spQueryLog( nullptr ) {}
	;		~CQueryLog() {}

	// Set the internal interface pointer
	void		SetPointer( IQueryLog* inQueryLog ) {
		m_spQueryLog = inQueryLog;
	}

	/// Query the message log with an SQL statement, receive XML.
	/// First page: positive page size and zero message id.
	/// Previous page: negative page size from the message id.
	/// Next page: positive page size from the message id.
	/// Last page: negative page size and zero message id.
	bool		QueryLog( /*in*/ SInt32 nPageSize, /*in*/ SInt32 nMessageID, /*in*/ Helium::HEBSTR bstrSQL,
		/*in*/ Helium::IHeUnknown* pXmlDoc, /*out*/ Helium::IHeUnknown* pXmlNode) {
		HeResult	hr = HE_S_OK;
		if ( m_spQueryLog ) {
			Peek::ThrowIfFailed( (hr = m_spQueryLog->QueryLog( nPageSize, nMessageID, bstrSQL, pXmlDoc, pXmlNode )) );
		}
		return (HE_SUCCEEDED( hr ));
	}

	/// Query the message log for a record count using an SQL "where" clause.
	/// The "where" class may be nullptr to get the total count.
	SInt32		QueryCount( /*in*/ Helium::HEBSTR bstrWhere ) {
		HeResult	hr = HE_S_OK;
		SInt32		nCount = 0;
		if ( m_spQueryLog ) {
			Peek::ThrowIfFailed( (hr = m_spQueryLog->QueryCount( bstrWhere, &nCount )) );
		}
		return nCount;
	}
};
#endif


// =============================================================================
//		CPeekProxy
// =============================================================================

class CPeekProxy
{
	friend class CRemotePlugin;
	friend class CRemoteStatsPlugin;
	friend class CPeekContext;

protected:
	static bool			s_bEngineHost;
	static CMediaUtils	s_MediaUtils;
	static CProtospecs	s_Protospecs;

protected:
	IHeUnknown*			m_pUnkSite;
	CLogService			m_LogService;

protected:
	void			SetConsoleHost() { s_bEngineHost = false; }
	void			SetInterfacePointers( IHeUnknown* inUnkSite );

public:
	static bool		IsEngine() { return s_bEngineHost; }
	static bool		IsConsole() { return !s_bEngineHost; }
	static bool		ValidateConsole() {
		if ( IsConsole() ) return true; _ASSERTE( s_bEngineHost != s_bEngineHost ); return false;
	}
	static bool		ValidateEngine() {
		if ( IsEngine() ) return true; _ASSERTE( !s_bEngineHost != !s_bEngineHost ); return false;
	}
#ifdef _DEBUG
	static void		ValidateHost( bool inHostType ) {
		_ASSERTE( s_bEngineHost == inHostType );
	}
#endif

public:
	;				CPeekProxy() : m_pUnkSite( nullptr ) {}
	virtual			~CPeekProxy() {}

	static CMediaUtils		GetMediaUtils() { return s_MediaUtils; }
	static CProtospecs		GetProtospecs() { return s_Protospecs; }

	CAppConfig				GetAppConfig() { return CAppConfig( m_pUnkSite ); }
	CLogService&			GetLogService() { return m_LogService; }
};


// =============================================================================
//		CPeekConsoleProxy
// =============================================================================

class CPeekConsoleProxy
	: public CPeekProxy
{
	friend class CPeekConsoleContext;
	friend class CRemotePlugin;

protected:
#ifdef IMPLEMENT_NOTIFICATIONS
//	CNotifyService				m_NotifyService;
#endif // IMPLEMENT_NOTIFICATIONS
	CHePtr<IAdapterManager>		m_spAdapterManager;

protected:
	void				SetInterfacePointers( IHeUnknown* inUnkSite );

public:
	;					CPeekConsoleProxy() {}
	virtual				~CPeekConsoleProxy() {}

#ifdef IMPLEMENT_NOTIFICATIONS
//	CNotifyService&		GetNotifyService() { return m_NotifyService; }
#endif // IMPLEMENT_NOTIFICATIONS
};


// =============================================================================
//		CPeekEngineProxy
// =============================================================================

class CPeekEngineProxy
	: public CPeekProxy
{
	friend class CPeekEngineContext;
	friend class CRemotePlugin;

protected:
	CHePtr<Helium::IHeUnknown>			m_spUnkSite;
	CHePtr<IFilterCollection>			m_spFilterCollection;
	CAdapterManager						m_AdapterManager;
	CNameTable							m_NameTable;
	CHePtr<IHardwareOptionsCollection>	m_spHardwareOptionsCollection;
	//CHePtr<IExpertPrefs>				m_spExpertPrefs;

#ifdef IMPLEMENT_NOTIFICATIONS
	CNotifyService						m_NotifyService;
#endif // IMPLEMENT_NOTIFICATIONS

	//CHePtr<IEngine>					m_spEngine;
	CHePtr<Helium::IHePropertyBag>		m_spSimplePropBag;
	//CHePtr<IActivationInfo>			m_spActivationInfo;
	//CHePtr<IFileLog>					m_spFileLog;
	//CHePtr<IEngine>					m_spEngine;
	CHePtr<IAlarmCollection>			m_spAlarmCollection;
	CHePtr<IEnginePluginCollection>		m_spEnginePluginCollection;
	CHePtr<IAdapterManager>				m_spAdapterManager;
	CHePtr<ICaptureTemplateCollection>	m_spCaptureTemplateCollection;
	//CHePtr<IEngineManagement>			m_spEngineManagement;
	//CHePtr<IGraphTemplateMgr>			m_spGraphTemplateMgr;

	CLookupNameEntry					m_LookupNameEntry;
	//	CQueryLog						m_QueryLog;

protected:
	void				ReleaseInterfacePointers();
	void				SetInterfacePointers( IHeUnknown* inUnkSite, CRemotePlugin* pRemote );

public:
	;					CPeekEngineProxy() {}
	virtual				~CPeekEngineProxy() {}

	CAdapterManager		GetAdapterManager() const { return m_AdapterManager; }
	CLookupNameEntry	GetLookupNameEntry() { return m_LookupNameEntry; }
	CNameTable			GetNameTable() { return m_NameTable; }
	Helium::IHeUnknown* GetSite() { return m_spUnkSite; }
#ifdef IMPLEMENT_NOTIFICATIONS
	CNotifyService&		GetNotifyService() { return m_NotifyService; }
	void				StopSendNotifies() { m_NotifyService.StopSendNotifies(); }
#endif // IMPLEMENT_NOTIFICATIONS
	//	CQueryLog			GetQueryLog() { return m_QueryLog; }
};
