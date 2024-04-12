// ============================================================================
//	Case.h
//		interface for the CCase class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "FileMap.h"
#include "TargetAddress.h"
#include "ReportMode.h"
#include "ProtocolType.h"
#include "PeekTime.h"
#include "PeekFile.h"
#include "Version.h"

class COptions;
class CMsgStream;
class CPeekDataModeler;
class CPeekDataElement;


// ============================================================================
//		CBoolResult
// ============================================================================

template< bool B >
class TBoolResult
{
protected:
	bool	m_Value;

public:
	;	TBoolResult<B>() : m_Value( B ) {}

	bool operator=( bool inValue ) {
		if ( (m_Value == B) && (inValue != B) ) {
			m_Value = !B;
		}
		return m_Value;
	}

	;	operator bool() {
		return m_Value;
	}
};

typedef TBoolResult<true>	CBoolResultTrue;
typedef TBoolResult<false>	CBoolResultFalse;


// ============================================================================
//		CCaseSettings
// ============================================================================

class CCaseSettings
{
	friend class CCase;
	friend class CModePage;
	friend class CAddressPage;
	friend class CTriggerPage;
	friend class CAttachmentPage;

protected:
	BOOL			m_bAcceptPackets;
	BOOL			m_bWritePackets;
	tFileNameType	m_nPacketFileFormat;
	BOOL			m_bSummaryStats;
	BOOL			m_bLogMessages;
	CPeekStringX	m_strFileNamePrefix;
	BOOL			m_bAppendEmail;
	CPeekStringX	m_strOutputDirectory;
	BOOL			m_Protocols[kProtocol_Max];
	CReportMode		m_Mode;
	CPeekTime		m_StartTime;
	CPeekTime		m_StopTime;
#if SAVE_ATTACHMENTS
	BOOL			m_bSaveAttachments;
	CPeekStringX	m_strAttachmentDirectory;
#endif

public:
	static CPeekStringA	BuildModifyLog( const CPeekStringA& inPreAmble, const CPeekStringA& inMessage,
			const CPeekStringA&	inBefore, const CPeekStringA& inAfter ) {
#if (TOVERIFY)
		static const CPeekStringA	strTo( " to " );
		const CPeekStringA*	ayStrs[] = {
			&inPreAmble,
			&inMessage,
			&inBefore,
			&strTo,
			&inAfter,
			Tags::kxNewLine.RefA(),
			NULL
		};

		return FastCat( ayStrs );
#else
		CPeekOutStringA	ss;
		ss <<
			inPreAmble <<
			inMessage <<
			inBefore <<
			" to " <<
			inAfter <<
			Tags::kxNewLine.GetA();
		return ss;
#endif
	}
public:
	;			CCaseSettings();
	;			~CCaseSettings() {}

#if SAVE_ATTACHMENTS
	bool				GetSaveAttachments() const { return (m_bSaveAttachments != FALSE); }
	const CPeekStringX&	GetAttachmentDirectory() const { return m_strAttachmentDirectory; }

	void				SetSaveAttachments( bool inEnable) { m_bSaveAttachments = inEnable; }
	void				SetAttachmentDirectory( const CPeekString& inDirectory ) { m_strAttachmentDirectory = inDirectory; }
#endif

	const CPeekStringX&	GetFileNamePrefix() const { return m_strFileNamePrefix; }
	const CPeekStringX	GetFileNamePrefixAppend() const {
		CPeekString strResult( m_strFileNamePrefix );
		if ( IsAppendEmail() ) {
			strResult += L"<email>_";
		}
		return CPeekStringX( strResult );
	}
	const CPeekStringX&	GetOutputDirectory() const { return m_strOutputDirectory; }
	CReportMode&		GetReportMode() { return m_Mode; }
	const CReportMode&	GetReportMode() const { return m_Mode; }

	CPeekStringA	FormatA() const;
	CPeekString		FormatActive() const { return (IsActive()) ? L"Active" : L"Inactive"; }
	CPeekStringA	FormatLongA() const;
	CPeekString		FormatMode() const;
	CPeekString		FormatPackets() const;
	CPeekString		FormatProtocol() const;
	CPeekString		FormatStartTime() const { return m_StartTime.Format(); }
	CPeekStringA	FormatStartTimeA() const { return m_StartTime.FormatA(); }
	CPeekString		FormatStopTime() const { return m_StopTime.Format(); }
	CPeekStringA	FormatStopTimeA() const { return m_StopTime.FormatA(); }
	CPeekString		FormatTriggers() const;

	tFileNameType	GetPacketFileFormat() const { return m_nPacketFileFormat; }
	CPeekTime		GetStartTime() const { return m_StartTime; }
	CPeekTime		GetStopTime() const { return m_StopTime; }

	bool		IsAcceptPackets() const { return (m_bAcceptPackets != FALSE); }
	bool		IsActive( CPeekTime inTime = CPeekTime::Now() ) const {
		if ( !(m_Protocols[kProtocol_SMTP] || m_Protocols[kProtocol_POP3]) ) {
			return false;
		}
		return ((inTime >= m_StartTime) && (inTime < m_StopTime));
	}
	bool		IsAppendEmail() const { return (m_bAppendEmail != FALSE); }
	bool		IsWritePackets() const { return (m_bWritePackets != FALSE); }
	bool		IsSummaryStats() const { return (m_bSummaryStats != FALSE); }
	bool		IsLogMessages() const { return (m_bLogMessages != FALSE); }
	bool		IsFullEmail() const { return m_Mode.IsFullEmail(); }
	bool		IsFullPacket() const { return m_Mode.IsFullPacket(); }
	bool		IsMatch( const CCaseSettings& inSettings ) const;
	bool		IsMode( tReportModes inMode ) const { return (m_Mode.GetMode() == inMode); }
	bool		IsPenEmail() const { return m_Mode.IsPenEmail(); }
	bool		IsPenText() const { return m_Mode.IsPenText(); }
	bool		IsPenXml() const { return m_Mode.IsPenXml(); }
	bool		IsPenPerAddress() const { return m_Mode.IsPenPerAddress(); }
	bool		IsProtocolOfInterest( tProtocolTypes inType ) const {
		return (CProtocolType::IsValid( inType )) ? ((m_Protocols[static_cast<size_t>( inType )]) ? true : false) : false;
	}

	bool		Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;
	bool		LogUpdate( const CPeekStringA& inPreAmble, const CCaseSettings& inSettings, CFileEx& inFile ) const;

	bool		Model( CPeekDataElement& ioCase );

	void		SetAcceptPackets( bool inEnable ) { m_bAcceptPackets = inEnable; }
	void		SetWritePackets( bool inEnable ) { m_bWritePackets = inEnable; }
	void		SetSummaryStats( bool inEnable ) { m_bSummaryStats = inEnable; }
	void		SetLogMessages( bool inEnable ) { m_bLogMessages = inEnable; }
	void		SetFileNamePrefix( const CPeekString& inFileNamePrefix ) {
		m_strFileNamePrefix = CFileEx::LegalizeFileName( inFileNamePrefix );
	}
	void		SetAppendEmail( bool inEnable ) { m_bAppendEmail = inEnable; }
	void		SetOutputDirectory( const CPeekString& inOutputDirectory ) {
		m_strOutputDirectory = CFileEx::LegalizeFilePath( inOutputDirectory );
	}
	void		SetProtocol( tProtocolTypes inType, bool inEnabled ) {
		if ( CProtocolType::IsValid( inType ) ) m_Protocols[inType] = inEnabled;
	}
	void		SetStartTime( CPeekTime inStartTime ) { m_StartTime = inStartTime; }
	void		SetStopTime( CPeekTime inStopTime ) { m_StopTime = inStopTime; }
};


// ============================================================================
//		CCase
// ============================================================================

class CCase
{
	friend class CEditTargetDlg;

public:
	enum {
		kColumnId_Id,
		kColumnId_Name,
		kColumnId_Address,
		kColumnId_Count,
		kColumnId_Protocol,
		kColumnId_Mode,
		kColumnId_State,
		kColumnId_Triggers,
		kColumnId_Folder
	};

	typedef unsigned int	tCaseId;

protected:
	tCaseId				m_nId;
	BOOL				m_bEnabled;
	BOOL				m_bDisplayIntegrity;
	CPeekStringX		m_strName;
	CTargetAddressList	m_AddressList;
	CCaseSettings		m_Settings;

#if SAVE_ATTACHMENTS
	void				SaveAttachments( CMsgStream* inMsgStream );
#endif

public:
	;		CCase();
	;		CCase( size_t nIndex );
	;		CCase( const CCase& inCase );
	virtual	~CCase() {}

	bool	Analyze( CMsgStream* inMsgStream );

	CPeekStringA	FormatA() const;
	CPeekStringA	FormatLongA() const;

	CTargetAddressList&			GetAddressList() { return m_AddressList; }
	const CTargetAddressList&	GetAddressList() const { return m_AddressList; }
	static const CPeekString&	GetDefaultName();
	CPeekString					GetColumnText( int inId ) const;
	tCaseId						GetId() const { return m_nId; }
	const CPeekStringX&			GetName() const { return m_strName; }
	const CPeekStringX&			GetOutputDirectory() const { return m_Settings.GetOutputDirectory(); }
	CCaseSettings&				GetSettings() { return m_Settings; }
	const CCaseSettings&		GetSettings() const { return m_Settings; }

	bool	IsAcceptPackets() const { return m_Settings.IsAcceptPackets(); }
	bool	IsActive() const { return (IsEnabled()) ? m_Settings.IsActive() : false; }
	bool	IsAddressOfInterest( const CPeekString& inAddress ) const {
		return (IsActive()) ? m_AddressList.IsAddressOfInterest( inAddress ) : false;
	}
	bool	IsEnabled() const { return (m_bEnabled != FALSE); }
	bool	IsFullEmail() const { return m_Settings.IsFullEmail(); }
	bool	IsFullPacket() const { return m_Settings.IsFullPacket(); }
	bool	IsDisplayIntegrity() const { return (m_bDisplayIntegrity != FALSE); }
	bool	IsLogMessages() const { return m_Settings.IsLogMessages(); }
	bool	IsMatch( const CCase& inCase ) const;
	bool	IsMode( tReportModes inMode ) const { return m_Settings.IsMode( inMode ); }
	bool	IsPenEmail() const { return m_Settings.IsPenEmail(); }
	bool	IsPenPerAddress() const { return m_Settings.IsPenPerAddress(); }
	bool	IsPenText() const { return m_Settings.IsPenText(); }
	bool	IsProtocolOfInterest( CProtocolType inProtocolType ) const {
		return (IsActive()) ? m_Settings.IsProtocolOfInterest( inProtocolType ) : false;
	}
	bool	IsSummaryStats() const { return m_Settings.IsSummaryStats(); }
	bool	IsWritePackets() const { return m_Settings.IsWritePackets(); }

	bool	Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;
	bool	LogUpdate( const CPeekStringA& inPreAmble, const CCase& inCase, CFileEx& inFile ) const;

	bool	Model( CPeekDataElement& ioCase );

	void	SetEnabled( bool inEnable ) { m_bEnabled = inEnable; }
	void	SetName( const CPeekString& inName ) { m_strName = CFileEx::LegalizeFileName( inName ); }
};


// ============================================================================
//		CCaseList
// ============================================================================

class CCaseList
	: public CPeekArray<CCase>
{
public:
	static size_t	s_nMaxAddresses;

public:
	static size_t	GetMaxSize() { return s_nMaxAddresses; }

public:
	;			CCaseList() {}
	;			CCaseList( const CCaseList& in ) { Copy( in ); }
	CCaseList&	operator=( const CCaseList& in ) {
		Copy( in );
		return *this;
	}

	bool	Analyze( CMsgStream* inMsgStream );

	bool	Find( CCase::tCaseId inId, size_t& outIndex ) const;
	bool	Find( const CPeekString& inName, std::vector<CCase>::const_iterator& outIterator ) const;
	
	int		GetCount32() const { return (GetCount() < kIndex_Invalid) ? (GetCount() & 0x0FFFFFFFF) : 0; }

	bool	IsAcceptPackets() const;
	bool	IsAddressOfInterest( const CPeekString& inAddress, size_t& outIndex ) const;
	bool	IsLogMessages() const;
	bool	IsProtocolOfInterest( CProtocolType inProtocolType ) const;
	bool	IsSlicePackets() const;
	bool	IsSummaryStats() const;
	
	bool	Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;
	bool	LogUpdate( const CPeekStringA& inPreAmble, const CCaseList& inCases, CFileEx& inFile ) const;
	
	bool	Model( CPeekDataElement& ioCaseList );
};
