//	Extent.h
//		interface for the CExtent class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#pragma once

// -----------------------------------------------------------------------------
//		CExtent
// -----------------------------------------------------------------------------

#include "Options.h"
#include "EMailStats.h"
#include "LogQueue.h"
#include "EMailCaptureContext.h"

class CExtent
{
protected:
	CPeekPlugin*	m_pPlugin;
	COptions		m_Options;
	CLogQueue		m_LogQueue;
	CEMailStats		m_Stats;

public:
	;		CExtent( CPeekPlugin* inPlugin );

	void	Load(){ m_Stats.Load( m_pPlugin ); }

	CPeekPlugin*	GetPlugin(){ return m_pPlugin; }

	// Options
	void		ReadOptions() { m_Options.Read(); }
	void		WriteOptions() { m_Options.Write(); }
	COptions&	GetOptions();
	void		SetOptions( COptions& inOptions );

	bool	GetIntegrityOptions( tIntegrityOptions& in ) { return m_Options.GetIntegrityOptions( in ); }

	// General Options
	const CString&	OutputDirectory() const { return m_Options.m_General.m_strOutputDirectory; }
	bool	MonitorSmtp() const { return (m_Options.m_General.m_bMonitorSmtp != FALSE); }
	bool	MonitorPop3() const { return (m_Options.m_General.m_bMonitorPop3 != FALSE); }
	bool	MonitorSmtpOrPop3() const { return (MonitorSmtp() || MonitorPop3()); }
	bool	DisplayLog() const { return (m_Options.m_General.m_bDisplayLog != FALSE); }
	bool	DisplayCapture() const { return (m_Options.m_General.m_bDisplayCapture != FALSE); }
	bool	DisplaySummary() const { return (m_Options.m_General.m_bDisplaySummary != FALSE); }
	bool	ProcessPackets() const { return (m_Options.m_General.m_bProcessPackets != FALSE); }
	bool	MultiThreaded() const { return (m_Options.m_General.m_bMultiThreaded != FALSE); }

	// Address Options
	bool				CaptureAll() const { return m_Options.m_Address.m_TargetList.CaptureAll(); }
	bool				ContainsAddress( const CStringA& inAddress ) { return m_Options.m_Address.m_TargetList.Contains( inAddress ); }
	const CUserList&	GetTargetList(){ m_Options.m_Address.m_TargetList.Lock(); return m_Options.m_Address.m_TargetList; }
	void				ReleaseTargetList() { m_Options.m_Address.m_TargetList.Unlock(); }

	// Mode Options
	bool	IsFullCapture() const { return (m_Options.m_Mode.m_nCaptureMode != 0); }
	bool	FullEml() const { return (m_Options.m_Mode.m_bFullEml != FALSE); }
	bool	FullPkt() const { return (m_Options.m_Mode.m_bFullPkt != FALSE); }
	bool	FullEmlOrPkt() const { return (FullEml() || FullPkt()); }
	bool	PenEml() const { return (m_Options.m_Mode.m_bPenEml != FALSE); }
	bool	PenText() const { return (m_Options.m_Mode.m_bPenText != FALSE); }
	bool	PenEmlOrText() const { return (PenEml() || PenText()); }

	// LogQueue
	void	PostLogMessages( CPeekContextList& inContextList ) { m_LogQueue.PostMessages( inContextList ); }
	void	RemoveAll() { m_LogQueue.RemoveAll(); }
	void	LogMessage( const CString& inMessage ){ if ( DisplayLog() ) m_LogQueue.AddMessage( inMessage ); }

	// Stats
	CEMailStats::EMailStat	StatFirstIndex() const { return m_Stats.FirstIndex(); }
	CEMailStats::EMailStat	StatNextIndex( CEMailStats::EMailStat pos ) const { return m_Stats.NextIndex( pos ); }
	void			Increment( CEMailStats::EMailStat inStat ) { m_Stats.Increment( inStat ); }
	UInt32			Get( CEMailStats::EMailStat inStat ) const { return m_Stats.Get( inStat ); } 
	const CString&	Label( CEMailStats::EMailStat inStat ) const { return m_Stats.Label( inStat ); }

	// Logging and Stats
	void	Reset() { m_Stats.Reset(); RemoveAll(); }
	void	NoteDetect( CEMailStats::EMailStatType inType );
	void	NoteDetect( CEMailStats::EMailStatType inType, const CTargetAddress& inTarget );
	void	NoteTarget( const CString& inTarget, const CString& inTemplate );
	//void	NoteTarget( const CTargetAddress& inTarget, const CString& inTemplate );
	void	NoteTargets( const CArrayString& inTargets, const CString& inTemplate );
	//void	NoteTargets( const CTargetAddressList& inTargets, const CString& inTemplate );
	void	NoteFileSave( const CString& inTarget );
	void	NoteReset( CEMailStats::EMailStatType inType );
	void	NoteReset( CEMailStats::EMailStatType inType, const CTargetAddress& inTarget );
	void	NoteMalformed( CEMailStats::EMailStatType inType );
	//void	NoteCaptured( type: Pen/Full, Target );
	void	NoteFileError( const CString& inFileName );
	void	NoteWriteError( const CString& inFileName );
	void	NoteTimedOutMessages( CEMailStats::EMailStatType inType, const CArrayString& inRemovedList );
};
