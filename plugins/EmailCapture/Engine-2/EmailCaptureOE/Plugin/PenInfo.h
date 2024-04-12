// ============================================================================
//	PenInfo.h
//		interface for the CPenInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "MsgInfo.h"
#include "FileEx.h"
#include "PeekDataModeler.h"
#include <memory>

using namespace std;

class CTargetAddress;
class CTargetSettings;

#define kUtil_Flag_None			0x0000
#define kUtil_Flag_No_Day		0x0001
#define kUtil_Flag_No_TimeZone	0x0002


// ============================================================================
//		CPenInfo
// ============================================================================

class CPenInfo
	:	public CMsgInfo
{
protected:
	CPeekStringX		m_strMessageId;
	CArrayStringA		m_strsReceived;

	// Optional Headers
	CPeekStringA		m_strReturnPath;
	CPeekStringA		m_strReplyTo;
	CPeekStringA		m_strXOriginatingIp;
	CPeekStringA		m_strXApparentlyTo;
	CPeekStringA		m_strXApparentlyFrom;

	CPeekStringA		m_strTxtDate;
	CPeekStringA		m_strTxtBody;
	CPeekStringA		m_strEmlBody;

	std::unique_ptr<CPeekDataModeler>	m_spModeler;

	//void	AddXmlContactList( CEasyXMLElement* inList, CMsgAddressList& inMsgList );

	void			FormatEmlBody();
	CPeekStringA	FormatTimeStampA( const CPeekTime& inTime, int inFlags = kUtil_Flag_None );
	void			FormatTxtDateBody();
	void			FormatXmlContacts();
	void			FormatXmlContacts( const CMsgAddress& inTarget );
	void			FormatXmlDocument( bool inReset );

	CPeekStringA	CleanAddress( const CPeekStringA& inAddress ) { return inAddress; }

public:
	;		CPenInfo( CMsgStream* inMsgStream );

	bool	WritePenEml( const CPeekString& inFileName );
	bool	WritePenEml( CFileEx& inFile );
	bool	WritePenEml( const CTargetAddress& inTargetAddress, const CPeekString& inFileName );
	bool	WritePenEml( const CTargetAddress& inTargetAddress, CFileEx& inFile );
	bool	WritePenText( CFileEx* inFile );
	bool	WritePenText( const CTargetAddress& inTargetAddress, CFileEx* inFile );
	bool	WritePenXml( CFileEx* inFile );
	bool	WritePenXml( const CTargetAddress& inTargetAddress, CFileEx* inFile );
};


// ============================================================================
//		CPenInfoPtr
// ============================================================================

typedef std::unique_ptr<CPenInfo>		CPenInfoPtr;
