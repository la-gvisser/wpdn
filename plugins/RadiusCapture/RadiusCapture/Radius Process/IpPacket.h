// =============================================================================
//	IpPacket.h
//  interface to CIpPacket, CTcpPacket
// =============================================================================
//	Copyright (c) 2004 WildPackets, Inc.

#pragma once

class CIpPacket
{
protected:
	UInt8*	m_pPacket;

public:
	CIpPacket();
	~CIpPacket();
};


class CTcpPacket
	: public CIpPacket
{
protected:

public:
	CTcpPacket();
	~CTcpPacket();
};
