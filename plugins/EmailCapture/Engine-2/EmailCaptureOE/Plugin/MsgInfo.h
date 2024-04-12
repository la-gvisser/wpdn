// ============================================================================
//	MsgInfo.h
//		interface for the CMsgInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "mimepp.h"
#include "IpHeaders.h"
#include "MsgAddress.h"
#include "FileEx.h"
#include <map>

class CMsgStream;


// ============================================================================
//		tMsgSizeInfo
// ============================================================================

typedef struct tMsgSizeInfo {
	size_t	nMsgSize;
	size_t	nHdrSize;
} tMsgSizeInfo;


// ============================================================================
//		CMsgInfo
// ============================================================================

class CMsgInfo
{
protected:
	typedef enum {
		kUndefined = 0,
		kOrigDate,
		kMailFrom,
		kRcptTo,
		kFrom,
		kSender,
		kReplyTo,
		kTo,
		kCC,
		kBCC,
		kMessageId,
		kSubject,
		kReceived,
		kReturnPath,
		kComments,
		kAttachments,
		kOther,
		kNonField,
		kEndOfHeaders = -1
	} FieldTypes;

	class CFieldName
	{
	public:
		FieldTypes		m_Type;
		CPeekStringA	m_strValue;

		CFieldName() : m_Type( kUndefined ) {}
		CFieldName( FieldTypes T, CPeekStringA Str ) : m_Type( T ), m_strValue( Str ) {}
	};

	class CFieldNameMap :
		public std::map<CPeekStringA, CMsgInfo::CFieldName>
	{
	public:
		CFieldNameMap();
	};

protected:
	static CFieldNameMap	m_FieldNameMap;

	CMsgStream*				m_pMsgStream;
	mimepp::Headers			m_Headers;

	CPeekStringX			m_strProtocol;
	CMsgAddress				m_addrProtocol;
	CPeekTime				m_InterceptTime;
	CIpAddressPortPair		m_IpPortPair;
	tMsgSizeInfo			m_SizeInfo;

	CMsgAddressList			m_MailFromList;
	CMsgAddressList			m_RcptToList;
	CPeekStringA			m_strDate;

	static CPeekStringA	FormatAddressA( const mimepp::Mailbox& inAddress );

	CPeekString		FormatAddress( const CMsgAddress& inTarget );
	void			AddFormattedAddress( CArrayString& outAddresses );
	void			AddFormattedAddress( const CMsgAddress& inTarget, CArrayString& outAddresses );
	CPeekStringA	FormatEmlHeaderA();
	CPeekStringA	FormatEmlHeaderA( const CTargetAddress& inTarget );
	CPeekStringA	FormatTxtHeaderA();
	CPeekStringA	FormatTxtHeaderA( const CMsgAddress& inTarget );

public:
	;			CMsgInfo( CMsgStream* inMsgStream );
	virtual		~CMsgInfo() {}

	const CMsgAddress*	GetFirstSender() const;
	CPeekString			GetFirstSenderAddress() const;

	bool		HasTarget( const CTargetAddress& inTargetAddress ) const {
		return (m_MailFromList.Contains( inTargetAddress ) || m_RcptToList.Contains( inTargetAddress ));
	}

	CPeekString	FormatAddressList();
	CPeekString	FormatTarget( const CMsgAddress& inTarget );

	void		WriteFullEml( const CPeekString& inFileName, CArrayString& outTargets );
	bool		WriteFullEml( const CTargetAddress& inTargetAddress, const CPeekString& inFileName );
	bool		WriteFullEml( const CTargetAddress& inTargetAddress, CFileEx& inFile );
	bool		WriteFullPkt( const CTargetAddress& inTargetAddress, const CPeekString& inFileName );
};
