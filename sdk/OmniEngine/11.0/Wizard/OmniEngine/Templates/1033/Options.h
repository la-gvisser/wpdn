// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Options
//
//	An Options holds the user preferences (settings) for the plugin and/or
//	individual capture contexts.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
[!if !NOTIFY_UIHANDLER]
	friend class COptionsDialog;

[!endif]
protected:
[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
[!if NOTIFY_NOTIFICATIONS]
  #ifdef IMPLEMENT_NOTIFICATIONS
	bool				m_bReceiveNotifies;
  #endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if NOTIFY_UIHANDLER]
	static const size_t	s_nCount = 3;
	CArrayString		m_ayOptions;
	CPeekString			m_strNotes;
[!else]
	typedef struct {
		CPeekString				fName;
		CIpAddress				fIpAddress;
		CPort					fPort;
		CGlobalId				fId;
		CPeekArray<CPeekString>	fUserNames;
	} tComputerExample;

	static CGlobalId			m_idSoftware;

	BOOL				m_bEnabledExample;
	CPeekString			m_strLabelExample;
	CEthernetAddress	m_EthernetExample;
	CIpAddress			m_IpAddressExample;
	CPort				m_nPortExample;
	CPeekTime			m_TimeExample;

	CPeekArray<tComputerExample>	m_ayComputers;
[!endif]
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	;			COptions();
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions();

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool		operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset();
[!if NOTIFY_CODEEXAMPLES]
[!if NOTIFY_NOTIFICATIONS || !NOTIFY_UIHANDLER]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
[!if NOTIFY_NOTIFICATIONS]
  #ifdef IMPLEMENT_NOTIFICATIONS
	bool		IsReceiveNotifies() { return m_bReceiveNotifies; }
	void		SetReceiveNotifies( bool bReceive ) { m_bReceiveNotifies = bReceive; }
  #endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if !NOTIFY_UIHANDLER]
	const CPeekString&	GetLabelExample() const { return m_strLabelExample; }
	CEthernetAddress	GetEthernetExample() const { return m_EthernetExample; }
	CIpAddress			GetIpAddressExample() const { return m_IpAddressExample; }
	CPort				GetPortExample() const { return m_nPortExample; }
	CPeekTime			GetTimeExample() const { return m_TimeExample; }

	bool		IsEnabledExample() const { return (m_bEnabledExample != FALSE); }

	void		ObjectExamples( CPeekDataElement&	inParentElement );

	void		RepeatingExample( CPeekDataElement& inParentElement );
	void		RepeatComputerExample( CPeekDataElement& inComputerElement,
										tComputerExample& ioComputer );

	void		SetLabelExample( const CPeekString& inLabel ) { m_strLabelExample = inLabel; }
	void		SetAddressExample( CIpAddress inAddress ) { m_IpAddressExample = inAddress; }
	void		SetAddressExample( UInt32 inAddress ) { m_IpAddressExample = inAddress; }
	void		SetPortExample( UInt16 inPort ) { m_nPortExample = inPort; }
[!endif]
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
[!endif]
};
