// ============================================================================
// DnsEntry.h:
//      interface for the CDnsEntry classes.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "Domain.h"
#include "DNSProcess.h"

class CDnsEntryList;

// ============================================================================
//		Globals
// ============================================================================

// DNS Type field values
enum {
	kType_HostAddress = 1,		// A
	kType_AuthNameServer,		// NS
	kType_MailDestination,		// MD
	kType_MailForwarder,		// MF
	kType_CononicalName,		// CNAME
	kType_StartAuthZone,		// SOA
	kType_MailDomainName,		// MB
	kType_MailGroup,			// MG
	kType_MailRename,			// MR
	kType_NULL,					// NULL
	kType_KnownService,			// WKS
	kType_DomainNamePtr,		// PTR
	kType_HostInfo,				// HINFO
	kType_MailInfo,				// MINFO
	kType_MailExchange,			// MX
	kType_TextString			// TXT
};

enum {
	kRequest_ZoneTrans = 252,	// AXFR
	kRequest_MailBox,			// MAILB
	kRequest_MailAgent			// MAILA
};

enum {
	kClass_Internet = 1,		// IN
	kClass_CSNet,				// CSNet
	kClass_Chaos,				// CH
	kClass_Hesiod				// HS
};

class CDnsEntryList;

// ============================================================================
//		CDnsEntry
// ============================================================================

class CDnsEntry
{
public:
	typedef enum {
		kEntry_Question,
		kEntry_Answer,
		kEntry_Authority,
		kEntry_Resource,
		kEntry_Unknown = -1
	} tEntryType;

protected:
	CDnsEntryList&	m_Parent;
	tEntryType		m_Entry;

	UInt32			m_nType;
	UInt32			m_nClass;
	UInt32			m_nTimeToLive;
	CPeekString		m_strName;
	CByteArray		m_Data;

	CPeekString		m_strAltName;
	CIpAddress		m_IpAddress;

	void		ParseError( const wchar_t& inMessage ) {
		ParseError( (CPeekString)inMessage );
	}
	void		ParseError( const CPeekString inMessage );

public:
	;		CDnsEntry( tEntryType inEntry, CDnsEntryList& inParent )
		:	m_Parent( inParent )
		,	m_Entry( inEntry )
		,	m_nType( 0 )
		,	m_nClass( 0 )
		,	m_nTimeToLive( 0 )
	{
	}
	;		CDnsEntry( const CDnsEntry& in )
		:	m_Parent( in.m_Parent )
		,	m_Entry( in.m_Entry )
		,	m_nType( in.m_nType )
		,	m_nClass( in.m_nClass )
		,	m_nTimeToLive( in.m_nTimeToLive )
		,	m_strName( in.m_strName )
		,	m_strAltName( in.m_strAltName )
		,	m_IpAddress( in.m_IpAddress )
	{
		m_Data.Copy( in.m_Data );
	}
	;		~CDnsEntry() {}

	CIpAddress	GetAddress() const { return m_IpAddress; }
	CPeekString	GetAltName() const { return m_strAltName; }
	CPeekString	Format() const;
	CPeekString	GetName() const { return m_strName; }
	bool		IsEntry( tEntryType inEntry ) const { return (m_Entry == inEntry); }
	bool		IsType( UInt32 inType ) const { return (m_nType == inType); }
	bool		IsValid() const { return (reinterpret_cast<void*>( &m_Parent ) != 0); }
	int			ParseQuestion( UInt32 inLength, const UInt8* inData );
	int			ParseRecord( UInt32 inLength, const UInt8* inData );
};


// ============================================================================
//		CDnsEntryList
// ============================================================================

class CDnsEntryList
	:	public CAtlArray<CDnsEntry>
{

	friend class CDnsEntry;

protected:
	UInt32				m_nLength;
	const tDnsHeader*	m_pHeader;
	UInt32				m_nQuestions;
	UInt32				m_nAnswers;
	UInt32				m_nAuthorities;
	UInt32				m_nResources;
	SInt32				m_sDNSDataLength;
	UInt32				m_nDNSType;
	CDNSProcess&		m_DNSProcess;

	void		ParseError();
	void		ParseError( const wchar_t& inMessage ) {
		ParseError( (CPeekString)inMessage );
	}
	void		ParseError( CPeekString inMessage ) {
		m_DNSProcess.LogError( inMessage );
	}

public:
	;		CDnsEntryList(
		CDNSProcess& inDNSProcess )
		:	m_nLength( 0 )
		,	m_pHeader( NULL )
		,	m_nQuestions( 0 )
		,	m_nAnswers( 0 )
		,	m_nAuthorities( 0 )
		,	m_nResources( 0 )
		,	m_sDNSDataLength( 0 )
		,	m_nDNSType( 0 )
		,	m_DNSProcess( inDNSProcess )
	{}
	;		~CDnsEntryList() {}

	int			FindQuestion( CPeekString inName ) const {
		for ( UInt32 i = 0; i < m_nQuestions; i++ ) {
			const CDnsEntry&	theEntry( GetAt( i ) );
			if ( inName.CompareNoCase( theEntry.GetName() ) == 0 ) {
				return i;
			}
		}
		return -1;
	}
	CPeekString	Format() const {
		size_t	nCount = GetCount();
		CPeekString	str;
		for ( size_t i = 0; i < nCount; i++ ) {
			str.Append( GetAt( i ).Format() );
			str.Append( L"\r\n" );
		}
		return str;
	}
	CPeekString	GetCName( UInt32 inOffset ) const;
	CPeekString	GetQuestionName() const { return (!IsEmpty()) ? GetAt( 0 ).GetName() : CPeekString(); }
	void		GetResolved( CResolvedDomainList& outList ) const;
	static bool	IsCompressed( const UInt8* inData ) { return ((*inData & 0xC0) != 0); }
	int			Parse( UInt32 inLength, const tDnsHeader* inHeader );
	CPeekString	ParseCName( UInt32 inLength, const UInt8* inData, int inDepth );
	bool		ResolveDnsEntry( const CDnsEntry& inEntry, CResolvedDomainList& outResolvedList ) const;
	static int	SafeStrLen( UInt32 inLength, const UInt8* inData );
};
