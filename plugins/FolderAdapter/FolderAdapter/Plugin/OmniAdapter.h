// =============================================================================
//	PeekSocket.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekAdapter.h"
// #include "PeekSocket.h"
#include "PeekContextProxy.h"
#include "GlobalId.h"
#include "FileEx.h"
#include "AdapterOptions.h"
#include <atomic>
#include <map>
#include <string>
#include <thread>
#include <vector>


// =============================================================================
//		COmniAdapter
// =============================================================================

class COmniAdapter
	:	public CPeekAdapter
{
public:
	static CAdapterInfo	CreateAdapterInfo( const CPeekString& inDescription,
		const CGlobalId& inId );

protected:
	static CPeekString	s_strName;
	static CPeekString	s_strDisplayName;
	static CPeekString	s_strAdapterName;
	static CPeekString	s_strFilesLabel;
	static CPeekString	s_strPacketsLabel;
	static UInt64		s_nLinkSpeed;

protected:
	CGlobalId			m_Id;
	CPluginAdapterInfo	m_AdapterInfo;
	CChannelManager		m_ChannelManager;

	std::thread			m_CaptureThread;
	std::atomic<bool>	m_StopFlag;

	mutable size_t		m_nRunningCaptures;

	// CAdapterOptions like
	FilePath::Path				m_pathMonitor;
	CPeekString					m_strName;
	std::vector<std::wstring>	m_ayMask;
	SInt32						m_nSpeed;
	FilePath::Path				m_pathSave;
	bool						m_bDontCheckFileOpen;

	// Statistics
	struct OmniSummaryStats {
		Peek::SummaryStatEntry	Adapter;
		Peek::SummaryStatEntry	FileStat;
		Peek::SummaryStatEntry	PacketStat;
	};
	CGlobalId					m_ParentStat;
	OmniSummaryStats			m_Stats;

protected:
#ifdef HE_LINUX
	bool		IsFileOpen( const CFileDetail& inFileDetail );
#endif

	void		MoveDeleteFile( const CFileDetail& inFileDetail );
	void		MoveFileList( const CFileDetailList& inFileList );

	void		ProcessFile( const CFileDetail& inFileDetail );
	void		ProcessFileList( const CFileDetailList& inFileList );

public:
	;		COmniAdapter( const CGlobalId& inId );
	;		COmniAdapter();
	virtual	~COmniAdapter();

	virtual void	CopyAdapterInfo( IAdapterInfo** outAdapterInfo ) {
		m_AdapterInfo.Copy( outAdapterInfo );
	}

	virtual CAdapterInfo		GetAdapterInfo() const { return m_AdapterInfo; }
 	virtual const CPeekString&	GetAdapterName() const;
	virtual bool				GetAddress( CEthernetAddress& /*outAddress*/ ) const {
		return false;
	}
	virtual UInt64				GetLinkSpeed() const { return m_AdapterInfo.GetLinkSpeed(); }
	virtual tMediaType			GetMediaType() const { return m_AdapterInfo.GetMediaType(); }
	CAdapterOptions				GetOptions() const;

	virtual bool	Initialize( CAdapterInfo inInfo, CPeekEngineProxy inEngineProxy );
	virtual bool	IsCapturing() const { return (m_nRunningCaptures > 0); }
	bool			IsNotValid() const { return m_AdapterInfo.IsNotValid(); }
	bool			IsSave() const { return !m_pathSave.empty(); }
	bool			IsValid() const { return m_AdapterInfo.IsValid(); }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnUpdateSummaryStats( CSnapshot inSnapshot );

	void			PerformanceDelay( SInt64 inNanoseconds );

	virtual int		Run();

	void			SetOptions( const CAdapterOptions& inOptions );
	virtual int		StartCapture();
	virtual int		StopCapture();

	bool			UpdateOptions( const CAdapterOptions& inOptions );
};
