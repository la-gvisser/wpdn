// =============================================================================
//	ChannelMgr.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#ifndef CHANNELMGR_H
#define CHANNELMGR_H

#include "ArrayEx.h"
#include "PacketHeaders.h"
#include "Comparator.h"
#include "MediaTypes.h"
#include <map>
#include <set>

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#endif // #ifndef COUNTOF

class CArrayEx;

class CChannelManager
{
public:
	typedef enum
	{
		kWirelessChannels,
		kEnumChannels,
		kMaxType
	} ChannelType;

	CChannelManager(ChannelType inType = kMaxType);
	CChannelManager(const CChannelManager& other);
	virtual ~CChannelManager();

	ChannelType GetChannelType() const { return m_type; }
	void SetChannelType(ChannelType type);

	// return raw count or -1 for invalid
	SInt32 GetCount() const;
	// return a pointer to the data and the count
	bool GetChannels(void** pData, SInt32 *pnCount);
	// return a pointer to the data and the count
	bool GetChannels(const void** pData, SInt32 *pnCount) const;
	// set the channel info in bulk
	bool SetChannels(const void* pData, SInt32 nCount);
	// add a channel if it does not exist, returns index of channel
	SInt32 AddChannel(const void* pData);

	CChannelManager& operator=(const CChannelManager& other);

	static bool	IsValidChannel(UInt32 nBand, SInt16 nChannel, SInt16 * pOutMin = NULL, SInt16 *pOutMax = NULL);
	static SWirelessChannel SetupWirelessChannel(TMediaType mt, TMediaSubType mst, int nChannel);
	static SWirelessChannel SetupWirelessChannel(UInt32 nBand, int nChannel, bool in40MHzNonCenterFreq = false);
	static SWirelessChannel SetupWirelessChannel(const SWirelessChannel & channel, bool in40MHzNonCenterFreq = false);
	static void FormatWirelessChannel(LPTSTR psz, int cchMax, SWirelessChannel theChannel);
#if defined(_AFX) || defined(_ATL)
	static void FormatWirelessChannel(CString& strChannel, SWirelessChannel theChannel)
	{
		FormatWirelessChannel(strChannel.GetBuffer(64), 64, theChannel);
		strChannel.ReleaseBuffer();
	}
#endif
	static SWirelessChannel ScanfWirelessChannel(LPCTSTR psz);
	static LPCTSTR GetWirelessBandString(UInt32 inBand, UInt32 inChannel = 0);
	static LPCTSTR GetWirelessBandShortString(UInt32 inBand, UInt32 inChannel = 0);
	static UInt32  StringToBand(LPCTSTR psz);
	static UInt32 GetDefault80211DataRates(UInt32				inBand,
		bool				bMCSforN,
		TArrayEx<UInt32>&	outRates);
	static UInt32 GetDefault80211Bands(TArrayEx<UInt32>& outBands);
	static UInt32 GetDefault80211nBands(TArrayEx<UInt32>& outBands);
	static UInt32 GetDefault80211Channels(UInt32						inBand,
		TMediaDomain					inDomain,
		bool							in40MHzNonCenterFreq,
		TArrayEx<SWirelessChannel>&	outSetupChannels,
		bool							inAdd40MhzCenterNonCenterFreq = false);

protected:
	CArrayEx*	m_payChannelData;
	ChannelType	m_type;
};

// compare class for sorting, etc. of wireless channels

class CWirelessChannelComparator : public CComparator
{
public:
	virtual CComparator*	Clone() const
	{
		return new CWirelessChannelComparator;
	}

	virtual SInt32			Compare(const void* inItem1, const void* inItem2,
		UInt32 /*inSize1*/, UInt32 /*inSize2*/) const
	{
		// first sort by channel frequency, then by channel index, then by band flags
		SWirelessChannel	*pOne = (SWirelessChannel*)inItem1,
			*pTwo = (SWirelessChannel*)inItem2;

		// frequency compare
		SInt32 nDiff = (pOne->Frequency - pTwo->Frequency);
		if (nDiff == 0)
		{	// channel compare
			nDiff = (pOne->Channel - pTwo->Channel);
			if (nDiff == 0)
			{	// how to diff band flags?
				nDiff = CompareBands(pOne->Band, pTwo->Band);
			}
		}

		return nDiff;
	}

	virtual bool			IsEqualTo(const void* inItem1, const void* inItem2,
		UInt32 inSize1, UInt32 inSize2) const
	{
		return (Compare(inItem1, inItem2, inSize1, inSize2) == 0);
	}

	static SInt32			CompareBands(UInt32 inBandOne, UInt32 inBandTwo)
	{
		if (inBandOne == inBandTwo) return 0;
		// sort order (ascending)
		// generic, b, g/bg, gt, super-g, a, at, n
		const UInt32 aySortOrder[] =
		{
			PEEK_802_11_ALL_BANDS,
			PEEK_802_11_BAND,
			PEEK_802_11_B_BAND,
			PEEK_802_11_G_BAND,
			PEEK_802_11_BG_BAND,
			PEEK_802_11_G_TURBO_BAND,
			PEEK_802_11_SUPER_G_BAND,
			PEEK_802_11_A_BAND,
			PEEK_802_11_A_TURBO_BAND,
			PEEK_802_11_N_BAND,
			PEEK_802_11_N_20MHZ_BAND,
			PEEK_802_11_N_40MHZ_BAND,
			PEEK_802_11_N_40MHZ_HIGH_BAND,
			PEEK_802_11_N_40MHZ_LOW_BAND,
			PEEK_802_11_A_LICENSED_1MHZ_BAND,
			PEEK_802_11_A_LICENSED_5MHZ_BAND,
			PEEK_802_11_A_LICENSED_10MHZ_BAND,
			PEEK_802_11_A_LICENSED_15MHZ_BAND,
			PEEK_802_11_A_LICENSED_20MHZ_BAND,
		};

		// find each in the array
		int	i, j;
		for (i = 0; i < (int)COUNTOF(aySortOrder); i++)
			if (aySortOrder[i] == inBandOne)
				break;

		for (j = 0; j < (int)COUNTOF(aySortOrder); j++)
			if (aySortOrder[j] == inBandTwo)
				break;

		// take the difference
		return i - j;
	}

	static CWirelessChannelComparator*		GetComparator()
	{
		static CWirelessChannelComparator	s_Comparator;
		return &s_Comparator;
	}
};


class CWirelessChannelFreqComparator : public CComparator
{
public:
	virtual CComparator*	Clone() const
	{
		return new CWirelessChannelFreqComparator;
	}

	virtual SInt32			Compare(const void* inItem1, const void* inItem2,
		UInt32 /*inSize1*/, UInt32 /*inSize2*/) const
	{
		// first sort by channel frequency, then by channel index, then by band flags
		SWirelessChannel	*pOne = (SWirelessChannel*)inItem1,
			*pTwo = (SWirelessChannel*)inItem2;

		// channel compare
		SInt32 nDiff = (pOne->Channel - pTwo->Channel);
		if (nDiff == 0)
		{	// how to diff band flags?
			nDiff = (pOne->Frequency - pTwo->Frequency);
			if (nDiff == 0)
			{
				nDiff = CWirelessChannelComparator::CompareBands(pOne->Band, pTwo->Band);
			}
		}

		return nDiff;
	}

	virtual bool			IsEqualTo(const void* inItem1, const void* inItem2,
		UInt32 inSize1, UInt32 inSize2) const
	{
		return (Compare(inItem1, inItem2, inSize1, inSize2) == 0);
	}


	static CWirelessChannelFreqComparator*		GetComparator()
	{
		static CWirelessChannelFreqComparator	s_Comparator;
		return &s_Comparator;
	}
};


class ChannelUtil
{
public:
	bool IsValidChannel(UInt32 nBand, SInt16 nChannel);

private:
	std::map<UInt32, std::set<SInt16> >	m_bandDefaultChannelMap;
};


// there may be a better place for this, but for now, it goes here
namespace WirelessHeaderUtil
{
	// -----------------------------------------------------------------------------
	//		[inline] InitMediaSpecInfo
	// -----------------------------------------------------------------------------

	inline void InitMediaSpecInfo(Wireless80211PrivateHeader* pHeader)
	{
		ASSERT(pHeader != NULL);
		if (pHeader == NULL) return;
		memset(pHeader, 0, sizeof(*pHeader));
		// special cases of NULL_DBM
		pHeader->SignaldBm = PEEK_NULL_DBM;
		pHeader->SignaldBm1 = PEEK_NULL_DBM;
		pHeader->SignaldBm2 = PEEK_NULL_DBM;
		pHeader->SignaldBm3 = PEEK_NULL_DBM;
		pHeader->NoisedBm = PEEK_NULL_DBM;
		pHeader->NoisedBm1 = PEEK_NULL_DBM;
		pHeader->NoisedBm2 = PEEK_NULL_DBM;
		pHeader->NoisedBm3 = PEEK_NULL_DBM;
	}

	// -----------------------------------------------------------------------------
	//		[inline] dBmConvert
	// -----------------------------------------------------------------------------

	inline SInt8 dBmConvert(SInt16 dBmOld)
	{	// simple value/range checks and casting
		return (dBmOld == PEEK_NULL_DBM_SHORT) ? PEEK_NULL_DBM :
			(dBmOld == PEEK_NULL_DBM) ? PEEK_NULL_DBM :
			(dBmOld < PEEK_MIN_DBM) ? PEEK_MIN_DBM :
			(dBmOld > PEEK_MAX_DBM) ? PEEK_MAX_DBM :
			(SInt8)dBmOld;
	}

	inline bool IsACBand(UInt32 inBand)
	{
		return inBand == PEEK_802_11_AC_BW80_PRIMARY_0_BAND ||
			inBand == PEEK_802_11_AC_BW80_PRIMARY_1_BAND ||
			inBand == PEEK_802_11_AC_BW80_PRIMARY_2_BAND ||
			inBand == PEEK_802_11_AC_BW80_PRIMARY_3_BAND ||
			inBand == PEEK_802_11_AC_NSS_2_BAND_0 ||
			inBand == PEEK_802_11_AC_NSS_2_BAND_1 ||
			inBand == PEEK_802_11_AC_NSS_2_BAND_2 ||
			inBand == PEEK_802_11_AC_NSS_2_BAND_3;

	}
};

#endif /* CHANNELMGR_H */
