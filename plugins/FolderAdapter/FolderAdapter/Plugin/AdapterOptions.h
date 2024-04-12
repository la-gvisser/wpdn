// =============================================================================
//	AdapterOptions.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekArray.h"
#include "PeekStrings.h"
#include "PeekTime.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	AdapterOptions
//
//	An Options holds the adapter settings.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CAdapterOptions
// =============================================================================

class CAdapterOptions
{
protected:
	CGlobalId	m_Id;
	CPeekString	m_strName;
	CPeekString	m_strMonitor;
	CPeekString	m_strMask;
	SInt32		m_nSpeed;
	CPeekString	m_strSave;
	bool		m_bDontCheckFileOpen;

protected:
	void		Copy( const CAdapterOptions& inOther );
	bool		IsEqual( const CAdapterOptions& inOther ) const;

public:
	;			CAdapterOptions();
	;			CAdapterOptions( const CAdapterOptions& inOther ) { Copy( inOther ); }
	virtual		~CAdapterOptions();

	CAdapterOptions&	operator=( const CAdapterOptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool	operator==( const CAdapterOptions& inOther ) { return IsEqual( inOther ); }
	bool	operator!=( const CAdapterOptions& inOther ) { return !IsEqual( inOther ); }

	const CGlobalId&	GetId() const { return m_Id; }
	const CPeekString&	GetMonitor() const { return m_strMonitor; }
	const CPeekString&	GetMask() const { return m_strMask; }
	const CPeekString&	GetName() const { return m_strName; }
	const CPeekString&	GetSave() const { return m_strSave; }
	SInt32				GetSpeed() const { return m_nSpeed; }

	bool	IsDontCheckFileOpen() const { return m_bDontCheckFileOpen; }
	bool	IsEmpty() const;
	bool	IsSave() const { return !m_strSave.empty(); }

	bool	Model( CPeekDataModeler& ioPrefs );
	bool	Model( CPeekDataElement& ioElement );

	void	Reset();

	void	SetDontCheckFileOpen( const bool inState ) { m_bDontCheckFileOpen = inState; }
	void	SetId( const CGlobalId& inId ) { m_Id = inId; }
	void	SetMask( const CPeekString& inMask ) { m_strMask = inMask; }
	void	SetMonitor( const CPeekString& inFolder ) { m_strMonitor = inFolder; }
	void	SetName( const CPeekString& inName ) { m_strName = inName; }
	void	SetSave( const CPeekString& inFolder ) { m_strSave = inFolder; }
	void	SetSpeed( SInt32 inSpeed ) { m_nSpeed = inSpeed; }
	void	SetSpeed( const CPeekString& inSpeed ) { m_nSpeed = std::stoi( inSpeed ); }

	CPeekString	ToString() const;
};
