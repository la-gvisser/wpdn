// =============================================================================
//	PeekProxy.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

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
	;		CUnknown( IHeUnknown* inUnk = NULL ) : m_pUnk( inUnk ) {}
	;		CUnknown( CHePtr<IHeUnknown> inUnk ) : m_pUnk( inUnk ) {}

	operator IHeUnknown*() { return m_pUnk; }
};


// =============================================================================
//		CPeekSysBSTR
// =============================================================================

class CPeekSysBSTR
{
public:
	Helium::HEBSTR	m_str;

public:
	;		CPeekSysBSTR() : m_str( NULL ) {}
	;		~CPeekSysBSTR()	{ Reset(); }

	;		operator Helium::HEBSTR() const {
		return m_str;
	}

	void	Reset() {
		if ( m_str ) {
#if (0)
			::SysFreeString( m_str );
			m_str = NULL;
#else
			// Get number of heaps, and allocate storage.
			DWORD	dwHeaps = GetProcessHeaps( 0, NULL );
			if ( dwHeaps == 0 ) return;

			std::vector<HANDLE>	ayHeaps( dwHeaps );

			// Get the heaps.
			::GetProcessHeaps( dwHeaps, &ayHeaps[0] );

			// Enumerate and check each heap if the contain the address.
			void*	pMemory( m_str );
			for ( DWORD i = 0; i < dwHeaps; i++ ) {
				PROCESS_HEAP_ENTRY he;
				he.lpData = NULL;

				HeapLock( ayHeaps[i] );
				while ( HeapWalk( ayHeaps[i], &he ) ) {
					if ( (pMemory >= he.lpData) &&
						 (pMemory < (reinterpret_cast<char *>( he.lpData ) + he.cbData)) ) {
						// lpData is the address that HeapFree knows about.
						// Passing m_str (pMemory) will fail.
						if ( HeapFree( ayHeaps[i], 0, he.lpData ) ) {
							m_str = NULL;
						}
#ifdef _DEBUG
						else {
							DWORD	dwError(  GetLastError() );
							if ( dwError > 0 ) {
								pMemory = m_str;
							}
						}
#endif
						HeapUnlock( ayHeaps[i] );
						return;
					}
				}
				HeapUnlock( ayHeaps[i] );
			}
#endif
		}
	}
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

	bool	IsValid() const { return (m_spPeekCaptures != NULL); }
	bool	IsNotValid() const { return (m_spPeekCaptures == NULL); }

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
			IPeekCapture*	pCapture = NULL;
			int				nIndex = (inIndex & 0x0FFFFFFFF);
			m_spPeekCaptures->GetItem( nIndex, &pCapture );
			if ( pCapture != NULL ) {
				Capture.SetPtr( pCapture );
			}
		}
		return Capture;
	}
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

	bool	IsValid() const { return (m_spPeekSetup != NULL); }
	bool	IsNotValid() const { return (m_spPeekSetup == NULL); }

	void			DeleteCapture( CPeekCapture inCapture ) {
		if ( IsValid() && inCapture.IsValid() ) {
			m_spPeekSetup->DeleteCapture( inCapture.GetPtr() );
		}
	}

	CPeekCaptures	GetCaptures() {
		CPeekCaptures	Captures;
		if ( IsValid() ) {
			IPeekCaptures*	pCaptures( NULL );
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
			IPeekCapture*		pCapture( NULL );
			HeResult	hr = m_spPeekSetup->NewCapture( vtCapture, &pCapture );
			if ( HE_SUCCEEDED( hr ) ) {
				Capture.SetPtr( pCapture );
			}
		}
		return Capture;
	}
};


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
	void			ReleasePtr() { m_pConsoleSend = NULL; }
	void			SetPtr( IConsoleSend* inConsoleSend ) {
		m_pConsoleSend = inConsoleSend;
	}

public:
	;		CConsoleSend() : m_pConsoleSend( NULL ) {}
	;		~CConsoleSend() {}

	bool	operator==( const IConsoleSend* inConsoleSend ) const{
		return (inConsoleSend == m_pConsoleSend);
	}

	bool	IsValid() const { return (m_pConsoleSend != NULL); }
	bool	IsNotValid() const { return (m_pConsoleSend == NULL); }

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
		HWND	hWnd = NULL;
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
		HWND	hWnd = NULL;
		if ( IsValid() ) {
			CHeBSTR	bstrCommand( inCommand );
			CHeBSTR	bstrClass( inWindowClass );
			CHeBSTR	bstrName( inWindowName );
			Peek::ThrowIfFailed( m_spConsoleUI->AddTabWindow( NULL,
				TRUE, bstrCommand, bstrClass, bstrName, &hWnd ) );
		}
		return hWnd;
	}
	IUnknown*	AddTabControl( const CPeekString& inInsertPoint, bool bInsertAfter,
					const CPeekString& inCommand, const CPeekString& inControlClass,
					const CPeekString& inWindowName ) {
		IUnknown*	pControl( NULL );
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

	bool	IsValid() const { return (m_spConsoleUI != NULL); }
	bool	IsNotValid() const { return (m_spConsoleUI == NULL); }
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
	void			ReleasePtr() { m_pEngineInfo = NULL; }
	void			SetPtr( IEngineInfo* inEngineInfo ) {
		m_pEngineInfo = inEngineInfo;
	}

public:
	;		CEngineInfo() : m_pEngineInfo( NULL ) {}
	;		~CEngineInfo() {}

	bool	operator==( const IEngineInfo* inConsoleSend ) const{
		return (inConsoleSend == m_pEngineInfo);
	}

	bool	IsValid() const { return (m_pEngineInfo != NULL); }
	bool	IsNotValid() const { return (m_pEngineInfo == NULL); }

	CPeekString	GetAddress() const {
		CPeekSysBSTR	bstrAddress;
		Peek::ThrowIfFailed( m_pEngineInfo->GetAddress( &bstrAddress.m_str ) );
		return CPeekString( bstrAddress );
	}
	UInt16		GetPort() {
		UInt16	nPort = 0;
		Peek::ThrowIfFailed( m_pEngineInfo->GetPort( &nPort ) );
		return nPort;
	}
	CPeekString	GetAuthentication() {
		CPeekSysBSTR	bstrAuthentication;
		Peek::ThrowIfFailed( m_pEngineInfo->GetAuthentication( &bstrAuthentication.m_str ) );
		return CPeekString( bstrAuthentication );
	}
	CPeekString	GetDomain() {
		CPeekSysBSTR	bstrDomain;
		Peek::ThrowIfFailed( m_pEngineInfo->GetDomain( &bstrDomain.m_str ) );
		return CPeekString( bstrDomain );
	}
	CPeekString	GetUserName() {
		CPeekSysBSTR	bstrUserName;
		Peek::ThrowIfFailed( m_pEngineInfo->GetUserName( &bstrUserName.m_str ) );
		return CPeekString( bstrUserName );
	}
	CPeekString	GetName() {
		CPeekSysBSTR	bstrName;
		Peek::ThrowIfFailed( m_pEngineInfo->GetName( &bstrName.m_str ) );
		return CPeekString( bstrName );
	}
};


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
	;		CMediaUtils( IMediaUtils* inMediaUtils = NULL )
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

	bool		IsNotValid() const { return (m_spMediaUtils == NULL); }
	bool		IsValid() const { return (m_spMediaUtils != NULL); }

	CPeekString	SpecToString( const TMediaSpec& inSpec ) {
		CHeBSTR	bstrText;
		if ( m_spMediaUtils != NULL ) {
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
	friend class CPeekEngineProxy;
	friend class CPeekConsoleProxy;

protected:
	CHePtr<ILogMessage>		m_spMessageService;

	ILogMessage*	GetPtr()	 { return m_spMessageService; }
	void			ReleasePtr() {	m_spMessageService.Release(); }

	void			SetPtr( Helium::IHeServiceProvider* inServiceProvider ) {
		if ( m_spMessageService == NULL ) {
			inServiceProvider->QueryService(
				ILogMessage::GetIID(), &m_spMessageService.p );
		}
	}

public:
	;		CLogService( ILogMessage* inMessage = NULL )
		: m_spMessageService( inMessage ) {}
	;		~CLogService() { ReleasePtr(); }

	void	DoLogMessage( CGlobalId& inContextId, CGlobalId& inSourceId,
				UInt32 inSourceKey, UInt64 inTimeStamp, int inSeverity,
				const CPeekString& inShortMessage,
				const CPeekString& inLongMessage ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMessageService->LogMessage(
				inContextId, inSourceId, inSourceKey, inTimeStamp,
				static_cast<PeekSeverity>( inSeverity ), inShortMessage,
				inLongMessage ) );
		}
	}

	bool	IsNotValid() const { return (m_spMessageService == NULL); }
	bool	IsValid() const { return (m_spMessageService != NULL ); }
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
	;		CNameTable( INameTable* inNameTable = NULL )
		: m_spNameTable( inNameTable ) {}
	;		~CNameTable() {}

	bool	IsNotValid() const { return (m_spNameTable == NULL); }
	bool	IsValid() const { return (m_spNameTable != NULL); }

	// Get an entry by index.
	bool		NameTableGetItem( TMediaSpecClass msc, SInt32 nIndex,
					PeekNameEntry* pEntry ) {
		bool bResult = false;
		if ( IsValid() ) {
			bResult = HE_SUCCEEDED( m_spNameTable->GetItem( msc, nIndex, pEntry ) );
		}
		return bResult;;
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
	;		CLookupNameEntry() : m_spLookupNameEntry( NULL ) {}
	;		~CLookupNameEntry() {}

	bool	IsNotValid() const { return (m_spLookupNameEntry == NULL); }
	bool	IsValid() const { return (m_spLookupNameEntry != NULL); }

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
		return SUCCEEDED( hr );
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


#if 0
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
	;		CQueryLog() : m_spQueryLog( NULL ) {}
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
		return (SUCCEEDED( hr ));
	}

	/// Query the message log for a record count using an SQL "where" clause.
	/// The "where" class may be NULL to get the total count.
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
	friend class CPeekContext;

protected:
	static bool			s_bEngineHost;
	static CMediaUtils	s_MediaUtils;
	static CProtospecs	s_Protospecs;

protected:
	void			SetConsoleHost() { s_bEngineHost = false; }
	void			SetInterfacePointers();

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
	;				CPeekProxy() {}
	virtual			~CPeekProxy() {}

	static CMediaUtils		GetMediaUtils() { return s_MediaUtils; }
	static CProtospecs		GetProtospecs() { return s_Protospecs; }
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
	CLogService					m_LogService;
#ifdef IMPLEMENT_NOTIFICATIONS
//	CNotifyService				m_NotifyService;
#endif // IMPLEMENT_NOTIFICATIONS
	CHePtr<IAdapterManager>		m_spAdapterManager;
	CHePtr<IAppConfig>			m_spAppConfig;

protected:
	void				SetInterfacePointers( IHeUnknown* inUnkSite );

public:
	;					CPeekConsoleProxy() {}
	virtual				~CPeekConsoleProxy() {}

	CLogService&		GetLogService() { return m_LogService; }
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
	CHePtr<IFilterCollection>			m_spFilterCollection;
	CLogService							m_LogService;
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
	CHePtr<IAppConfig>					m_spAppConfig;
	//CHePtr<IGraphTemplateMgr>			m_spGraphTemplateMgr;

	CLookupNameEntry					m_LookupNameEntry;
	//	CQueryLog						m_QueryLog;

protected:
	void				ReleaseInterfacePointers();
	void				SetInterfacePointers( IHeUnknown* inUnkSite, CRemotePlugin* pRemote );

public:
	;					CPeekEngineProxy() {}
	virtual				~CPeekEngineProxy() {}

	CLogService&		GetLogService() { return m_LogService; }
	CLookupNameEntry	GetLookupNameEntry() { return m_LookupNameEntry; }
	CNameTable			GetNameTable() { return m_NameTable; }
#ifdef IMPLEMENT_NOTIFICATIONS
	CNotifyService&		GetNotifyService() { return m_NotifyService; }
	void				StopSendNotifies() { m_NotifyService.StopSendNotifies(); }
#endif // IMPLEMENT_NOTIFICATIONS
	//	CQueryLog			GetQueryLog() { return m_QueryLog; }
};
