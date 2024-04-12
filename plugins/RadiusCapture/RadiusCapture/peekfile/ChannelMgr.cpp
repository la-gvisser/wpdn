// =============================================================================
//	ChannelMgr.cpp
// =============================================================================
//	Copyright (c) Wildpackets, Inc. 2004-2005. All rights reserved.

#include "stdafx.h"
#include "ChannelMgr.h"
#include <vector>
#include <ctype.h>
#include <wctype.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------
//	constructor
// -----------------------------------------------------------------------------

CChannelManager::CChannelManager( ChannelType inType ) :
	m_payChannelData( NULL ),
	m_type( kMaxType )
{
	SetChannelType(inType);
}

// -----------------------------------------------------------------------------
//	destructor
// -----------------------------------------------------------------------------

CChannelManager::~CChannelManager()
{
	delete m_payChannelData;
}

// -----------------------------------------------------------------------------
//	operator=
// -----------------------------------------------------------------------------

CChannelManager& 
CChannelManager::operator=(const CChannelManager& other)
{
	SetChannelType(other.m_type);
	if (other.m_type != kMaxType && other.m_payChannelData != NULL)
	{
		SetChannels(other.m_payChannelData->GetData(), other.m_payChannelData->GetCount());
	}

	return *this;
}

// -----------------------------------------------------------------------------
//	SetChannelType
// -----------------------------------------------------------------------------

void 
CChannelManager::SetChannelType( ChannelType type )
{
	if (m_type != type)
	{	// reset the channel info
		delete m_payChannelData;
		m_payChannelData = NULL;
		m_type = type;
		// based on the type, set the item size
		SInt32 itemSize = 0;
		CComparator* pComparator = NULL;

		switch (m_type)
		{
			case kWirelessChannels:
			{
				itemSize = sizeof(SWirelessChannel);
				pComparator = CWirelessChannelComparator::GetComparator()->Clone();
			}
			break;

			case kEnumChannels:
			{
				itemSize = sizeof(ULONG);
				pComparator = CLongComparator::GetComparator()->Clone();
			}
			break;

			default:
			{
			}
			break;
		}

		if (itemSize && pComparator)
		{
			m_payChannelData = new CArrayEx(itemSize, pComparator);
			m_payChannelData->SetKeepSorted( true );
		}
	}
}

// -----------------------------------------------------------------------------
//	GetChannels
// -----------------------------------------------------------------------------

bool
CChannelManager::GetChannels(void** pData, SInt32 *pnCount)
{
	if (m_type != kMaxType && m_payChannelData)
	{
		*pData = m_payChannelData->GetData();
		*pnCount = m_payChannelData->GetCount();
		return true;
	}
	
	*pData = NULL;
	*pnCount = 0;

	return false;
}

// -----------------------------------------------------------------------------
//	GetChannels
// -----------------------------------------------------------------------------

bool
CChannelManager::GetChannels(const void** pData, SInt32 *pnCount) const
{
	if (m_type != kMaxType && m_payChannelData)
	{
		*pData = m_payChannelData->GetData();
		*pnCount = m_payChannelData->GetCount();
		return true;
	}

	*pData = NULL;
	*pnCount = 0;

	return false;
}

// -----------------------------------------------------------------------------
//	SetChannels
// -----------------------------------------------------------------------------

bool 
CChannelManager::SetChannels(const void* pData, SInt32 nCount)
{
	if (m_payChannelData == NULL || m_type == kMaxType)
		return false;

	m_payChannelData->SetCount(nCount);
	memcpy(m_payChannelData->GetData(), pData, nCount*m_payChannelData->GetItemSize(0));
	return true;
}

// -----------------------------------------------------------------------------
//	AddChannel
// -----------------------------------------------------------------------------

SInt32 
CChannelManager::AddChannel(const void* pData)
{
	if (m_payChannelData == NULL || m_type == kMaxType)
		return CArrayEx::kArrayIndex_Invalid;

	SInt32 nIndex = m_payChannelData->GetIndexOf(pData);
	if (nIndex < 0)
		nIndex = m_payChannelData->AddItem( pData );

	return nIndex;
}

// -----------------------------------------------------------------------------
//	GetCount
// -----------------------------------------------------------------------------

SInt32 
CChannelManager::GetCount() const
{
	return (m_payChannelData == NULL) ? 0 : m_payChannelData->GetCount();
}


// -----------------------------------------------------------------------------
//	[static] IsValidChannel
// -----------------------------------------------------------------------------

bool
CChannelManager::IsValidChannel(
	UInt32		nBand,
	SInt16		nChannel,
	SInt16 *	pOutMin,	// [default = NULL]
	SInt16 *	pOutMax )	// [default = NULL]
{

	// Setup defaults.
	if ( pOutMin != NULL ) *pOutMin = 0;
	if ( pOutMax != NULL ) *pOutMax = 0;
	TArrayEx<SWirelessChannel> channels;
	UInt32 nCount = GetDefault80211Channels( nBand, kMediaDomain_Null, false, channels );

	// Initialize values.
	bool bValid = false;
	for ( UInt32 i = 0; i < nCount; i++ )
	{
		if ( channels[i].Channel == nChannel )
		{
			bValid = true;
		}
		
		if ( ( pOutMin != NULL ) && ( ( *pOutMin == 0 ) || 
			 ( ( *pOutMin > channels[i].Channel ) && ( channels[i].Channel > 0 ) ) ) )
		{
			*pOutMin = channels[i].Channel;
		}

		if ( ( pOutMax != NULL ) && 
			 ( ( *pOutMax < channels[i].Channel ) && ( channels[i].Channel > 0 ) ) )
		{
			*pOutMax = channels[i].Channel;
		}
	}
	return bValid;
}


// -----------------------------------------------------------------------------
//	[static] SetupWirelessChannel
// -----------------------------------------------------------------------------

SWirelessChannel 
CChannelManager::SetupWirelessChannel( 
	TMediaType		/*mt*/,
	TMediaSubType	mst, 
	int				nChannel )
{
	SWirelessChannel theChannel = {(SInt16)nChannel,0,0};

	// figure out the band and frequency
	if (!IS_WIRELESS(mst))
		return theChannel;

	// for AP v5.1.x and below, A and B channels were all exclusive
	// BG: 1 - 14
	// A: 30 - 165
	// A turbo: ???

	// no turbo flags, so assume that the channel is non-turbo
	// calculate the channel frequency with:
	// B: 	2407 + (channel*5) 
	//		1 : 2412 MHz;
	//		2 : 2417 MHz;
	//		3 : 2422 MHz; ...
	// A:   5000 + (channel*5)
	//		1 : 5005 MHz;	
	//		2 : 5010 MHz;
	//		3 : 5015 MHz; ...

	// TODO: Handle 4.9GHz broadband public safety band which uses some of the same channel numbers as 802.11bg.
	if (nChannel <= 14)
	{	// assume B/G band
		// TODO: Handle 802.11g channel 6 turbo!
		theChannel.Band = PEEK_802_11_BG_BAND;
		theChannel.Frequency = 2407 + nChannel*5;
	}
	else
	{
		theChannel.Band = PEEK_802_11_A_BAND;
		theChannel.Frequency = 5000 + nChannel*5;

		switch ( nChannel )
		{
		case 42:
		case 50:
		case 58:
		case 152:
		case 160:
			// These are all 802.11a turbo channels so change
			// the band accordingly.
			theChannel.Band = PEEK_802_11_A_TURBO_BAND;
			break;

		default:
			// Don't change the band for these channels.
			;
		}
	}

	return theChannel;
}

// -----------------------------------------------------------------------------
//	[static] SetupWirelessChannel
// -----------------------------------------------------------------------------

SWirelessChannel 
CChannelManager::SetupWirelessChannel( 
	UInt32		inBand, 
	int			nChannel, 
	bool		in40MHzNonCenterFreq ) // default = false
{
	SWirelessChannel theChannel = {(SInt16)nChannel,0,inBand};

	switch ( inBand )
	{

		case PEEK_802_11_A_BAND:
		case PEEK_802_11_A_TURBO_BAND:
		{
			theChannel.Frequency = 5000 + nChannel*5;
		}
		break;

		case PEEK_802_11_BAND:
		case PEEK_802_11_B_BAND:
		//case PEEK_802_11_BG_BAND:
		case PEEK_802_11_G_BAND:
		case PEEK_802_11_G_TURBO_BAND:
		case PEEK_802_11_SUPER_G_BAND:
		{
			theChannel.Frequency = 2407 + nChannel*5;
		}
		break;

		case PEEK_802_11_ALL_BANDS:
		case PEEK_802_11_N_20MHZ_BAND:
		{
			if ( nChannel <= 14 )
			{
				theChannel.Frequency = 2407 + nChannel*5;
			}
			else if ( ( nChannel >= 30 ) && ( nChannel <= 165 ) )
			{
				theChannel.Frequency = 5000 + nChannel*5;	
			}
		}
		break;

		case PEEK_802_11_N_BAND:
		case PEEK_802_11_N_40MHZ_BAND:
		{
			ASSERT( false ); // We can't and should not determine channel frequency from these bands!
		}
		break;

		case PEEK_802_11_N_40MHZ_HIGH_BAND:
		case PEEK_802_11_N_40MHZ_LOW_BAND:
		{
			bool bFound = false;
			std::vector<int> channels;
			int nSecChannel = 0;
			int nStartFreq = 0;
			
			if ( inBand == PEEK_802_11_N_40MHZ_HIGH_BAND )
			{
				if ( nChannel < 11 )
				{
					static const int tempChannels[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
					for ( size_t i = 0; i < sizeof(tempChannels)/sizeof(tempChannels[0]); i++)
					{
						channels.push_back( tempChannels[i] );
					}
					nStartFreq = 2407;
				}
				else if ( nChannel >= 36 )
				{
					static const int tempChannels[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 149, 157 };
					for ( size_t i = 0; i < sizeof(tempChannels)/sizeof(tempChannels[0]); i++)
					{
						channels.push_back( tempChannels[i] );
					}
					nStartFreq = 5000;
				}
				nSecChannel = 1;
			}
			else if ( inBand == PEEK_802_11_N_40MHZ_LOW_BAND )
			{
				if ( nChannel <= 13 )
				{
					static const int tempChannels[] = { 5, 6, 7, 8, 9, 10, 11, 12, 13 };
					for ( size_t i = 0; i < sizeof(tempChannels)/sizeof(tempChannels[0]); i++)
					{
						channels.push_back( tempChannels[i] );
					}
					nStartFreq = 2407;
				}
				else if ( nChannel > 36 )
				{
					static const int tempChannels[] = { 40, 48, 56, 64, 104, 112, 120, 128, 136, 153, 161 };
					for ( size_t i = 0; i < sizeof(tempChannels)/sizeof(tempChannels[0]); i++)
					{
						channels.push_back( tempChannels[i] );
					}
					nStartFreq = 5000;
				}
				nSecChannel = -1;
			}
			else
			{
				break;
			}
			
			for ( int i = 0; i < (int)channels.size(); i++ )
			{
				if ( nChannel == channels[ i ] )
				{
					if ( in40MHzNonCenterFreq )
					{
						theChannel.Frequency = nStartFreq + nChannel*5;
					}
					else
					{
						UInt32 nPrimaryChannel = nStartFreq + nChannel*5;
						UInt32 nSecondayChannel = nStartFreq + (nChannel + (nSecChannel*4))*5;
						if ( nSecondayChannel > nPrimaryChannel )
						{
							theChannel.Frequency = ( (nSecondayChannel - nPrimaryChannel) / 2 ) + nPrimaryChannel;
						}
						else
						{
							theChannel.Frequency = ( (nPrimaryChannel - nSecondayChannel) / 2 ) + nSecondayChannel;
						}
					}
					bFound = true;
					break;
				}
			}
		}
		break;
		
		//case PEEK_802_11_A_LICENSED_1MHZ_BAND:
		//case PEEK_802_11_A_LICENSED_15MHZ_BAND:
		case PEEK_802_11_A_LICENSED_5MHZ_BAND:		// starts at channel 5
		case PEEK_802_11_A_LICENSED_10MHZ_BAND:		// starts at channel 10
		case PEEK_802_11_A_LICENSED_20MHZ_BAND:		// starts at channel 20
		{
			theChannel.Frequency = 4940 + ( ((int) (theChannel.Channel / 10)) * 5 );
		}
		break;

		default:
		{
			// Please add the band channel detail for this unknown band.
			ASSERT( false );
		}
		break;
	}
	return theChannel;
}


// -----------------------------------------------------------------------------
//	[static] SetupWirelessChannel
// -----------------------------------------------------------------------------

SWirelessChannel 
CChannelManager::SetupWirelessChannel( const SWirelessChannel & channel,
									   bool						in40MHzNonCenterFreq ) // default = false
{
	return CChannelManager::SetupWirelessChannel( channel.Band, channel.Channel, in40MHzNonCenterFreq );
}


// -----------------------------------------------------------------------------
//	[static] FormatWirelessChannel
// -----------------------------------------------------------------------------

//void 
//CChannelManager::FormatWirelessChannel( 
//	LPTSTR				psz,
//	int					cchMax,
//	SWirelessChannel	channel )
//{
//	ASSERT( psz != NULL );
//	if ( psz == NULL ) return;
//
//	*psz = 0;
//
//	ASSERT( cchMax >= 64 );
//	if ( cchMax < 64 ) return;
//
//	// %d%s - %6.3f
//	LPCTSTR lpszBand = GetWirelessBandString( channel.Band, channel.Channel );
//	
//	if ( lpszBand != NULL )
//	{
//		if ( IS_49GHZ_BROADBAND_PUBLIC_SAFETY_BAND( channel.Band ) )
//		{
//			// TODO: The frequency really needs to be the true frequency so that this code doesn't have
//			//       to guess if the frequency has something after the decimal point. This will also
//			//       require a driver change.
//			if ( (channel.Channel % 10) == 0 )
//			{
//				_stprintf( psz, _T("%d - %u MHz (%s)"),
//					(int) channel.Channel, (unsigned int) channel.Frequency, lpszBand );
//			}
//			else
//			{
//				_stprintf( psz, _T("%d - %u.5 MHz (%s)"),
//					(int) channel.Channel, (unsigned int) channel.Frequency, lpszBand );
//			}
//		}
//		else
//		{
//			_stprintf( psz, _T("%d - %u MHz (%s)"),
//				(int) channel.Channel, (unsigned int) channel.Frequency, lpszBand );
//		}
//	}
//	else
//	{
//		_stprintf( psz, _T("%d - %u MHz"),
//			(int) channel.Channel, (unsigned int) channel.Frequency );
//	}
//}


// -----------------------------------------------------------------------------
//	[static] ScanfWirelessChannel
// -----------------------------------------------------------------------------

//SWirelessChannel
//CChannelManager::ScanfWirelessChannel( LPCTSTR psz )
//{
//	int		nChannel   = 0;
//	int		nFrequency = 0;
//	TCHAR	szBand[48];
//
//	szBand[0] = 0;
//	// TODO: Must handle the 4.9GHz broadband public safety stuff separately because the frequency
//	//       can be provided in a different format (some frequencies end with .5).
//
//	_stscanf( psz, _T("%d - %u(%3s)"), &nChannel, &nFrequency, szBand );
//
//	tstring			strBand( psz );
//	const size_t	nBandEnd = strBand.rfind( _T(")") );
//	if ( nBandEnd != tstring::npos )
//	{
//		const size_t	nBandBegin = strBand.rfind( _T("("), nBandEnd );
//		if ( nBandBegin != tstring::npos )
//		{
//			strBand	= strBand.substr( nBandBegin + 1, nBandEnd - nBandBegin - 1 );
//		}
//	}
//	
//	SWirelessChannel	result;
//	result.Channel		= (SInt16) nChannel;
//	result.Frequency	= (UInt32) nFrequency;
//	result.Band			= StringToBand( strBand.c_str() );
//	return result;
//}


// -----------------------------------------------------------------------------
//	[static] GetWirelessBandString
// -----------------------------------------------------------------------------

LPCTSTR 
CChannelManager::GetWirelessBandString( UInt32 inBand, UInt32 inChannel )
{
	switch (inBand)
	{
		case PEEK_802_11_ALL_BANDS:
			return _T("all");
		case PEEK_802_11_B_BAND:
			return _T("b");
		case PEEK_802_11_A_BAND:
			return _T("a");
//		case PEEK_802_11_G_BAND:
		case PEEK_802_11_BG_BAND:
			return _T("bg");
		case PEEK_802_11_N_BAND:
			return _T("n");
		case PEEK_802_11_N_20MHZ_BAND:
		{
			if ( ( inChannel >= 1 ) && ( inChannel <= 14 ) )
			{
				return _T("bgn");
			}
			else if ( ( inChannel >= 30 ) && ( inChannel <= 165 ) )
			{
				return _T("an");
			}
			else
			{
				return _T("n20");
			}
		}
		case PEEK_802_11_N_40MHZ_BAND:
			return _T("n40");
		case PEEK_802_11_N_40MHZ_LOW_BAND:
			return _T("n40l");
		case PEEK_802_11_N_40MHZ_HIGH_BAND:
			return _T("n40h");
		case PEEK_802_11_A_TURBO_BAND:
			return _T("at");
		case PEEK_802_11_G_TURBO_BAND:
			return _T("gt");
		case PEEK_802_11_SUPER_G_BAND:
			return _T("sg");
		case PEEK_802_11_A_LICENSED_1MHZ_BAND:
			return _T("BPS 1MHz ChBw");
		case PEEK_802_11_A_LICENSED_5MHZ_BAND:
			return _T("BPS 5MHz ChBw");
		case PEEK_802_11_A_LICENSED_10MHZ_BAND:
			return _T("BPS 10MHz ChBw");
		case PEEK_802_11_A_LICENSED_15MHZ_BAND:
			return _T("BPS 15MHz ChBw");
		case PEEK_802_11_A_LICENSED_20MHZ_BAND:
			return _T("BPS 20MHz ChBw");
		case PEEK_802_11_UNKNOWN_1_BAND: //   201                 // [u1]"Unknown 1"
			return _T("u1");
		case PEEK_802_11_UNKNOWN_2_BAND: //   202                 // [u2]"Unknown 2"
			return _T("u2");
		case PEEK_802_11_UNKNOWN_3_BAND: //   203                 // [u3]"Unknown 3"
			return _T("u3");
		case PEEK_802_11_UNKNOWN_4_BAND: //   204                 // [u4]"Unknown 4"
			return _T("u4");
		case PEEK_802_11_UNKNOWN_5_BAND: //   205                 // [u5]"Unknown 5"
			return _T("u5");
		case PEEK_802_11_UNKNOWN_6_BAND: //   206                 // [u6]"Unknown 6"
			return _T("u6");
		case PEEK_802_11_UNKNOWN_7_BAND: //   207                 // [u7]"Unknown 7"
			return _T("u7");
		case PEEK_802_11_UNKNOWN_8_BAND: //   208                 // [u8]"Unknown 8"
			return _T("u8");
		case PEEK_802_11_UNKNOWN_9_BAND: //   209                 // [u9]"Unknown 9"
			return _T("u9");
		case PEEK_802_11_BAND:
		default:
			return _T("");
	}
}

// -----------------------------------------------------------------------------
//	[static] GetWirelessBandString
// -----------------------------------------------------------------------------

LPCTSTR
CChannelManager::GetWirelessBandShortString( UInt32 ulBand, UInt32 inChannel )
{
	switch ( ulBand )
	{
	// the formal names are too long for some displays, so we abbreviate
	case PEEK_802_11_A_LICENSED_1MHZ_BAND:
		return _T("s1");
	case PEEK_802_11_A_LICENSED_5MHZ_BAND:
		return _T("s5");
	case PEEK_802_11_A_LICENSED_10MHZ_BAND:
		return _T("s10");
	case PEEK_802_11_A_LICENSED_15MHZ_BAND:
		return _T("s15");
	case PEEK_802_11_A_LICENSED_20MHZ_BAND:
		return _T("s20");
	default:	// covers the all other known and unknown bands
		return GetWirelessBandString( ulBand, inChannel );
	}
}

// -----------------------------------------------------------------------------
//	[static] StringToBand
// -----------------------------------------------------------------------------

UInt32
CChannelManager::StringToBand( LPCTSTR psz )
{
	if ( ( 0 == _tcscmp( psz, _T("all") ) ) ||
		 ( 0 == _tcscmp( psz, _T("802.11(all)") ) ) )
	{
		return PEEK_802_11_ALL_BANDS;
	}
	else if ( ( 0 == _tcscmp( psz, _T("b") ) ) || ( 0 == _tcscmp( psz, _T("802.11b") ) ) )
	{
		return PEEK_802_11_B_BAND;
	}
	else if ( ( 0 == _tcscmp( psz, _T("a") ) ) || ( 0 == _tcscmp( psz, _T("802.11a") ) ) ) 
	{
		return PEEK_802_11_A_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("bg") ) )		 || ( 0 == _tcscmp( psz, _T("g") ) )  || 
		 ( 0 == _tcscmp( psz, _T("802.11bg") ) ) || ( 0 == _tcscmp( psz, _T("802.11g") ) ) )
	{
		return PEEK_802_11_BG_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("n") ) ) || ( 0 == _tcscmp( psz, _T("802.11n") ) ) )
	{
		return PEEK_802_11_N_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("n20") ) ) || ( 0 == _tcscmp( psz, _T("802.11n20") ) ) ||
		 ( 0 == _tcscmp( psz, _T("bgn") ) ) || ( 0 == _tcscmp( psz, _T("802.11bgn") ) ) ||
		 ( 0 == _tcscmp( psz, _T("an" ) ) ) || ( 0 == _tcscmp( psz, _T("802.11an" ) ) ) )
	{
		return PEEK_802_11_N_20MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("n40") ) ) || ( 0 == _tcscmp( psz, _T("802.11n40") ) ) )
	{
		return PEEK_802_11_N_40MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("n40l") ) ) || ( 0 == _tcscmp( psz, _T("802.11n40l") ) ) )
	{
		return PEEK_802_11_N_40MHZ_LOW_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("n40h") ) ) || ( 0 == _tcscmp( psz, _T("802.11n40h") ) ) )
	{
		return PEEK_802_11_N_40MHZ_HIGH_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("at") ) ) || ( 0 == _tcscmp( psz, _T("802.11at") ) ) )
	{
		return PEEK_802_11_A_TURBO_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("gt") ) ) || ( 0 == _tcscmp( psz, _T("802.11gt") ) ) )
	{
		return PEEK_802_11_G_TURBO_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("sg") ) ) || ( 0 == _tcscmp( psz, _T("802.11sg") ) ) )
	{
		return PEEK_802_11_SUPER_G_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("BPS 1MHz ChBw") ) ) || ( 0 == _tcscmp( psz, _T("802.11BPS 1MHz ChBw") ) ) )
	{
		return PEEK_802_11_A_LICENSED_1MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("BPS 5MHz ChBw") ) ) || ( 0 == _tcscmp( psz, _T("802.11BPS 6MHz ChBw") ) ) )
	{
		return PEEK_802_11_A_LICENSED_5MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("BPS 10MHz ChBw") ) ) || ( 0 == _tcscmp( psz, _T("802.11BPS 10MHz ChBw") ) ) )
	{
		return PEEK_802_11_A_LICENSED_10MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("BPS 15MHz ChBw") ) ) || ( 0 == _tcscmp( psz, _T("802.11BPS 15MHz ChBw") ) ) )
	{
		return PEEK_802_11_A_LICENSED_15MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("BPS 20MHz ChBw") ) ) || ( 0 == _tcscmp( psz, _T("802.11BPS 20MHz ChBw") ) ) )
	{
		return PEEK_802_11_A_LICENSED_20MHZ_BAND;
	}
	// also do the short band string conversions
	if ( ( 0 == _tcscmp( psz, _T("s1") ) ) || ( 0 == _tcscmp( psz, _T("802.11s1") ) ) )
	{
		return PEEK_802_11_A_LICENSED_1MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("s5") ) ) || ( 0 == _tcscmp( psz, _T("802.11s5") ) ) )
	{
		return PEEK_802_11_A_LICENSED_5MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("s10") ) ) || ( 0 == _tcscmp( psz, _T("802.11s10") ) ) )
	{
		return PEEK_802_11_A_LICENSED_10MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("s15") ) ) || ( 0 == _tcscmp( psz, _T("802.11s15") ) ) )
	{
		return PEEK_802_11_A_LICENSED_15MHZ_BAND;
	}
	if ( ( 0 == _tcscmp( psz, _T("s20") ) ) || ( 0 == _tcscmp( psz, _T("802.11s20") ) ) )
	{
		return PEEK_802_11_A_LICENSED_20MHZ_BAND;
	}

	// unknowns:
	if (*psz == _T('u'))
	{
		psz++;
		// 1-9, and end of string
		if (*psz != _T('0') && *(psz + 1) == 0 && _istdigit(*psz))
		{
			// we start at 1, so -1 off the conversion
			return PEEK_802_11_UNKNOWN_1_BAND + _ttol(psz) - 1;
		}
	}

	return PEEK_802_11_BAND;
}


// -----------------------------------------------------------------------------
//		[static] GetDefault80211DataRates
// -----------------------------------------------------------------------------

UInt32 
CChannelManager::GetDefault80211DataRates( UInt32				inBand,
										   bool					bMCSforN,
										   TArrayEx<UInt32>&	outRates )
{
	outRates.RemoveAllItems();

	switch ( inBand )
	{

		case PEEK_802_11_BAND:
		{
			static const UInt32 ayRates[] = { 2, 4 };	// 1, 2Mbps
			outRates.SetCount(COUNTOF(ayRates));
			for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
				outRates[n] = ayRates[n];	// rates are stored in 0.5Mbps
		}
		break;

		case PEEK_802_11_A_BAND:
		case PEEK_802_11_A_TURBO_BAND:
		{	
			static const UInt32 ayRates[] = { 6, 9, 12, 18, 24, 36, 48, 54 };	// 6, 9, 12, 18, 24, 36, 48, 54Mbps
			outRates.SetCount(COUNTOF(ayRates));
			for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
				outRates[n] = ayRates[n]*2;	// rates are stored in 0.5Mbps, so multiply base rate * 2
		}
		break;

		case PEEK_802_11_B_BAND:
		{
			static const UInt32 ayRates[] = { 2, 4, 11, 22 };	// 1, 2, 5.5, and 11Mbps
			outRates.SetCount(COUNTOF(ayRates));
			for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
				outRates[n] = ayRates[n];	// rates are stored in 0.5Mbps
		}
		break;

		case PEEK_802_11_BG_BAND: // includes PEEK_802_11_G_BAND
		case PEEK_802_11_G_TURBO_BAND:
		case PEEK_802_11_SUPER_G_BAND:
		{
			// G : 1, 2, 5.5, 6, 9, 11, 12, 18, 22, 24, 33, 36, 48, 54
			static const UInt32 ayRates[] = { 2, 4, 11, 12, 18, 22, 24, 36, 48, 72, 96, 108 };
			outRates.SetCount( COUNTOF(ayRates) );
			for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
				outRates[n] = ayRates[n]; // rates are stored in 0.5Mbps
		}
		break;

		case PEEK_802_11_N_20MHZ_BAND:
		{
			if ( bMCSforN )
			{
				outRates.SetCount( 77 ); // From 802.11n std. max MCS value 77.
				for ( UInt32 n = 0; n < outRates.GetCount(); n++ )
				{
					outRates[n] = n;
				}
			}
			else
			{
				static const UInt32 ayRates[] = {2, 4, 11, 12, 13, 18, 22, 24, 26, 27, 36, 39, 48, 52, 54, 
					72, 78, 79, 81, 96, 104, 108, 117, 130, 154, 162, 163, 208, 210, 216, 234, 
					243, 260 };
				outRates.SetCount(COUNTOF(ayRates));
				for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
					outRates[n] = ayRates[n];	// rates are stored in 0.5Mbps
			}
		}
		break;

		case PEEK_802_11_N_BAND:
		case PEEK_802_11_N_40MHZ_BAND:
		case PEEK_802_11_N_40MHZ_LOW_BAND:
		case PEEK_802_11_N_40MHZ_HIGH_BAND:
		{
			if ( bMCSforN )
			{
				outRates.SetCount( 77 ); // From 802.11n std. max MCS value 77.
				for ( UInt32 n = 0; n < outRates.GetCount(); n++ ) 
				{
					outRates[n] = n;
				}
			}
			else
			{
				static const UInt32 ayRates[] = {2, 4, 11, 12, 13, 18, 22, 24, 26, 27, 36, 39, 48, 52, 54, 
					72, 78, 79, 81, 96, 104, 108, 117, 130, 154, 162, 163, 208, 210, 216, 234, 
					243, 260, 270, 324, 432, 486, 540, 600};
				outRates.SetCount(COUNTOF(ayRates));
				for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
					outRates[n] = ayRates[n];	// rates are stored in 0.5Mbps
			}
		}
		break;

		case PEEK_802_11_ALL_BANDS:

		{	// all A, B, G, and N rates
			// A :            6,      9,     12,           18,           24,             36,					   48,     54
			// B : 1, 2, 5.5,            11
			// G :            6,	  9, 11, 12,           18,       22, 24,         33, 36,					   48,     54
			// N : 1, 2, 5.5, 6, 6.5, 9, 11, 12, 13, 13.5, 18, 19.5,     24, 26, 27,     36, 39(78), 39(79), 40.5, 48, 52, 54, 58.5, 65, 78, 81, 81.5, 104, 105, 108, 117, 121.5, 130, 135, 162, 216, 243, 270, 300(pulled from Atheros 802.11n driver)
			static const UInt32 ayRates[] = {2, 4, 11, 12, 13, 18, 22, 24, 26, 27, 36, 39, 48, 52, 54, 
				72, 78, 79, 81, 96, 104, 108, 117, 130, 154, 162, 163, 208, 210, 216, 234, 
				243, 260, 270, 324, 432, 486, 540, 600};
			outRates.SetCount(COUNTOF(ayRates));
			for (UInt32 n = 0; n < COUNTOF(ayRates); n++)
				outRates[n] = ayRates[n];	// rates are stored in 0.5Mbps
		}
		break;

		case PEEK_802_11_A_LICENSED_1MHZ_BAND:
		case PEEK_802_11_A_LICENSED_5MHZ_BAND:
		case PEEK_802_11_A_LICENSED_10MHZ_BAND:
		case PEEK_802_11_A_LICENSED_15MHZ_BAND:
		case PEEK_802_11_A_LICENSED_20MHZ_BAND:
		case PEEK_802_11_UNKNOWN_1_BAND:
		case PEEK_802_11_UNKNOWN_2_BAND:
		case PEEK_802_11_UNKNOWN_3_BAND:
		case PEEK_802_11_UNKNOWN_4_BAND:
		case PEEK_802_11_UNKNOWN_5_BAND:
		case PEEK_802_11_UNKNOWN_6_BAND:
		case PEEK_802_11_UNKNOWN_7_BAND:
		case PEEK_802_11_UNKNOWN_8_BAND:
		case PEEK_802_11_UNKNOWN_9_BAND:
		default:
		{
			outRates.RemoveAllItems();
		}
		break;

	}

	return outRates.GetCount();
}


// -----------------------------------------------------------------------------
//		[static] GetDefault80211Bands
// -----------------------------------------------------------------------------

UInt32 
CChannelManager::GetDefault80211Bands( TArrayEx<UInt32>&	outRates )
{
	outRates.RemoveAllItems();

	outRates.AddItem( PEEK_802_11_ALL_BANDS );
	outRates.AddItem( PEEK_802_11_BAND );
	outRates.AddItem( PEEK_802_11_B_BAND );
	outRates.AddItem( PEEK_802_11_A_BAND );
	outRates.AddItem( PEEK_802_11_G_BAND );
	outRates.AddItem( PEEK_802_11_BG_BAND );
	outRates.AddItem( PEEK_802_11_N_BAND );
	outRates.AddItem( PEEK_802_11_A_TURBO_BAND );
	outRates.AddItem( PEEK_802_11_G_TURBO_BAND );
	outRates.AddItem( PEEK_802_11_SUPER_G_BAND );
	outRates.AddItem( PEEK_802_11_A_LICENSED_1MHZ_BAND );
	outRates.AddItem( PEEK_802_11_A_LICENSED_5MHZ_BAND );
	outRates.AddItem( PEEK_802_11_A_LICENSED_10MHZ_BAND );
	outRates.AddItem( PEEK_802_11_A_LICENSED_15MHZ_BAND );
	outRates.AddItem( PEEK_802_11_A_LICENSED_20MHZ_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_1_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_2_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_3_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_4_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_5_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_6_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_7_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_8_BAND );
	outRates.AddItem( PEEK_802_11_UNKNOWN_9_BAND );
	outRates.AddItem( PEEK_802_11_N_20MHZ_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_HIGH_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_LOW_BAND );

	return outRates.GetCount();
}


// -----------------------------------------------------------------------------
//		[static] GetDefault80211nBands 
// -----------------------------------------------------------------------------

UInt32 
CChannelManager::GetDefault80211nBands(	TArrayEx<UInt32>&	outRates )
{
	outRates.RemoveAllItems();

	outRates.AddItem( PEEK_802_11_N_BAND );
	outRates.AddItem( PEEK_802_11_N_20MHZ_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_HIGH_BAND );
	outRates.AddItem( PEEK_802_11_N_40MHZ_LOW_BAND );

	return outRates.GetCount();
}

// -----------------------------------------------------------------------------
//		[static] GetDefault80211Channels
// -----------------------------------------------------------------------------

static void 
CreateSWirelessChannel( TArrayEx<SWirelessChannel> & ayArray, SInt16 chan, UInt32 freq, UInt32 band ) 
{ 
	SWirelessChannel channel;
	channel.Channel = chan; 
	channel.Frequency = freq; 
	channel.Band = band; 
	ayArray.AddItem( channel ); 
}

static void
AddArrayToArray( const bool in40MHzNonCenterFreq, 
				 const UInt32 inBand,
				 const TArrayEx<SWirelessChannel>& ayArray, 
				 TArrayEx<SWirelessChannel> &outArray )
{
	UInt32 nCount = ayArray.GetCount();
	for ( UInt32 i = 0; i < nCount; i++ )
	{
		SWirelessChannel channel = ayArray[i];
		channel.Band = inBand;
		outArray.AddItem( CChannelManager::SetupWirelessChannel( channel, in40MHzNonCenterFreq ) );
	}
}


UInt32 
CChannelManager::GetDefault80211Channels( UInt32						inBand,
										  TMediaDomain					inDomain,
										  bool							in40MHzNonCenterFreq,	
										  TArrayEx<SWirelessChannel>&	outChannels,
										  bool							inAdd40MhzCenterNonCenterFreq ) // default = false
{

	// Pulled from spec 802.11-maD4.0 and 802.11n-D2.05
	// Turbo channels pulled from: http://madwifi.org/wiki/UserDocs/802.11a_channels
	TArrayEx<SWirelessChannel> ayMKK80211AChannels;
	CreateSWirelessChannel( ayMKK80211AChannels, 34, 0, 0 );
	CreateSWirelessChannel( ayMKK80211AChannels, 38, 0, 0 );
	CreateSWirelessChannel( ayMKK80211AChannels, 42, 0, 0 );
	CreateSWirelessChannel( ayMKK80211AChannels, 46, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayETSI80211AChannels;
	CreateSWirelessChannel( ayETSI80211AChannels, 36, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 40, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 44, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 48, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 52, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 56, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 60, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 64, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 100, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 104, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 108, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 112, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 116, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 120, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 124, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 128, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 132, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 136, 0, 0 );
	CreateSWirelessChannel( ayETSI80211AChannels, 140, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayFCC80211AChannels;
	CreateSWirelessChannel( ayFCC80211AChannels, 36, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 40, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 44, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 48, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 52, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 56, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 60, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 64, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 100, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 104, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 108, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 112, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 116, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 120, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 124, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 128, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 132, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 136, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 140, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 149, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 153, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 157, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 161, 0, 0 );
	CreateSWirelessChannel( ayFCC80211AChannels, 165, 0, 0 );
	
	TArrayEx<SWirelessChannel> ay80211AChannels;
	CreateSWirelessChannel( ay80211AChannels, 34, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 36, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 38, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 40, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 44, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 46, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 48, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 52, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 56, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 60, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 64, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 100, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 104, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 108, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 112, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 116, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 120, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 124, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 128, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 132, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 136, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 140, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 149, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 153, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 157, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 161, 0, 0 );
	CreateSWirelessChannel( ay80211AChannels, 165, 0, 0 );

	TArrayEx<SWirelessChannel> ay80211ATurboChannels;
	CreateSWirelessChannel( ay80211ATurboChannels, 42, 0, 0 );
	CreateSWirelessChannel( ay80211ATurboChannels, 50, 0, 0 );
	CreateSWirelessChannel( ay80211ATurboChannels, 58, 0, 0 );
	CreateSWirelessChannel( ay80211ATurboChannels, 152, 0, 0 );
	CreateSWirelessChannel( ay80211ATurboChannels, 160, 0, 0 );

	TArrayEx<SWirelessChannel> ayFCC80211BChannels;
	CreateSWirelessChannel( ayFCC80211BChannels, 1, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 2, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 3, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 4, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 5, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 6, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 7, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 8, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 9, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 10, 0, 0 );
	CreateSWirelessChannel( ayFCC80211BChannels, 11, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayETSI80211BChannels;
	CreateSWirelessChannel( ayETSI80211BChannels, 1, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 2, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 3, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 4, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 5, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 6, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 7, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 8, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 9, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 10, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 11, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 12, 0, 0 );
	CreateSWirelessChannel( ayETSI80211BChannels, 13, 0, 0 );

	TArrayEx<SWirelessChannel> ayMKK80211BChannels;
	CreateSWirelessChannel( ayMKK80211BChannels, 14, 0, 0 );

	TArrayEx<SWirelessChannel> ay80211BChannels;
	CreateSWirelessChannel( ay80211BChannels, 1, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 2, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 3, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 4, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 5, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 6, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 7, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 8, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 9, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 10, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 11, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 12, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 13, 0, 0 );
	CreateSWirelessChannel( ay80211BChannels, 14, 0, 0 );

	TArrayEx<SWirelessChannel> ayFCC80211N_24_HighChannels;
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 1, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 2, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 3, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 4, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 5, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 6, 0, 0 ); 
	CreateSWirelessChannel( ayFCC80211N_24_HighChannels, 7, 0, 0 );

	TArrayEx<SWirelessChannel> ayFCC80211N_5_HighChannels;
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 36, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 44, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 52, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 60, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 100, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 108, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 116, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 124, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 132, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 149, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_HighChannels, 157, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayFCC80211N_24_LowChannels;
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 5, 0, 0 ); 
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 6, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 7, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 8, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 9, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 10, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_24_LowChannels, 11, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayFCC80211N_5_LowChannels; 
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 40, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 48, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 56, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 64, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 104, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 112, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 120, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 128, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 136, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 153, 0, 0 );
	CreateSWirelessChannel( ayFCC80211N_5_LowChannels, 161, 0, 0 );

	TArrayEx<SWirelessChannel> ayETSI80211N_24_HighChannels; 
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 1, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 2, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 3, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 4, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 5, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 6, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 7, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 8, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_HighChannels, 9, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayETSI80211N_5_HighChannels; 
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 36, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 44, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 52, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 60, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 100, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 108, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 116, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 124, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_HighChannels, 132, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayETSI80211N_24_LowChannels;
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 5, 0, 0 ); 
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 6, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 7, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 8, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 9, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 10, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 11, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 12, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_24_LowChannels, 13, 0, 0 );
	
	TArrayEx<SWirelessChannel> ayETSI80211N_5_LowChannels; 
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 40, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 48, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 56, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 64, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 104, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 112, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 120, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 128, 0, 0 );
	CreateSWirelessChannel( ayETSI80211N_5_LowChannels, 136, 0, 0 );

	switch ( inBand )
	{
		case PEEK_802_11_A_BAND:
		case PEEK_802_11_A_TURBO_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_MKK:	
					if ( inBand != PEEK_802_11_A_TURBO_BAND )
					{
						AddArrayToArray( false, inBand, ayMKK80211AChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					break;

				case kMediaDomain_ETSI:	
					if ( inBand != PEEK_802_11_A_TURBO_BAND )
					{
						AddArrayToArray( false, inBand, ayETSI80211AChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					break;

				case kMediaDomain_FCC:
					if ( inBand != PEEK_802_11_A_TURBO_BAND )
					{
						AddArrayToArray( false, inBand, ayFCC80211AChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					break;

				default:
					if ( inBand != PEEK_802_11_A_TURBO_BAND )
					{
						AddArrayToArray( false, inBand, ay80211AChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					break;
			}
		}
		break;

		case PEEK_802_11_BAND:
		case PEEK_802_11_B_BAND:
//		case PEEK_802_11_G_BAND:
		case PEEK_802_11_BG_BAND:
		case PEEK_802_11_G_TURBO_BAND:
		case PEEK_802_11_SUPER_G_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_FCC:		// channels 1-11
					AddArrayToArray( false, inBand, ayFCC80211BChannels, outChannels );
					break;

				case kMediaDomain_MKK:		// channel 14
					AddArrayToArray( false, inBand, ayMKK80211BChannels, outChannels );

				case kMediaDomain_ETSI:		// channel 1-13, this is for all of europe
					AddArrayToArray( false, inBand, ayETSI80211BChannels, outChannels );
					break;

				default:					// all channels available
					AddArrayToArray( false, inBand, ay80211BChannels, outChannels );
					break;
			}
		}
		break;

		case PEEK_802_11_ALL_BANDS:
		case PEEK_802_11_N_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_FCC:		
				{
					if ( inBand == PEEK_802_11_ALL_BANDS )
					{
						AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					}
					else
					{
						AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ayFCC80211BChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ayFCC80211AChannels, outChannels );					
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
					}

					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_24_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
					}
				}
				break;

				case kMediaDomain_ETSI:
				{
					if ( inBand == PEEK_802_11_ALL_BANDS )
					{
						AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
					}
					else
					{
						AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ayETSI80211BChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ayETSI80211AChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_5_HighChannels, outChannels );
					}

					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_5_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_5_LowChannels, outChannels );
					}
				}
				break;

				default:
				{
					if ( inBand == PEEK_802_11_ALL_BANDS )
					{
						AddArrayToArray( false, PEEK_802_11_A_TURBO_BAND, ay80211ATurboChannels, outChannels );
						AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ay80211AChannels, outChannels );
					}
					else
					{
						AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ayFCC80211AChannels, outChannels );
					}
					AddArrayToArray( false, PEEK_802_11_N_20MHZ_BAND, ay80211BChannels, outChannels );
					
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
					}

					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
					}
					
				}
				break;
			}

			if ( inBand == PEEK_802_11_ALL_BANDS )
			{
				TArrayEx<SWirelessChannel> tempChannels;
				UInt32 nTotal = outChannels.GetCount();
				for ( UInt32 i = 0; i < nTotal; i++ )
				{
					outChannels[ i ].Band = inBand;
					UInt32 nCount = tempChannels.GetCount();
					bool bFound = false;
					for ( UInt32 j = 0; j < nCount; j++ )
					{
						if ( ( tempChannels[ j ].Channel == outChannels[ i ].Channel ) && 
							 ( tempChannels[ j ].Frequency == outChannels[ i ].Frequency ) )
						{
							bFound = true;
							break;
						}
					}
					if ( !bFound )
					{
						tempChannels.AddItem( outChannels[ i ] );
					}
				}
				outChannels = tempChannels;

			}
		}
		break;

		case PEEK_802_11_N_20MHZ_BAND:
		{	
			switch ( inDomain )
			{
				case kMediaDomain_FCC:		
					AddArrayToArray( false, inBand, ayFCC80211BChannels, outChannels );
					AddArrayToArray( false, inBand, ayFCC80211AChannels, outChannels );
					break;

				case kMediaDomain_MKK:
					AddArrayToArray( false, inBand, ayMKK80211BChannels, outChannels );
					AddArrayToArray( false, inBand, ayMKK80211AChannels, outChannels );
					break;

				case kMediaDomain_ETSI:
					AddArrayToArray( false, inBand, ayETSI80211BChannels, outChannels );
					AddArrayToArray( false, inBand, ayETSI80211AChannels, outChannels );
					break;

				default:
					AddArrayToArray( false, inBand, ay80211BChannels, outChannels );
					AddArrayToArray( false, inBand, ay80211AChannels, outChannels );
					break;
			}
		}
		break;

		case PEEK_802_11_N_40MHZ_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_ETSI:
				{				
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayETSI80211N_5_HighChannels, outChannels );
					}

					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_5_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayETSI80211N_5_LowChannels, outChannels );
					}
				}
				break;

				case kMediaDomain_FCC:
				default:
					{
						AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_24_HighChannels, outChannels );
						AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
						if ( inAdd40MhzCenterNonCenterFreq )
						{
							AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_24_HighChannels, outChannels );
							AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_HIGH_BAND, ayFCC80211N_5_HighChannels, outChannels );
						}

						AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_24_LowChannels, outChannels );
						AddArrayToArray( in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
						if ( inAdd40MhzCenterNonCenterFreq )
						{
							AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_24_LowChannels, outChannels );
							AddArrayToArray( !in40MHzNonCenterFreq, PEEK_802_11_N_40MHZ_LOW_BAND, ayFCC80211N_5_LowChannels, outChannels );
						}
					}
					break;
			}

			for ( UInt32 i = 0; i < outChannels.GetCount(); i++ )
			{
				outChannels[ i ].Band = inBand;
			}
		}
		break;

		case PEEK_802_11_N_40MHZ_LOW_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_ETSI:
				{
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayETSI80211N_24_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayETSI80211N_5_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayETSI80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayETSI80211N_5_LowChannels, outChannels );
					}
				}
				break;

				case kMediaDomain_FCC:
				default:
				{
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayFCC80211N_24_LowChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayFCC80211N_5_LowChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayFCC80211N_24_LowChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayFCC80211N_5_LowChannels, outChannels );
					}
				}
				break;
			}
		}
		break;

		case PEEK_802_11_N_40MHZ_HIGH_BAND:
		{
			switch ( inDomain )
			{
				case kMediaDomain_ETSI:
				{
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayETSI80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayETSI80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayETSI80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayETSI80211N_5_HighChannels, outChannels );
					}
				}
				break;

				case kMediaDomain_FCC:
				default:
				{
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayFCC80211N_24_HighChannels, outChannels );
					AddArrayToArray( in40MHzNonCenterFreq, inBand, ayFCC80211N_5_HighChannels, outChannels );
					if ( inAdd40MhzCenterNonCenterFreq )
					{
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayFCC80211N_24_HighChannels, outChannels );
						AddArrayToArray( !in40MHzNonCenterFreq, inBand, ayFCC80211N_5_HighChannels, outChannels );
					}
				}
				break;
			}
		}
		break;

		case PEEK_802_11_A_LICENSED_1MHZ_BAND:
		case PEEK_802_11_A_LICENSED_5MHZ_BAND:
		case PEEK_802_11_A_LICENSED_10MHZ_BAND:
		case PEEK_802_11_A_LICENSED_15MHZ_BAND:
		case PEEK_802_11_A_LICENSED_20MHZ_BAND:
		case PEEK_802_11_UNKNOWN_1_BAND:
		case PEEK_802_11_UNKNOWN_2_BAND:
		case PEEK_802_11_UNKNOWN_3_BAND:
		case PEEK_802_11_UNKNOWN_4_BAND:
		case PEEK_802_11_UNKNOWN_5_BAND:
		case PEEK_802_11_UNKNOWN_6_BAND:
		case PEEK_802_11_UNKNOWN_7_BAND:
		case PEEK_802_11_UNKNOWN_8_BAND:
		case PEEK_802_11_UNKNOWN_9_BAND:
		default:
		{
			outChannels.RemoveAllItems();
		}
	}

	outChannels.SetComparator( new CWirelessChannelFreqComparator() );
	outChannels.Sort();

	return outChannels.GetCount();
}

