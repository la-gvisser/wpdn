// ============================================================================
//	PenInfo.h
//		interface for the CPenInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#pragma once

#include "MsgInfo.h"

// -----------------------------------------------------------------------------
//		CPenInfo
// -----------------------------------------------------------------------------

class CPenInfo
	:	public CMsgInfo
{
protected:
	CStringA			m_strMessageId;
	CArrayStringA		m_strsReceived;

	// Optional Headers
	CStringA			m_strReturnPath;
	CStringA			m_strReplyTo;
	CStringA			m_strXOriginatingIp;
	CStringA			m_strXApparentlyTo;
	CStringA			m_strXApparentlyFrom;

	CStringA			m_strTxtDate;
	CStringA			m_strTxtBody;
	CStringA			m_strEmlBody;

	void			FormatEmlBody();
	void			FormatTxtDateBody();

public:
	;		CPenInfo( CMsgStream* inMsgStream );

	void	WritePenEml( const CString& inFileName, CArrayString& outTarget );
	void	WritePenEmls( const CString& inPrefix, const CString& inOutputDirectory, CArrayString& outTargets );
	void	WritePenText( const CString& inFileName, CArrayString& outTarget );
	void	WritePenTexts( const CString& inPrefix, const CString& inOutputDirectory, CArrayString& outTargets );
};
