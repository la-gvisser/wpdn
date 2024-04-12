// ============================================================================
//	Packet.h
//		interface for CPacket class.
// ============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "MemUtil.h"
#include "WPTypes.h"
#include "CaptureFile.h"

#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using std::string;
using std::wstring;

using namespace MemUtil;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Packet
//
//	An Ethernet Packet.
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#include "wppushpack.h"

static const size_t		s_nEthernetBytes( 6 );
static const size_t		s_nIPv4Bytes( 4 );
static const size_t		s_nIPv6Bytes( 16 );

typedef std::array<UInt8, s_nEthernetBytes>	tEthernet;
typedef std::array<UInt8, s_nIPv4Bytes>		tIPv4;
typedef std::array<UInt8, s_nIPv6Bytes>		tIPv6;

#define ETH_TYPE_IPV4				0x0800	// IPv4 Protocol
#define ETH_TYPE_IPV6				0x86DD	// IPv6 Protocol
#define ETH_TYPE_ARP 				0x0806	// Arp Protocol
#define ETH_TYPE_RARP				0x8035	// Reverse Arp Protocol
#define ETH_TYPE_VLAN				0x8100	// IEEE 802.1Q VLAN tagging
#define ETH_TYPE_QINQ				0x88A8	// IEEE 802.1ad QinQ tagging
#define ETH_TYPE_PPPOE_DISCOVERY	0x8863	// PPPoE Discovery Stage
#define ETH_TYPE_PPPOE_SESSION		0x8864	// PPPoE Session Stage
#define ETH_TYPE_ETAG				0x893F	// IEEE 802.1BR E-Tag
#define ETH_TYPE_1588				0x88F7	// IEEE 802.1AS 1588 Precise Time Protocol
#define ETH_TYPE_SLOW				0x8809	// Slow protocols (LACP and Marker)
#define ETH_TYPE_TEB				0x6558	// Transparent Ethernet Bridging
#define ETH_TYPE_LLDP				0x88CC	// LLDP Protocol
#define ETH_TYPE_MPLS				0x8847	// MPLS ethertype
#define ETH_TYPE_MPLSM				0x8848	// MPLS multicast ethertype

#define IP_TYPE_HOPOPT              0x0000  // IPv6 Hop-by-Hop Option
#define IP_TYPE_ICMP                0x0001  // Internet Control Message Protocol
#define IP_TYPE_IGMP                0x0002  // Internet Group Management Protocol
#define IP_TYPE_GGP                 0x0003  // Gateway-to-Gateway Protocol
#define IP_TYPE_IP_IN_IP            0x0004  // IP in IP (encapsulation)
#define IP_TYPE_ST                  0x0005  // Internet Stream Protocol
#define IP_TYPE_TCP                 0x0006  // Transmission Control Protocol
#define IP_TYPE_CBT                 0x0007  // Core-based trees
#define IP_TYPE_EGP                 0x0008  // Exterior Gateway Protocol
#define IP_TYPE_IGP                 0x0009  // Interior Gateway Protocol
#define IP_TYPE_BBN_RCC_MON         0x000A  // BBN RCC Monitoring
#define IP_TYPE_NVP_II              0x000B  // Network Voice Protocol
#define IP_TYPE_PUP                 0x000C  // Xerox PUP
#define IP_TYPE_ARGUS               0x000D  // ARGUS
#define IP_TYPE_EMCON               0x000E  // EMCON
#define IP_TYPE_XNET                0x000F  // Cross Net Debugger
#define IP_TYPE_CHAOS               0x0010  // Chaos
#define IP_TYPE_UDP                 0x0011  // User Datagram Protocol
#define IP_TYPE_MUX                 0x0012  // Multiplexing
#define IP_TYPE_DCN_MEAS            0x0013  // DCN Measurement Subsystems
#define IP_TYPE_HMP                 0x0014  // Host Monitoring Protocol
#define IP_TYPE_PRM                 0x0015  // Packet Radio Measurement
#define IP_TYPE_XNS_IDP             0x0016  // XEROX NS IDP
#define IP_TYPE_TRUNK_1             0x0017  // Trunk-1
#define IP_TYPE_TRUNK_2             0x0018  // Trunk-2
#define IP_TYPE_LEAF_1              0x0019  // Leaf-1
#define IP_TYPE_LEAF_2              0x001A  // Leaf-2
#define IP_TYPE_RDP                 0x001B  // Reliable Data Protocol
#define IP_TYPE_IRTP                0x001C  // Internet Reliable Transaction Protocol
#define IP_TYPE_ISO_TP4             0x001D  // ISO Transport Protocol Class 4
#define IP_TYPE_NETBLT              0x001E  // Bulk Data Transfer Protocol
#define IP_TYPE_MFE_NSP             0x001F  // MFE Network Services Protocol
#define IP_TYPE_MERIT_INP           0x0020  // MERIT Internodal Protocol
#define IP_TYPE_DCCP                0x0021  // Datagram Congestion Control Protocol
#define IP_TYPE_3PC                 0x0022  // Third Party Connect Protocol
#define IP_TYPE_IDPR                0x0023  // Inter-Domain Policy Routing Protocol
#define IP_TYPE_XTP                 0x0024  // Xpress Transport Protocol
#define IP_TYPE_DDP                 0x0025  // Datagram Delivery Protocol
#define IP_TYPE_IDPR_CMTP           0x0026  // IDPR Control Message Transport Protocol
#define IP_TYPE_TP_PLUS_PLUS        0x0027  // TP++ Transport Protocol
#define IP_TYPE_IL                  0x0028  // IL Transport Protocol
#define IP_TYPE_IPv6                0x0029  // IPv6 Encapsulation (6to4 and 6in4)
#define IP_TYPE_SDRP                0x002A  // Source Demand Routing Protocol
#define IP_TYPE_IPv6_Route          0x002B  // Routing Header for IPv6
#define IP_TYPE_IPv6_Frag           0x002C  // Fragment Header for IPv6
#define IP_TYPE_IDRP                0x002D  // Inter-Domain Routing Protocol
#define IP_TYPE_RSVP                0x002E  // Resource Reservation Protocol
#define IP_TYPE_GRE                 0x002F  // Generic Routing Encapsulation
#define IP_TYPE_DSR                 0x0030  // Dynamic Source Routing Protocol
#define IP_TYPE_BNA                 0x0031  // Burroughs Network Architecture
#define IP_TYPE_ESP                 0x0032  // Encapsulating Security Payload
#define IP_TYPE_AH                  0x0033  // Authentication Header
#define IP_TYPE_I_NLSP              0x0034  // Integrated Net Layer Security Protocol
#define IP_TYPE_SwIPe               0x0035  // SwIPe
#define IP_TYPE_NARP                0x0036  // NBMA Address Resolution Protocol
#define IP_TYPE_MOBILE              0x0037  // IP  Mobility (Min Encap)
#define IP_TYPE_TLSP                0x0038  // Transport Layer Security Protocol (using Kryptonet key management)
#define IP_TYPE_SKIP                0x0039  // Simple Key-Management for Internet Protocol
#define IP_TYPE_IPv6_ICMP           0x003A  // ICMP for IPv6
#define IP_TYPE_IPv6_NoNxt          0x003B  // No Next Header for IPv6
#define IP_TYPE_IPv6_Opts           0x003C  // Destination Options for IPv6
#define IP_TYPE_INTERNAL_PROTOCOL   0x003D  // Any host internal protocol
#define IP_TYPE_CFTP                0x003E  // CFTP
#define IP_TYPE_LOCAL_NETWORK       0x003F  // Any local network
#define IP_TYPE_SAT_EXPAK           0x0040  // SATNET and Backroom EXPAK
#define IP_TYPE_KRYPTOLAN           0x0041  // Kryptolan
#define IP_TYPE_RVD                 0x0042  // MIT Remote Virtual Disk Protocol
#define IP_TYPE_IPPC                0x0043  // Internet Pluribus Packet Core
#define IP_TYPE_DIST_FILE_SYSTEM    0x0044  // Any distributed file system
#define IP_TYPE_SAT_MON             0x0045  // SATNET Monitoring
#define IP_TYPE_VISA                0x0046  // VISA Protocol
#define IP_TYPE_IPCU                0x0047  // Internet Packet Core Utility
#define IP_TYPE_CPNX                0x0048  // Computer Protocol Network Executive
#define IP_TYPE_CPHB                0x0049  // Computer Protocol Heart Beat
#define IP_TYPE_WSN                 0x004A  // Wang Span Network
#define IP_TYPE_PVP                 0x004B  // Packet Video Protocol
#define IP_TYPE_BR_SAT_MON          0x004C  // Backroom SATNET Monitoring
#define IP_TYPE_SUN_ND              0x004D  // SUN ND PROTOCOL-Temporary
#define IP_TYPE_WB_MON              0x004E  // WIDEBAND Monitoring
#define IP_TYPE_WB_EXPAK            0x004F  // WIDEBAND EXPAK
#define IP_TYPE_ISO_IP              0x0050  // International Organization for Standardization Internet Protocol
#define IP_TYPE_VMTP                0x0051  // Versatile Message Transaction Protocol
#define IP_TYPE_SECURE_VMTP         0x0052  // Secure Versatile Message Transaction Protocol
#define IP_TYPE_VINES               0x0053  // VINES
#define IP_TYPE_TTP                 0x0054  // TTP
#define IP_TYPE_IPTM                0x0054  // Internet Protocol Traffic Manager
#define IP_TYPE_NSFNET_IGP          0x0055  // NSFNET-IGP
#define IP_TYPE_DGP                 0x0056  // Dissimilar Gateway Protocol
#define IP_TYPE_TCF                 0x0057  // TCF
#define IP_TYPE_EIGRP               0x0058  // EIGRP
#define IP_TYPE_OSPF                0x0059  // Open Shortest Path First
#define IP_TYPE_SPRITE_RPC          0x005A  // Sprite RPC Protocol
#define IP_TYPE_LARP                0x005B  // Locus Address Resolution Protocol
#define IP_TYPE_MTP                 0x005C  // Multicast Transport Protocol
#define IP_TYPE_AX_25               0x005D  // AX.25
#define IP_TYPE_OS                  0x005E  // KA9Q NOS compatible IP over IP tunneling
#define IP_TYPE_MICP                0x005F  // Mobile Internetworking Control Protocol
#define IP_TYPE_SCC_SP              0x0060  // Semaphore Communications Sec. Pro
#define IP_TYPE_ETHERIP             0x0061  // Ethernet-within-IP Encapsulation
#define IP_TYPE_ENCAP               0x0062  // Encapsulation Header
#define IP_TYPE_ENCRYPTION          0x0063  // Any private encryption scheme
#define IP_TYPE_GMTP                0x0064  // GMTP
#define IP_TYPE_IFMP                0x0065  // Ipsilon Flow Management Protocol
#define IP_TYPE_PNNI                0x0066  // PNNI over IP
#define IP_TYPE_PIM                 0x0067  // Protocol Independent Multicast
#define IP_TYPE_ARIS                0x0068  // IBM's ARIS (Aggregate Route IP Switching) Protocol
#define IP_TYPE_SCPS                0x0069  // Space Communications Protocol Standards
#define IP_TYPE_QNX                 0x006A  // QNX
#define IP_TYPE_A_N                 0x006B  // Active Networks
#define IP_TYPE_IPComp              0x006C  // IP Payload Compression Protocol
#define IP_TYPE_SNP                 0x006D  // Sitara Networks Protocol
#define IP_TYPE_COMPAQ_PEER         0x006E  // Compaq Peer Protocol
#define IP_TYPE_IPX_IN_IP           0x006F  // IPX in IP
#define IP_TYPE_VRRP                0x0070  // Virtual Router Redundancy Protocol, Common Address Redundancy Protocol
#define IP_TYPE_PGM                 0x0071  // PGM Reliable Transport Protocol
#define IP_TYPE_ZERO_HOP            0x0072  // 0-hop protocol
#define IP_TYPE_L2TP                0x0073  // Layer Two Tunneling Protocol Version 3
#define IP_TYPE_DDX                 0x0074  // D-II Data Exchange (DDX)
#define IP_TYPE_IATP                0x0075  // Interactive Agent Transfer Protocol
#define IP_TYPE_STP                 0x0076  // Schedule Transfer Protocol
#define IP_TYPE_SRP                 0x0077  // SpectraLink Radio Protocol
#define IP_TYPE_UTI                 0x0078  // Universal Transport Interface Protocol
#define IP_TYPE_SMP                 0x0079  // Simple Message Protocol
#define IP_TYPE_SM                  0x007A  // Simple Multicast Protocol     draft-perlman-simple-multicast-03
#define IP_TYPE_PTP                 0x007B  // Performance Transparency Protocol
#define IP_TYPE_IS_IS_OVER_IPV4     0x007C  // Intermediate System to Intermediate System (IS-IS) Protocol over IPv4
#define IP_TYPE_FIRE                0x007D  // Flexible Intra-AS Routing Environment
#define IP_TYPE_CRTP                0x007E  // Combat Radio Transport Protocol
#define IP_TYPE_CRUDP               0x007F  // Combat Radio User Datagram
#define IP_TYPE_SSCOPMCE            0x0080  // Service-Specific Connection-Oriented Protocol in a Multilink and Connectionless Environment
#define IP_TYPE_IPLT                0x0081  // IPLT
#define IP_TYPE_SPS                 0x0082  // Secure Packet Shield
#define IP_TYPE_PIPE                0x0083  // Private IP Encapsulation within IP (Expired)
#define IP_TYPE_SCTP                0x0084  // Stream Control Transmission Protocol
#define IP_TYPE_FC                  0x0085  // Fibre Channel
#define IP_TYPE_RSVP_E2E_IGNORE     0x0086  // Reservation Protocol (RSVP) End-to-End Ignore
#define IP_TYPE_MOBILITY_HEADER     0x0087  // Mobility Extension Header for IPv6
#define IP_TYPE_UDPLITE             0x0088  // Lightweight User Datagram Protocol
#define IP_TYPE_MPLS_IN_IP          0x0089  //  Multiprotocol Label Switching Encapsulated in IP
#define IP_TYPE_MANET               0x008A  // MANET Protocols
#define IP_TYPE_HIP                 0x008B  // Host Identity Protocol
#define IP_TYPE_SHIM6               0x008C  // Site Multihoming by IPv6 Intermediation
#define IP_TYPE_WESP                0x008D  // Wrapped Encapsulating Security Payload
#define IP_TYPE_ROHC                0x008E  // Robust Header Compression
#define IP_TYPE_ETHERNET            0x008F  // IPv6 Segment Routing (TEMPORARY)
#define IP_TYPE_AGGFRAG             0x0090  // AGGFRAG Encapsulation Payload for ESP
#define IP_TYPE_UNASSIGNED_BEGIN    0x0091  // Unassigned, begining of range
#define IP_TYPE_UNASSIGNED_END      0x00FC  // Unassigned, end of range
#define IP_TYPE_EXPERIMENTAL_1      0x00FD  // Use for experimentation and testing
#define IP_TYPE_EXPERIMENTAL_2      0x00FE  // Use for experimentation and testing
#define IP_TYPE_RESERVED            0x00FF  // Reserved


class CByteVectorStream
{
protected:
	size_t			m_nLength;
	size_t			m_nOffset;
	const UInt8*	m_pData;

public:
	;	CByteVectorStream( const CByteVector& inData )
		:	m_nLength( inData.size() )
		,	m_nOffset( 0 )
		,	m_pData( inData.data() )
	{}
	;	CByteVectorStream( size_t inLength, const UInt8* inData )
		:	m_nLength( inLength )
		,	m_nOffset( 0 )
		,	m_pData( inData )
	{}

	const UInt8*	Data() const { return m_pData; }
	size_t			Length() const { return m_nLength; }
	size_t			Offset() const { return m_nOffset; }

	wstring	Format() const;

	const UInt8*	GetData( size_t inLength ) {
		if ( Length() < inLength ) {
			throw std::out_of_range( "Not enough data." );
		}
		const UInt8*	pData( Data() );
		if ( pData == nullptr ) {
			throw std::out_of_range( "Not enough data." );
		}
		Seek( inLength );
		return pData;
	}


	bool	IsEOD() const { return m_nLength == 0; }

	UInt8	ReadUInt8() {
		if ( IsEOD() ) {
			throw std::out_of_range( "No more data." );
    	}
		m_nOffset++;
		return *m_pData++;
	}
	UInt16	ReadUInt16() {
		if ( m_nLength < 2 ) {
			throw std::out_of_range( "Not enough data." );
    	}
		UInt16	nValue( *( reinterpret_cast<const UInt16*>( m_pData ) ) );
		m_nLength -= 2;
		m_nOffset += 2;
		m_pData += 2;
		return nValue;
	}
	UInt32	ReadUInt32() {
		if ( m_nLength < 4 ) {
			throw std::out_of_range( "Not enough data." );
    	}
		UInt32	nValue( *( reinterpret_cast<const UInt32*>( m_pData ) ) );
		m_nLength -= 4;
		m_nOffset += 4;
		m_pData += 4;
		return nValue;
	}
	UInt64	ReadUInt64() {
		if ( m_nLength < 8 ) {
			throw std::out_of_range( "Not enough data." );
    	}
		UInt64	nValue( *( reinterpret_cast<const UInt64*>( m_pData ) ) );
		m_nLength -= 8;
		m_nOffset += 8;
		m_pData += 8;
		return nValue;
	}
	void	Reset() {
		m_nLength = 0;
		m_nOffset = 0;
		m_pData = nullptr;
	}

	void	Seek( size_t inLength ) {
		if ( inLength <= m_nLength ) {
			m_nLength -= inLength;
			m_nOffset += inLength;
			m_pData += inLength;
		}
		else {
			Reset();
		}
	}
};


class CEthernetAddress
{
protected:
	tEthernet	m_bytes;

public:
	;	CEthernetAddress()
		:	m_bytes{}
	{}

	// operator const UInt8*() const { return m_bytes; }
	// operator const UInt16*() const { return reinterpret_cast<const UInt16*>( m_bytes ); }

	wstring	Format() const;
	size_t	Length() const { return s_nEthernetBytes; }
	size_t	Load( CByteVectorStream& inData ) {
		const UInt8*	pData( inData.GetData( Length() ) );
		m_bytes = tEthernet( { pData[0], pData[1], pData[2], pData[3], pData[4], pData[5] } );
		return Length();
	}
} WP_PACK_STRUCT;


class CIPAddress
{
public:
	virtual ~CIPAddress() {}

	virtual operator const UInt8*() const = 0;

	virtual wstring	Format() const = 0;
	virtual size_t	Length() const = 0;
} WP_PACK_STRUCT;


class CIPv4Address
	:   public CIPAddress
{
protected:
	tIPv4	m_bytes;

public:
	;	CIPv4Address()
		:	m_bytes{}
	{}
	virtual ~CIPv4Address() {}

	virtual operator const UInt8*() const { return m_bytes.data(); }

	virtual wstring	Format() const;
	virtual size_t	Length() const { return s_nIPv4Bytes; }
	size_t			Load( CByteVectorStream& inData ) {
		const UInt8*	pData( inData.GetData( Length() ) );
		m_bytes = tIPv4( { pData[3], pData[2], pData[1], pData[0] } );
		return Length();
	}
} WP_PACK_STRUCT;


class CIPv6Address
	:	public CIPAddress
{
protected:
	tIPv6	m_bytes;

public:
	;	CIPv6Address()
		:	m_bytes{}
	{}
	virtual ~CIPv6Address() {}

	virtual operator const UInt8*() const { return m_bytes.data(); }

	virtual wstring	Format() const;
	virtual size_t	Length() const { return s_nIPv6Bytes; }
	size_t			Load( CByteVectorStream& inData ) {
		const UInt8*	pData( inData.GetData( Length() ) );
		m_bytes = tIPv6( {
			pData[0],  pData[1],   pData[2],  pData[3],  pData[4],  pData[5],  pData[6],  pData[7],
			pData[8],  pData[9],  pData[10], pData[11], pData[12], pData[13], pData[14], pData[15]
		} );
		return Length();
	}
} WP_PACK_STRUCT;


class CEthernetHeader {
public:
	static const size_t	s_Length = (sizeof( UInt16 ) + (2 * s_nIPv6Bytes) );

public:
	CEthernetAddress	m_Destination;
	CEthernetAddress	m_Source;
	UInt16				m_nProtocolType;

	;	CEthernetHeader()
		:	m_Destination{}
		,	m_Source{}
		,	m_nProtocolType{}
	{}

	size_t	Length() const {
		return s_Length;
	}
	size_t	Load( CByteVectorStream& inData ) {
		if ( inData.Length() < Length() ) {
			throw std::out_of_range( "Not enough data." );
		}
		m_Destination.Load( inData );
		m_Source.Load( inData );
		m_nProtocolType = NETWORKTOHOST16( inData.ReadUInt16() );
		return Length();
	}
} WP_PACK_STRUCT;


class C80211Header {
public:
	static const size_t	s_nLength = ( (3 * sizeof( UInt16 )) + (3 * s_nEthernetBytes ) );

public:
	UInt16				m_nVersionTypeSubType;
	UInt16				m_nDuration;
	CEthernetAddress	m_eaBSSID;
	CEthernetAddress	m_eaSource;
	CEthernetAddress	m_eaDestination;
	UInt16				m_nProtocolType;

	;	C80211Header()
		:	m_nVersionTypeSubType{}
		,	m_nDuration{}
		,	m_eaBSSID{}
		,	m_eaSource{}
		,	m_eaDestination{}
		,	m_nProtocolType{}
	{}

	size_t	Length() const { return s_nLength; }
} WP_PACK_STRUCT;


// Pure virtual class, gets the unusual name (instead of CIPHeader).
class CIPHeader
{
public:
	virtual const CIPAddress&	Destination() const = 0;

	virtual size_t				Length() const = 0;
	virtual size_t				Load( CByteVectorStream& inData ) = 0;

	virtual UInt16				Protocol() const = 0;

	virtual const CIPAddress&	Source() const = 0;
};


class CIPv4Header
	:	public CIPHeader
{
public:
	static const size_t	s_nLength = ( (7 * sizeof( UInt8 )) + (3 * sizeof( UInt16 )) +
						(2 * s_nIPv4Bytes ) );

public:
	UInt8			m_nVersion;
	UInt8			m_nLength;
	UInt8			m_nTypeOfService;
	UInt16			m_nTotalLength;
	UInt16			m_nIdentifier;
	UInt8			m_nFlags;
	UInt8			m_nFragmentOffset;
	UInt8			m_nTimeToLive;
	UInt8			m_nProtocol;
	UInt16			m_nHeaderChecksum;
	CIPv4Address	m_ipaSource;
	CIPv4Address	m_ipaDestination;

public:
	;	CIPv4Header()
		:	m_nVersion{}
		,	m_nLength{}
		,	m_nTypeOfService{}
		,	m_nTotalLength{}
		,	m_nIdentifier{}
		,	m_nFlags{}
		,	m_nFragmentOffset{}
		,	m_nTimeToLive{}
		,	m_nProtocol{}
		,	m_nHeaderChecksum{}
		,	m_ipaSource{}
		,	m_ipaDestination{}
	{}
	virtual ~CIPv4Header() {}

	virtual const CIPAddress&	Destination() const { return m_ipaDestination; }

	virtual size_t				Load( CByteVectorStream& inData );
	virtual size_t				Length() const { return s_nLength;}

	virtual const CIPAddress&	Source() const { return m_ipaSource; }

	virtual UInt16				Protocol() const { return m_nProtocol; }
};

class CIPv6Header
	:	public CIPHeader
{
public:
	static const size_t	s_nLength = ( (2 * sizeof( UInt8 )) + sizeof( UInt16 ) +
						(2 * sizeof( UInt32 )) + (2 * s_nIPv6Bytes) );

public:
	UInt32			m_nVersionClassLabel;
	UInt16			m_nPayloadLength;
	UInt8			m_nNextHeader;
	UInt8			m_nHopLimit;
	CIPv6Address	m_ipaSource;
	CIPv6Address	m_ipaDestination;

public:
	;		CIPv6Header()
			:	m_nVersionClassLabel{}
			,	m_nPayloadLength{}
			,	m_nNextHeader{}
			,	m_nHopLimit{}
			,	m_ipaSource{}
			,	m_ipaDestination{}
	{}

	virtual const CIPAddress&	Destination() const { return m_ipaDestination; }

	virtual size_t				Length() const { return s_nLength; }
	virtual size_t				Load( CByteVectorStream& inData );

	virtual UInt16				Protocol() const { return -1; }

	virtual const CIPAddress&	Source() const { return m_ipaSource; }
};


class CTCPHeader
{
	static const size_t	s_nLength = ( (2 * sizeof( UInt8 )) + (5 * sizeof( UInt16 )) +
						(2 * sizeof( UInt32 )) );

public:
	UInt16	m_nSourcePort;
	UInt16	m_nDestinationPort;
	UInt32	m_nSequenceNumber;
	UInt32	m_nAckNumber;
	UInt8	m_nOffset;
	UInt8	m_nFlags;
	UInt16	m_nWindowSize;
	UInt16	m_nChecksum;
	UInt16	m_nUrgentPointer;

public:
	;		CTCPHeader() {}
	virtual	~CTCPHeader() {}

	virtual size_t	Length() const {
		return s_nLength;
	}
	virtual size_t	Load( CByteVectorStream& inData ) {
		if ( inData.Length() < Length() ) {
			throw std::out_of_range( "Not enough data." );
		}
		m_nSourcePort = inData.ReadUInt16();
		m_nDestinationPort = inData.ReadUInt16();
		m_nSequenceNumber = inData.ReadUInt32();
		m_nAckNumber = inData.ReadUInt32();
		m_nOffset = inData.ReadUInt8();
		m_nFlags = inData.ReadUInt8();
		m_nWindowSize = inData.ReadUInt16();
		m_nChecksum = inData.ReadUInt16();
		m_nUrgentPointer = inData.ReadUInt16();
		return Length();
	}
};

class CUDPHeader
{
	static const size_t	s_nLength = ( 4 * sizeof( UInt16 ) );

public:
	UInt16	m_nSourcePort;
	UInt16	m_nDestinationPort;
	UInt16	m_nLength;
	UInt16	m_nChecksum;

public:
	;		CUDPHeader() {}
	virtual	~CUDPHeader() {}

	virtual size_t	Length() const {
		return s_nLength;
	}
	virtual size_t	Load( CByteVectorStream& inData ) {
		if ( inData.Length() < Length() ) {
			throw std::out_of_range( "Not enough data." );
		}
		m_nSourcePort = inData.ReadUInt16();
		m_nDestinationPort = inData.ReadUInt16();
		m_nLength = inData.ReadUInt16();
		m_nChecksum = inData.ReadUInt16();
		return Length();
	}
} WP_PACK_STRUCT;

struct tDNSHeader {
	unsigned short	id;			// query identification number

	unsigned short	rd:		1;	// recursion desired
	unsigned short	tc:		1;	// truncated message
	unsigned short	aa:		1;	// authoritative answer
	unsigned short	opcode:	4;	// purpose of message
	unsigned short	qr:		1;	// response flag

	unsigned short	rcode:	4;	// response code
	unsigned short	unused:	1;	// Z - unused bits
	unsigned short	ad:		1;	// authentic data from named
	unsigned short	cd:		1;	// checking disabled by resolver
	unsigned short	ra:		1;	// recursion available

	unsigned short	qdcount;	// number of question entries
	unsigned short	ancount;	// number of answer entries
	unsigned short	nscount;	// number of authority entries
	unsigned short	arcount;	// number of resource entries
};

#include "wppoppack.h"


// ============================================================================
//		CPacket
// ============================================================================

class CPacket
{
protected:
	CEthernetHeader				m_Ethernet;
	std::unique_ptr<CIPHeader>	m_spIP;
	// union {
	// 	CTCPHeader	m_TCP;
	// 	CUDPHeader	m_UDP;
	// };

public:
	// ;	CPacket() {}
	virtual ~CPacket() {}

	const CEthernetHeader&	GetEthernetHeader() const { return m_Ethernet; }
	const CIPHeader*		GetIPHeader() const { return m_spIP.get(); }
	// const CIPv4Header*		GetIPHeader() const { return m_spIP.get(); }

	bool	ParsePacketData( const CByteVector& inData );

	size_t	SetEthernet( const CEthernetHeader& inEthernet ) {
		m_Ethernet = inEthernet;
		return sizeof( CEthernetHeader );
	}
	// size_t	SetIP( const CIPHeader& inIP ) {
	// 	if ( m_Ethernet.m_ProtocolType == 0 ) return 0;

	// 	m_IP = inIP;
	// 	return sizeof( CIPHeader );
	// }
	// size_t	SetTCPHeader( const CTCPHeader& inTCP ) {
	// 	m_TCP = inTCP;
	// 	return sizeof( CTCPHeader );
	// }
	// size_t	SetUDPHeader( const CUDPHeader& inUDP ) {
	// 	m_UDP = inUDP;
	// 	return sizeof( CUDPHeader );
	// }
};

} // namespace CaptureFile
