// ============================================================================
//		RadiusContextInterface.h
// ============================================================================

#pragma once

class CCaseOptions;

// ============================================================================
//		IRadiusContext Interface
// ============================================================================

class IRadiusContext
{
public:
	virtual void		AddLogToScreenMessage( const CPeekString& strMsg ) = 0;
	virtual void		CreateAnUniqueCaseCaptureId( CCaseOptions* pPluginCaseOptions, CCaseOptions* pContextCaseOptions ) = 0;
	virtual UInt32		GetCurrentDisplayPacketNumber() = 0;
//	virtual void		DoLogMessage( int inSeverity, const CPeekString& inMessage ) = 0;
//	virtual bool		IsCapturing( bool bHaveLock = 0 ) = 0;
//	virtual CPeekString GetCaptureName() = 0;
	virtual bool		IsCapturing() = 0;
};
