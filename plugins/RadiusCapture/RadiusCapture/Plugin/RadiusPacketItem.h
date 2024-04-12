// ============================================================================
// RadiusPacketItem.h:
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "Options.h"
#include "RadiusPacketData.h"

// Codes
const UInt8		RADCODE_ACCESSREQUEST		= 1;
const UInt8		RADCODE_ACCESSACCEPT		= 2;
const UInt8		RADCODE_ACCESSREJECT		= 3;
const UInt8		RADCODE_ACCOUNTINGREQUEST	= 4;
const UInt8		RADCODE_ACCOUNTINGRESPONSE	= 5;
const UInt8		RADCODE_ACCESSCHALLENGE		= 11;

// Codes Received
#define		RECEIVED_ACCESSREQUEST			0x01
//#define 	RECEIVED_ACCESSACCEPT			0x02
#define 	RECEIVED_ACCESSRESPONSE			0x02
#define 	RECEIVED_ACCESSREJECT			0x04
#define		RECEIVED_ACCOUNTINGREQUEST		0x08
#define		RECEIVED_ACCOUNTINGRESPONSE		0x10
#define		RECEIVED_ACCESSCHALLENGE		0x20

// Attributes
const UInt8		RADATT_USERNAME				= 1;
const UInt8		RADATT_USERPASSWORD			= 2;
const UInt8		RADATT_CHAPPASSWORD			= 3;
const UInt8		RADATT_NASIPADDRESS			= 4;  // IP of the NAS
const UInt8		RADATT_NASPORT				= 5;
const UInt8		RADATT_SERVICETYPE			= 6;
const UInt8		RADATT_FRMIPADDRESS			= 8;
const UInt8		RADATT_STATUS				= 40;
const UInt8		RADATT_SESSIONID			= 44;

// Accounting request status (RADATT_STATUS) attribute values
const UInt32	ACCOUNTING_START			= 1;
const UInt32	ACCOUNTING_STOP				= 2;
const UInt32	ACCOUNTING_OFF				= 8; 

// Monitoring start or stop
#define			MONITORING_NO_ACTION		0
#define			MONITORING_START			1
#define			MONITORING_START_NOREPORT	2
#define			MONITORING_STOP				3

#pragma pack( push, 1 )
typedef struct {
	UInt8	uCode;
	UInt8	uIdentifier;
	UInt16	uLength;
	UInt8	ayAuthenticator[16];
} RadiusHeader;

typedef struct {
	UInt8	uAttribute;
	UInt8	uLength;
	UInt8	uData[4];	// just a placeholder size.
} RadiusAttribute;
#pragma pack( pop )

typedef enum {
	NULLPacketPointer = 0,
	NULLPacketDataPointer,
	NULLPluginPointer,
	NULLOutputPointer,
	WrongPacketSize,
} RediusErrorCode;


// ============================================================================
//		CRadiusPacketItem
// ============================================================================

class CRadiusPacketItem
{

	
public:

	CRadiusPacketItem();
	~CRadiusPacketItem() {
	}

	CRadiusPacketItem( const CRadiusPacketItem& inOriginal ) {
		Copy( inOriginal );
	}
	CRadiusPacketItem& operator= ( const CRadiusPacketItem& inOriginal ) {
		return Copy( inOriginal );
	}

/*
	bool RadiusTransactionComplete();
	void Update( CRadiusPacketItem& theRadiusPacketItem );
	bool IsMatch( CRadiusPacketItem& CandidateItem );
*/
	bool GetIPAndRadiusInfo( CPacket& inPacket );
//		const PluginPacket*		inPacket,
//		const UInt8*			inPacketData,
//		UInt8					inMediaType,
//		UInt8					inMediaSubType,
//		UInt32					inProtoSpecMatched,
//		COptions*				inOptions );

	UInt32	GetSourceIPAddr() {
		return m_uSrcAddr;
	}
	UInt32	GetDestIPAddr() {
		return m_uDstAddr;
	}
	void	InitializeCaptureOptions() {
//		m_CaptureOptions.Initialize();
	}
	UInt16	GetRadiusPort() {
		return m_uRadiusPort;
	}
	bool	IPMatch( CRadiusPacketItem& OtherItem ) {
		if ( (m_uSrcAddr == OtherItem.m_uSrcAddr && m_uDstAddr == OtherItem.m_uDstAddr) ||
			 (m_uSrcAddr == OtherItem.m_uDstAddr && m_uDstAddr == OtherItem.m_uSrcAddr) ) {
			return true;
		}
		return false;
	}
	bool	IsRadiusOfInterest(){
		if( m_uCode == RADCODE_ACCESSREQUEST     ||  m_uCode == RADCODE_ACCESSACCEPT	   ||
			m_uCode == RADCODE_ACCOUNTINGREQUEST ||  m_uCode == RADCODE_ACCOUNTINGRESPONSE ||
			m_uCode == RADCODE_ACCESSREJECT		 ||  m_uCode == RADCODE_ACCESSCHALLENGE ){
			return true;
		}
		return false;
	}
//	bool	IsRadiusProcessPossibility() {
//		if( m_uCode == RADCODE_ACCESSACCEPT || m_uCode == RADCODE_ACCOUNTINGREQUEST ) {
//			return true;
//		}
//		return false;
//	}
	CPeekString	GetRadiusUserName() {
		return m_strName;
	}
	bool	HasName() {
		return ( m_strName != L"" );
	}
	bool	HasFramedIPAddr() {
		return( static_cast<bool>( m_uFramedIPAddress != 0 ) );
	}
	UInt32	GetFramedIPAddr() {
		return m_uFramedIPAddress;
	}
	UInt8	GetRadiusIdentifier() {
		return m_uIdentifier;
	}
	UInt8	GetAccessIdentifier() {
		return m_uAccessIdentifier;
	}
	UInt8	GetAccountingIdentifier() {
		return m_uAccountingIdentifier;
	}
	bool	PacketIsRadius() {
		return m_bIsRadius;
	}
	UInt64	GetLastUpdateTime() {
		return m_LastUpdateTime;
	}
	void	SetLastUpdateTime() { 
		time_t uTime;
		time( &uTime );
		m_LastUpdateTime = static_cast<UInt64>(uTime);
	}
	void	SetMonitoring( UInt32 uMonitoring ) {
		m_uMonitorting = uMonitoring;
	}
	UInt32	GetMonitoring() {
		return m_uMonitorting;
	}
	bool	ProcessRadius() {
		if ( GetRadiusUserName().IsEmpty() ) return false;
		return true;
	}

/*
	CCaptureOptions& GetCaptureOptions() {
		return m_CaptureOptions;
	}
*/
	protected:
		bool				GetRadiusInfo( const UInt8* pIPData );
		CRadiusPacketItem&	Copy( const CRadiusPacketItem& inOriginal );

		void				InitializeFramedIPAddress() {
			m_uFramedIPAddress = 0;
		}

	protected:
		CIpAddress m_uSrcAddr;
		CIpAddress m_uDstAddr;
//		UInt32	m_uSrcAddr;
//		UInt32	m_uDstAddr;
		UInt16	m_uRadiusPort;
		bool	m_bIsRadius;
		UInt32	m_uMonitorting;
		CPeekString	m_strName;
		UInt32	m_uFramedIPAddress;
		UInt64	m_LastUpdateTime;
		UInt8	m_uCode;
		UInt8	m_uIdentifier;
		UInt8	m_uStatus;
		UInt8	m_uAccessIdentifier;
		UInt8	m_uAccountingIdentifier;
//		bool	m_bDataWasAccepted;
//		bool	m_bProcessRadius;
/*
		CCaptureOptions m_CaptureOptions;
*/
};
