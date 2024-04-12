// =============================================================================
//	IpAddress.h
//  interface to CIpAddress
// =============================================================================
//	Copyright (c) 2004 WildPackets, Inc.

#pragma once

class CIpAddress
{
protected:
	UInt32	m_IpAddress;

public:
	;			CIpAddress( const UInt32 nIpAddress = 0 );
	;			~CIpAddress();

	CIpAddress&	operator=( const tIpAddress& Other ){ m_IpAddress = Other.word; return *this; }
	bool		operator==( const CIpAddress& Other ) const { return (m_IpAddress == Other.m_IpAddress); }
				operator UInt32() const { return m_IpAddress; }

	bool		IsNull() const { return (m_IpAddress == 0); }
	bool		Compare( const tIpAddress& Other ) const { return (m_IpAddress == Other.word); }
	CStringA	FormatA() const;
};


class CIpAddressPort
{
protected:
	CIpAddress	m_IpAddress;
	UInt16		m_nPort;

public:
	;		CIpAddressPort();
	;		CIpAddressPort( CIpAddress inIpAddress, UInt16 inPort );

	inline bool		operator==( const CIpAddressPort& inOther ) const;

	UInt32		Hash() const { return m_IpAddress; }
	CIpAddress	GetAddress() const { return m_IpAddress; }
	UInt16		GetPort() const { return m_nPort; }

	void	SetIpAddress( CIpAddress inIpAddress ){ m_IpAddress = inIpAddress; }
	void	SetPort( UInt16 inPort ){ m_nPort = inPort; }
	bool	IsNull(){ return (m_IpAddress.IsNull() & (m_nPort == 0)); }
};


class CIpAddressPortPair
{
protected:
	CIpAddressPort		m_Src;
	CIpAddressPort		m_Dst;

public:
	;		CIpAddressPortPair(){}
	;		CIpAddressPortPair( CIpAddressPort inSrc, CIpAddressPort inDst );

	UInt32	Hash() const { return m_Src.Hash() ^ m_Dst.Hash(); }
	bool	operator==( const CIpAddressPortPair& inOther ) const;
	bool	IsInverse( const CIpAddressPortPair& inOther ) const;
	void	Invert( CIpAddressPortPair& inOther ) const { inOther.m_Src = m_Dst; inOther.m_Dst = m_Src; }

	CIpAddress	GetSrcAddress() const { return m_Src.GetAddress(); }
	UInt16		GetSrcPort() const { return m_Src.GetPort(); }
	CIpAddress	GetDstAddress() const { return m_Dst.GetAddress(); }
	UInt16		GetDstPort() const { return m_Dst.GetPort(); }

	void	SetSrcIpAddress( CIpAddress inIpAddress ){ m_Src.SetIpAddress( inIpAddress ); }
	void	SetSrcPort( UInt16 inPort ){ m_Src.SetPort( inPort ); }
	void	SetDstIpAddress( CIpAddress inIpAddress ){ m_Dst.SetIpAddress( inIpAddress ); }
	void	SetDstPort( UInt16 inPort ){ m_Dst.SetPort( inPort ); }
};

class CIpAddressPortPairTraits
	: public CElementTraits<CIpAddressPortPair>
{
public:
	static ULONG	Hash( const CIpAddressPortPair& inIpPortPair ) { return inIpPortPair.Hash(); }
	static bool		CompareElements( const CIpAddressPortPair& inItem1, const CIpAddressPortPair& inItem2 ) {
		return inItem1 == inItem2; }
};

