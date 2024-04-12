// =============================================================================
//	Activate.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004-2005. All rights reserved.

#ifndef ACTIVATE_H
#define ACTIVATE_H

#include "AGTypes.h"

namespace Activation
{
	enum { MAX_CAPABILITY = 16 };

	/// \internal
	/// \enum ActivationType
	/// \ingroup Activation
	/// \brief Activation types.
	/// \see IWPActivationInfo
	enum ActivationType
	{
		kCapture_Ethernet,
		kCapture_Wireless,
		kCapture_GigHardware,
		kCapture_WAN,
		kFeature_Expert,
		kFeature_PeerMap,
		kFeature_Voice,
		kFeature_LocalCapture,
		kFeature_RemoteCapture,
		kFeature_Graphs,
		kCapture_LocalWireless,
		kFeature_EngineStats,
		kFeature_TelchemyVoIP,
		kFeature_Apdex,
		kFeature_NoExpertLimit,
		kFeature_NoAdapterLimit,
		kFeature_NoRemoteConsoleLimit,

		kFeature_Dashboard,	// This and below new for 5.0
		kFeature_WebViews,
		kFeature_RadcomVoIP,
		kFeature_ForensicSearch,
		kFeature_PromiscuousCapture,

		kFeature_CallPlayback,	// New for Omni 6.0

		kActivationType_Count,
		kActivationType_First	= kCapture_Ethernet,
		kActivationType_None	= -1
	};

	HRESULT Activate( 
		LPCTSTR lpszURL,
		BOOL    bSecure,
		LPCTSTR lpszSerNum,
		UInt32  inProdCode,
		UInt32  inMajorVers,
		UInt32  inMinorVers,
		LPCTSTR lpszCPUID,
		LPCTSTR lpszName,
		LPCTSTR lpszEmail,
		LPCTSTR lpszCompany,
		UInt32*	outResult,
		LPTSTR  outContent,
		UInt32  cbContent, 
		LPTSTR  outActivationCode,
		UInt32  cbActivationCode );
				  
	bool GetCPUID(
		TCHAR*	pszCPUID,
		bool	bAppendChecksum );

	int CompareActivationKey( 
		LPCTSTR	inSerialNumber, 
		UInt32	inProductID, 
		UInt32	inMajorVersion, 
		UInt32	inMinorVersion, 
		LPCTSTR	inActivationKey );

	HRESULT GetActivationInfo(
		LPCTSTR	lpszKey,
		UInt32*	pnProduct,
		UInt32* pnMajorVer,
		UInt32* pnMinorVer,
		BOOL*	payOptions );

	bool GetExpirationDate(
		LPCTSTR	lpszKey,
		UInt16*	pnMonth,
		UInt16*	pnYear );

	HRESULT IsOptionActivated(
		UInt32			nProductID,
		UInt32			nMajorVer,
		UInt32			nMinorVer,
		BOOL*			payOptions,
		ActivationType	nActivationType,
		BOOL*			pbIsActivated );

	HRESULT GetOptionIndex(
		UInt32			nProductID,
		UInt32			nMajorVer,
		UInt32			nMinorVer,
		ActivationType	nActivationType,
		SInt32&			outOptionIndex );

}	// namespace Activation

#endif
