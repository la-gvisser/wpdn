// ============================================================================
//	ut_peekstring.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#include "StdAfx.h"
#include "PeekStrings.h"
#include "IpHeaders.h"

int
UT_IpAddress()
{
	int		nFailures( 0 );

	CIpAddress	ipAlpha( 0x01020304 );
	CPeekString	strAlpha = ipAlpha.Format();
	if ( strAlpha != L"1.2.3.4" ) nFailures++;
	strAlpha = ipAlpha.Format( true );
	if ( strAlpha != L"001.002.003.004" ) nFailures++;

	CPeekStringA	strAlphaA = ipAlpha.FormatA();
	if ( strAlphaA != "1.2.3.4" ) nFailures++;
	strAlphaA = ipAlpha.FormatA( true );
	if ( strAlphaA != "001.002.003.004" ) nFailures++;

	++ipAlpha;
	strAlpha = ipAlpha.Format();
	if ( strAlpha != L"1.2.3.5" ) nFailures++;

	ipAlpha = 0xFAFBFCFD;
	strAlpha = ipAlpha.Format();
	if ( strAlpha != L"250.251.252.253" ) nFailures++;
	strAlpha = ipAlpha.Format( true );
	if ( strAlpha != L"250.251.252.253" ) nFailures++;


	tIpV6Address	ipSixAddress;
	ipSixAddress.longs[0] = 0;
	ipSixAddress.longs[1] = 0;

	CIpAddress		ipNull( &ipSixAddress );
	CPeekString		strIpSix = ipNull.Format();
	if ( strIpSix != L"0::0" ) nFailures++;
	strIpSix = ipNull.Format( true );
	if ( strIpSix != L"0000:0000:0000:0000:0000:0000:0000:0000" ) nFailures++;
	CPeekStringA	strIpSixA = ipNull.FormatA();
	if ( strIpSixA != "0::0" ) nFailures++;
	strIpSixA = ipNull.FormatA( true );
	if ( strIpSixA != "0000:0000:0000:0000:0000:0000:0000:0000" ) nFailures++;

	ipSixAddress.bytes[0] = 0x20;
	ipSixAddress.bytes[1] = 0x01;
	ipSixAddress.bytes[2] = 0x0E;
	ipSixAddress.bytes[3] = 0x10;
	ipSixAddress.bytes[4] = 0x4C;
	ipSixAddress.bytes[5] = 0x00;
	ipSixAddress.bytes[6] = 0xEE;
	ipSixAddress.bytes[7] = 0x00;
	ipSixAddress.bytes[8] = 0x00;
	ipSixAddress.bytes[9] = 0x01;
	ipSixAddress.bytes[10] = 0x00;
	ipSixAddress.bytes[11] = 0x00;
	ipSixAddress.bytes[12] = 0x00;
	ipSixAddress.bytes[13] = 0x00;
	ipSixAddress.bytes[14] = 0x00;
	ipSixAddress.bytes[15] = 0x01;

	CIpAddress	ip6Alpha( &ipSixAddress );
	strIpSix = ip6Alpha.Format();
	if ( strIpSix != L"2001:e10:4c00:ee00:1::1" ) nFailures++;
	strIpSix = ip6Alpha.Format( true );
	if ( strIpSix != L"2001:0e10:4c00:ee00:0001:0000:0000:0001" ) nFailures++;
	strIpSixA = ip6Alpha.FormatA();
	if ( strIpSixA != "2001:e10:4c00:ee00:1::1" ) nFailures++;
	strIpSixA = ip6Alpha.FormatA( true );
	if ( strIpSixA != "2001:0e10:4c00:ee00:0001:0000:0000:0001" ) nFailures++;

	return nFailures;
}
