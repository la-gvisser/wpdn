// ============================================================================
//	RadiusMessage.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

class CRadiusMessage
{

public:

	CRadiusMessage() {}
	~CRadiusMessage() {}

	CRadiusMessage( CPeekString& strTime, CPeekString& strMessage ) 
		: m_strTime( strTime ), m_strMessage( strMessage )
	{}

	CPeekString GetTime() {
		return m_strTime;
	}
	CPeekString GetMessage() {
		return m_strMessage;
	}
	void SetScreenMessage( CPeekString strScreenMessage ) {
		m_strScreenMessage = strScreenMessage;
	}
	CPeekString GetScreenMessage() {
		return m_strScreenMessage;
	}
	void SetLogFileMessage( CPeekString strLogFileMessage ) {
		m_strLogFileMessage = strLogFileMessage;
	}
	CPeekString GetLogFileMessage() {
		return m_strLogFileMessage;
	}
	
protected:
	CPeekString m_strTime;
	CPeekString m_strMessage;
	CPeekString m_strLogFileMessage;
	CPeekString m_strScreenMessage;
};