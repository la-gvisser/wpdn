// =============================================================================
//	MCSTable.cpp
// =============================================================================
//	Copyright (c) 2014 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "MCSTable.h"
#include "PacketHeaders.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace
{
	enum eModulationTypes
	{
		UNKNOWN_MODULATION = 0,
		BPSK,
		QPSK,
		QAM_16,
		QAM_64,
		QAM_256
	};

	enum eCodingTypes
	{
		UNKNOWN_CODING = 0,
		ONE_OVER_TWO,
		THREE_OVER_FOUR,
		TWO_OVER_THREE,
		FIVE_OVER_SIX
	};

	enum eGuardIntervals
	{
		EIGHT_HUNDRED_NS = 0,
		FOUR_HUNDRED_NS
	};

	enum eBandwidths
	{
		TWENTY_MHZ = 0,
		FORTY_MHZ,
		EIGHTY_MHZ,
		ONE_HUNDRED_SIXTY_MHZ
	};

	static const size_t RATE_TABLE_SIZE = 8;
	static const double DATA_RATE_MAGNITUDE = 10.0;

	class MCSTableImpl
	{
	public:
		struct Row
		{	
		public:
			Row(UInt8 streams, eModulationTypes modulation, eCodingTypes coding, const UInt32 (&dataRates)[RATE_TABLE_SIZE])
				: streamCount(streams),
				modulationType(modulation),
				codingType(coding)
			{
				std::copy(dataRates, dataRates + RATE_TABLE_SIZE, rates);
			}

			Row(UInt8 streams, eModulationTypes modulation, eCodingTypes coding, 
					UInt32 rate1, UInt32 rate2, UInt32 rate3, UInt32 rate4,
					UInt32 rate5, UInt32 rate6, UInt32 rate7, UInt32 rate8)
				: streamCount(streams),
				modulationType(modulation),
				codingType(coding)
			{
				rates[0] = rate1;
				rates[1] = rate2;
				rates[2] = rate3;
				rates[3] = rate4;
				rates[4] = rate5;
				rates[5] = rate6;
				rates[6] = rate7;
				rates[7] = rate8;
			}
			
			UInt8 StreamCount() const { return streamCount; }
			eModulationTypes ModulationType() const { return modulationType; }
			eCodingTypes CodingType() const { return codingType; }
			
			double DataRate(size_t index) const
			{
				if (index >= RATE_TABLE_SIZE)
					throw std::out_of_range("Row::DataRate: index exceeds size of rate table");

				return static_cast<double>(rates[index]) / DATA_RATE_MAGNITUDE;
			}

			double DataRate(eBandwidths bandwidth, eGuardIntervals interval) const
			{
				size_t index = (2 * static_cast<size_t>(bandwidth)) + static_cast<size_t>(interval);
				return DataRate(index);
			}

		private:
			UInt8 streamCount;
			eModulationTypes modulationType;
			eCodingTypes codingType;

			//data rates are stored as fixed point decimals with one decimal place.
			//this avoids any problems with floating point accuracy
			UInt32 rates[RATE_TABLE_SIZE];
		};

	public:
		MCSTableImpl()
		{
			CreateMCSTable();
		}

		~MCSTableImpl() {}

		const Row& GetHTRow(size_t mcsIndex) const
		{
			if (mcsIndex >= m_HTTable.size())
				throw std::out_of_range("MCSTableImpl::GetHTRow: invalid mcs index for 802.11n");

			size_t rowIndex = m_HTTable[mcsIndex];

			return m_Rates[rowIndex];
		}

		const Row& GetVHTRow(size_t mcsIndex, UInt8 streamCount) const
		{
			if (mcsIndex > MAX_802_11AC_MCS_INDEX)
				throw std::out_of_range("MCSTableImpl::GetVHTRow: invalid mcs index for 802.11ac");

			if ((streamCount == 0) || (streamCount > MAX_802_11AC_STREAM_COUNT))
				throw std::out_of_range("MCSTableImpl::GetVHTRow: invalid stream count for 802.11ac");

			//for 802.11ac the mcs index is always a value between 0-9
			//use the stream count to offset into the correct part of the rate table.
			size_t tableIndex = ((MAX_802_11AC_MCS_INDEX + 1) * (streamCount - 1)) + mcsIndex;
			size_t rowIndex = m_VHTTable[tableIndex];

			return m_Rates[rowIndex];
		}

	private:
		void CreateMCSTable()
		{
			//first, create the rate table
			//note that the rates are stored as fixed point decimals with one decimal place
			m_Rates.push_back(Row(1, BPSK, ONE_OVER_TWO,		65, 72, 135, 150, 293, 325, 585, 650));
			m_Rates.push_back(Row(1, QPSK, ONE_OVER_TWO,		130, 144, 270, 300, 585, 650, 1170, 1300));
			m_Rates.push_back(Row(1, QPSK, THREE_OVER_FOUR,		195, 217, 405, 450, 878, 975, 1755, 1950));
			m_Rates.push_back(Row(1, QAM_16, ONE_OVER_TWO,		260, 289, 540, 600, 1170, 1300, 2340, 2600));
			m_Rates.push_back(Row(1, QAM_16, THREE_OVER_FOUR,	390, 433, 810, 900, 1755, 1950, 3510, 3900));
			m_Rates.push_back(Row(1, QAM_64, TWO_OVER_THREE,	520, 578, 1080, 1200, 2340, 2600, 4680, 5200));
			m_Rates.push_back(Row(1, QAM_64, THREE_OVER_FOUR,	585, 650, 1215, 1350, 2633, 2925, 5265, 5850));
			m_Rates.push_back(Row(1, QAM_64, FIVE_OVER_SIX,		650, 722, 1350, 1500, 2925, 3250, 5850, 6500));
			m_Rates.push_back(Row(1, QAM_256, THREE_OVER_FOUR,	780, 867, 1620, 1800, 3510, 3900, 7020, 7800));
			m_Rates.push_back(Row(1, QAM_256, FIVE_OVER_SIX,	0, 0, 1800, 2000, 3900, 4333, 7800, 8667));
			m_Rates.push_back(Row(2, BPSK, ONE_OVER_TWO,		130, 144, 270, 300, 585, 650, 1170, 1300));
			m_Rates.push_back(Row(2, QPSK, ONE_OVER_TWO,		260, 289, 540, 600, 1170, 1300, 2340, 2600));
			m_Rates.push_back(Row(2, QPSK, THREE_OVER_FOUR,		390, 433, 810, 900, 1755, 1950, 3510, 3900));
			m_Rates.push_back(Row(2, QAM_16, ONE_OVER_TWO,		520, 578, 1080, 1200, 2340, 2600, 4680, 5200));
			m_Rates.push_back(Row(2, QAM_16, THREE_OVER_FOUR,	780, 867, 1620, 1800, 3510, 3900, 7020, 7800));
			m_Rates.push_back(Row(2, QAM_64, TWO_OVER_THREE,	1040, 1156, 2160, 2400, 4680, 5200, 9360, 10400));
			m_Rates.push_back(Row(2, QAM_64, THREE_OVER_FOUR,	1170, 1303, 2430, 2700, 5265, 5850, 10530, 11700));
			m_Rates.push_back(Row(2, QAM_64, FIVE_OVER_SIX,		1300, 1444, 2700, 3000, 5850, 6500, 11700, 13000));
			m_Rates.push_back(Row(2, QAM_256, THREE_OVER_FOUR,	1560, 1733, 3240, 3600, 7020, 7800, 14040, 15600));
			m_Rates.push_back(Row(2, QAM_256, FIVE_OVER_SIX,	0, 0, 3600, 4000, 7800, 8667, 15600, 17333));
			m_Rates.push_back(Row(3, BPSK, ONE_OVER_TWO,		195, 217, 405, 450, 878, 975, 1755, 1950));
			m_Rates.push_back(Row(3, QPSK, ONE_OVER_TWO,		390, 433, 810, 900, 1755, 1950, 3510, 3900));
			m_Rates.push_back(Row(3, QPSK, THREE_OVER_FOUR,		585, 650, 1215, 1350, 2633, 2925, 5265, 5850));
			m_Rates.push_back(Row(3, QAM_16, ONE_OVER_TWO,		780, 867, 1620, 1800, 3510, 3900, 7020, 7800));
			m_Rates.push_back(Row(3, QAM_16, THREE_OVER_FOUR,	1170, 1300, 2430, 2700, 5265, 5850, 10530, 11700));
			m_Rates.push_back(Row(3, QAM_64, TWO_OVER_THREE,	1560, 1733, 3240, 3600, 7020, 7800, 14040, 15600));
			m_Rates.push_back(Row(3, QAM_64, THREE_OVER_FOUR,	1755, 1950, 3645, 4050, 0, 0, 15795, 17550));
			m_Rates.push_back(Row(3, QAM_64, FIVE_OVER_SIX,		1950, 2167, 4050, 4500, 8775, 9750, 17550, 19500));
			m_Rates.push_back(Row(3, QAM_256, THREE_OVER_FOUR,	2340, 2600, 4860, 5400, 10530, 11700, 21060, 23400));
			m_Rates.push_back(Row(3, QAM_256, FIVE_OVER_SIX,	2600, 2889, 5400, 6000, 11700, 13000, 0, 0));
			m_Rates.push_back(Row(4, BPSK, ONE_OVER_TWO,		260, 289, 540, 600, 1170, 1300, 2340, 2600));
			m_Rates.push_back(Row(4, QPSK, ONE_OVER_TWO,		520, 578, 1080, 1200, 2340, 2600, 4680, 5200));
			m_Rates.push_back(Row(4, QPSK, THREE_OVER_FOUR,		780, 867, 1620, 1800, 3510, 3900, 7020, 7800));
			m_Rates.push_back(Row(4, QAM_16, ONE_OVER_TWO,		1040, 1156, 2160, 2400, 4680, 5200, 9360, 10400));
			m_Rates.push_back(Row(4, QAM_16, THREE_OVER_FOUR,	1560, 1733, 3240, 3600, 7020, 7800, 14040, 15600));
			m_Rates.push_back(Row(4, QAM_64, TWO_OVER_THREE,	2080, 2311, 4320, 4800, 9360, 10400, 18720, 20800));
			m_Rates.push_back(Row(4, QAM_64, THREE_OVER_FOUR,	2340, 2600, 4860, 5400, 10530, 11700, 21060, 23400));
			m_Rates.push_back(Row(4, QAM_64, FIVE_OVER_SIX,		2600, 2889, 5400, 6000, 11700, 13000, 23400, 26000));
			m_Rates.push_back(Row(4, QAM_256, THREE_OVER_FOUR,	3120, 3467, 6480, 7200, 14040, 15600, 28080, 31200));
			m_Rates.push_back(Row(4, QAM_256, FIVE_OVER_SIX,	0, 0, 7200, 8000, 15600, 17333, 31200, 34667));
			m_Rates.push_back(Row(5, BPSK, ONE_OVER_TWO,		0, 0, 0, 0, 1463, 1625, 2925, 3250));
			m_Rates.push_back(Row(5, QPSK, ONE_OVER_TWO,		0, 0, 0, 0, 2925, 3250, 5850, 6500));
			m_Rates.push_back(Row(5, QPSK, THREE_OVER_FOUR,		0, 0, 0, 0, 4388, 4875, 8775, 9750));
			m_Rates.push_back(Row(5, QAM_16, ONE_OVER_TWO,		0, 0, 0, 0, 5850, 6500, 11700, 13000));
			m_Rates.push_back(Row(5, QAM_16, THREE_OVER_FOUR,	0, 0, 0, 0, 8775, 9750, 17550, 19500));
			m_Rates.push_back(Row(5, QAM_64, TWO_OVER_THREE,	0, 0, 0, 0, 11700, 13000, 23400, 26000));
			m_Rates.push_back(Row(5, QAM_64, THREE_OVER_FOUR,	0, 0, 0, 0, 13163, 14625, 26325, 29250));
			m_Rates.push_back(Row(5, QAM_64, FIVE_OVER_SIX,		0, 0, 0, 0, 14625, 16250, 29250, 32500));
			m_Rates.push_back(Row(5, QAM_256, THREE_OVER_FOUR,	0, 0, 0, 0, 17550, 19500, 35100, 39000));
			m_Rates.push_back(Row(5, QAM_256, FIVE_OVER_SIX,	0, 0, 0, 0, 19500, 21667, 39000, 43333));
			m_Rates.push_back(Row(6, BPSK, ONE_OVER_TWO,		0, 0, 0, 0, 1755, 1950, 3510, 3900));
			m_Rates.push_back(Row(6, QPSK, ONE_OVER_TWO,		0, 0, 0, 0, 3510, 3900, 7020, 7800));
			m_Rates.push_back(Row(6, QPSK, THREE_OVER_FOUR,		0, 0, 0, 0, 5265, 5850, 10530, 11700));
			m_Rates.push_back(Row(6, QAM_16, ONE_OVER_TWO,		0, 0, 0, 0, 7020, 7800, 14040, 15600));
			m_Rates.push_back(Row(6, QAM_16, THREE_OVER_FOUR,	0, 0, 0, 0, 10530, 11700, 21060, 23400));
			m_Rates.push_back(Row(6, QAM_64, TWO_OVER_THREE,	0, 0, 0, 0, 14040, 15600, 28080, 31200));
			m_Rates.push_back(Row(6, QAM_64, THREE_OVER_FOUR,	0, 0, 0, 0, 15795, 17550, 31590, 35100));
			m_Rates.push_back(Row(6, QAM_64, FIVE_OVER_SIX,		0, 0, 0, 0, 17550, 19500, 35100, 39000));
			m_Rates.push_back(Row(6, QAM_256, THREE_OVER_FOUR,	0, 0, 0, 0, 21060, 23400, 42120, 46800));
			m_Rates.push_back(Row(6, QAM_256, FIVE_OVER_SIX,	0, 0, 0, 0, 0, 0, 46800, 52000));
			m_Rates.push_back(Row(7, BPSK, ONE_OVER_TWO,		0, 0, 0, 0, 2048, 2275, 4095, 4550));
			m_Rates.push_back(Row(7, QPSK, ONE_OVER_TWO,		0, 0, 0, 0, 4095, 4550, 8190, 9100));
			m_Rates.push_back(Row(7, QPSK, THREE_OVER_FOUR,		0, 0, 0, 0, 6143, 6825, 12285, 13650));
			m_Rates.push_back(Row(7, QAM_16, ONE_OVER_TWO,		0, 0, 0, 0, 8190, 9100, 16380, 18200));
			m_Rates.push_back(Row(7, QAM_16, THREE_OVER_FOUR,	0, 0, 0, 0, 12285, 13650, 24570, 27300));
			m_Rates.push_back(Row(7, QAM_64, TWO_OVER_THREE,	0, 0, 0, 0, 16380, 18200, 32760, 36400));
			m_Rates.push_back(Row(7, QAM_64, THREE_OVER_FOUR,	0, 0, 0, 0, 0, 0, 36855, 40950));
			m_Rates.push_back(Row(7, QAM_64, FIVE_OVER_SIX,		0, 0, 0, 0, 20475, 22750, 40950, 45500));
			m_Rates.push_back(Row(7, QAM_256, THREE_OVER_FOUR,	0, 0, 0, 0, 24570, 27300, 49140, 54600));
			m_Rates.push_back(Row(7, QAM_256, FIVE_OVER_SIX,	0, 0, 0, 0, 27300, 30333, 54600, 60667));
			m_Rates.push_back(Row(8, BPSK, ONE_OVER_TWO,		0, 0, 0, 0, 2340, 2600, 4680, 5200));
			m_Rates.push_back(Row(8, QPSK, ONE_OVER_TWO,		0, 0, 0, 0, 4680, 5200, 9360, 10400));
			m_Rates.push_back(Row(8, QPSK, THREE_OVER_FOUR,		0, 0, 0, 0, 7020, 7800, 14040, 15600));
			m_Rates.push_back(Row(8, QAM_16, ONE_OVER_TWO,		0, 0, 0, 0, 9360, 10400, 18720, 20800));
			m_Rates.push_back(Row(8, QAM_16, THREE_OVER_FOUR,	0, 0, 0, 0, 14040, 15600, 28080, 31200));
			m_Rates.push_back(Row(8, QAM_64, TWO_OVER_THREE,	0, 0, 0, 0, 18720, 20800, 37440, 41600));
			m_Rates.push_back(Row(8, QAM_64, THREE_OVER_FOUR,	0, 0, 0, 0, 21060, 23400, 42120, 46800));
			m_Rates.push_back(Row(8, QAM_64, FIVE_OVER_SIX,		0, 0, 0, 0, 23400, 26000, 46800, 52000));
			m_Rates.push_back(Row(8, QAM_256, THREE_OVER_FOUR,	0, 0, 0, 0, 28080, 31200, 56160, 62400));
			m_Rates.push_back(Row(8, QAM_256, FIVE_OVER_SIX,	0, 0, 0, 0, 31200, 34667, 62400, 69333));

			//now create the index tables
			for (size_t i = 0; i < m_Rates.size(); ++i)
			{
				const Row& row = m_Rates[i];

				m_VHTTable.push_back(i);

				//rows with a modulation type of 256-QAM are only applicable to the VHT Table
				if ( (row.ModulationType() != QAM_256) && (row.StreamCount() <= MAX_802_11N_STREAM_COUNT))
					m_HTTable.push_back(i);
			}
		}

	private:
		typedef std::vector<Row> RateTable_t;
		typedef std::vector<size_t> IndexTable_t;

		RateTable_t m_Rates;
		IndexTable_t m_HTTable;		//802.11n rates
		IndexTable_t m_VHTTable;	//802.11ac rates

		static const UInt8 MAX_802_11N_STREAM_COUNT = 4;
		static const UInt8 MAX_802_11AC_STREAM_COUNT = 8;
		static const size_t MAX_802_11AC_MCS_INDEX = 9;
	};

	static const MCSTableImpl g_MCSTable;
}

namespace MCSTable
{
	namespace detail
	{
		double
		MCSTableFacade::GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const
		{
			return MCSTable::GetDataRate(band, dataRate, flagNHeader);
		}

		UInt16
			MCSTableFacade::GetMCSIndex(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const
		{
			return MCSTable::GetMCSIndex(band, dataRate, flagNHeader);
		}

		UInt32
			MCSTableFacade::GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const
		{
			return MCSTable::GetSpatialStreams(band, dataRate, flagNHeader);
		}

	}

	typedef std::pair<eBandwidths, eGuardIntervals> DataRateKey_t;

	bool HasBandwidthFlag(UInt32 flags)
	{
		static const UInt32 BANDWIDTH_MASK = PEEK_FLAGN_20MHZ_LOWER | PEEK_FLAGN_20MHZ_UPPER |
			PEEK_FLAGN_40MHz | PEEK_FLAGN_80MHz | PEEK_FLAGN_160MHz;

		return ((flags & BANDWIDTH_MASK) > 0);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetDataRateKey
	//
	//	input:	802.11N bitset
	//	output: DataRateKey_t 
	//  throws: invalid_argument exception if a DataRateKey cannot be generated from the bitset.
	//
	//	Creates a DataRateKey_t with the Bandwidth and GuardInterval values based on the
	//	bitfields set in the flags argument.
	//
	//////////////////////////////////////////////////////////////////////////
	DataRateKey_t GetDataRateKey(UInt32 flags)
	{
		static const UInt32 FLAG_MASK = PEEK_FLAGN_20MHZ_LOWER | PEEK_FLAGN_20MHZ_UPPER |
			PEEK_FLAGN_40MHz | PEEK_FLAGN_80MHz | PEEK_FLAGN_160MHz |
			PEEK_FLAGN_HALF_GI | PEEK_FLAGN_FULL_GI;

		static const UInt32 TWENTY_LOWER_FULL_GI = PEEK_FLAGN_FULL_GI | PEEK_FLAGN_20MHZ_LOWER;
		static const UInt32 TWENTY_UPPER_FULL_GI = PEEK_FLAGN_FULL_GI | PEEK_FLAGN_20MHZ_UPPER;
		static const UInt32 TWENTY_LOWER_HALF_GI = PEEK_FLAGN_HALF_GI | PEEK_FLAGN_20MHZ_LOWER;
		static const UInt32 TWENTY_UPPER_HALF_GI = PEEK_FLAGN_HALF_GI | PEEK_FLAGN_20MHZ_UPPER;
		static const UInt32 FORTY_FULL_GI = PEEK_FLAGN_FULL_GI | PEEK_FLAGN_40MHz;
		static const UInt32 FORTY_HALF_GI = PEEK_FLAGN_HALF_GI | PEEK_FLAGN_40MHz;
		static const UInt32 EIGHTY_FULL_GI = PEEK_FLAGN_FULL_GI | PEEK_FLAGN_80MHz;
		static const UInt32 EIGHTY_HALF_GI = PEEK_FLAGN_HALF_GI | PEEK_FLAGN_80MHz;
		static const UInt32 ONE_HUNDRED_SIXTY_FULL_GI = PEEK_FLAGN_FULL_GI | PEEK_FLAGN_160MHz;
		static const UInt32 ONE_HUNDRED_SIXTY_HALF_GI = PEEK_FLAGN_HALF_GI | PEEK_FLAGN_160MHz;

		switch (flags & FLAG_MASK)
		{
		case TWENTY_LOWER_FULL_GI:
		case TWENTY_UPPER_FULL_GI:
		{
			return DataRateKey_t(TWENTY_MHZ, EIGHT_HUNDRED_NS);
		}
		case TWENTY_LOWER_HALF_GI:
		case TWENTY_UPPER_HALF_GI:
		{
			return DataRateKey_t(TWENTY_MHZ, FOUR_HUNDRED_NS);
		}
		case FORTY_FULL_GI: return DataRateKey_t(FORTY_MHZ, EIGHT_HUNDRED_NS);
		case FORTY_HALF_GI: return DataRateKey_t(FORTY_MHZ, FOUR_HUNDRED_NS);
		case EIGHTY_FULL_GI: return DataRateKey_t(EIGHTY_MHZ, EIGHT_HUNDRED_NS);
		case EIGHTY_HALF_GI: return DataRateKey_t(EIGHTY_MHZ, FOUR_HUNDRED_NS);
		case ONE_HUNDRED_SIXTY_FULL_GI: return DataRateKey_t(ONE_HUNDRED_SIXTY_MHZ, EIGHT_HUNDRED_NS);
		case ONE_HUNDRED_SIXTY_HALF_GI: return DataRateKey_t(ONE_HUNDRED_SIXTY_MHZ, FOUR_HUNDRED_NS);
		default: break;
		}

		throw std::invalid_argument("GetDataRateKey: flags bitfield does not map to a rate key");
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetInstance
	//
	//	input:	none
	//	output: detail::MCSTableFacade
	//  throws: none
	//
	//	Included for backwards compatibility.
	//	Creates a static instance of the MCSTableFacade struct and returns a pointer to the static instance.
	//
	//////////////////////////////////////////////////////////////////////////
	detail::MCSTableFacade* GetInstance()
	{
		static detail::MCSTableFacade table;
		return &table;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetDataRate
	//
	//	input:	
	//		band:	a value indicating which 802.11 spec was used to receive the packet
	//		dataRate:	may contain a dataRate or an MCS index depending on the bits set in the flagNHeader
	//		flagNHeader:	bitset indicating various 802.11 options that were enabled when receiving the packet
	//
	//	output: a value (in Mbps) from the MCS Table or 0 if an error occurred.
	//  throws: none
	//
	//	Uses the band, dataRate and flagNHeader fields to generate an index into the MCSTable.
	//	Returns the data rate from the MCSTable associated with the generated index.
	//	Any errors are silently discarded.
	//
	//////////////////////////////////////////////////////////////////////////
	double GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader)
	{
		std::string error;
		return GetDataRate(band, dataRate, flagNHeader, error);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetDataRate
	//
	//	input:	
	//		band:	a value indicating which 802.11 spec was used to receive the packet
	//		dataRate:	may contain a dataRate or an MCS index depending on the bits set in the flagNHeader
	//		flagNHeader:	bitset indicating various 802.11 options that were enabled when receiving the packet
	//		errorMsg:	empty on success, on failure contains a description of what caused the error.
	//
	//	output: a value (in Mbps) from the MCS Table or 0 if an error occurred.
	//  throws: none
	//
	//	Uses the band, dataRate and flagNHeader fields to generate an index into the MCSTable.
	//	Returns the data rate from the MCSTable associated with the generated index.
	//	If an error occurs a description of the error is stored in errorMsg and 0 is returned
	//
	//////////////////////////////////////////////////////////////////////////
	double GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader, std::string& errorMsg)
	{
		double rate = 0.0;

		errorMsg.clear();

		//make sure an mcs index is present
		if ((flagNHeader & PEEK_FLAGN_MCS) == 0)
		{
			rate = static_cast<double>(dataRate) / 2.0;
			return rate;
		}
		
		bool hasBandwidthFlag = HasBandwidthFlag(flagNHeader);

		//Sometimes third party vendors don't set the flags in the flagsN bitset.
		//In order to simplify the code, set some of the flagN bits based on the band information.
		switch (band)
		{
		case PEEK_802_11_N_20MHZ_BAND:
		{
			if (hasBandwidthFlag == false)
			{
				flagNHeader |= PEEK_FLAGN_20MHZ_LOWER;
				hasBandwidthFlag = true;
			}
		} break;
		case PEEK_802_11_N_40MHZ_BAND:            
		case PEEK_802_11_N_40MHZ_LOW_BAND:        
		case PEEK_802_11_N_40MHZ_HIGH_BAND:
		{
			if (hasBandwidthFlag == false)
			{
				flagNHeader |= PEEK_FLAGN_40MHz;
				hasBandwidthFlag = true;
			}
		} break;
		case PEEK_802_11_AC_BW80_PRIMARY_0_BAND:
		case PEEK_802_11_AC_BW80_PRIMARY_1_BAND:
		case PEEK_802_11_AC_BW80_PRIMARY_2_BAND:
		case PEEK_802_11_AC_BW80_PRIMARY_3_BAND:
		case PEEK_802_11_AC_NSS_2_BAND_0:
		case PEEK_802_11_AC_NSS_2_BAND_1:
		case PEEK_802_11_AC_NSS_2_BAND_2:
		case PEEK_802_11_AC_NSS_2_BAND_3:
		{
			if ((hasBandwidthFlag == false) && ((band == PEEK_802_11_AC_NSS_2_BAND_0) || (band == PEEK_802_11_AC_NSS_2_BAND_1)) )
			{
				//why only AC_NSS_2_BAND_0 and AC_NSS_2_BAND_1?
				//I have no idea, but that was how the old code did it so I am keeping it for compatibility.
				flagNHeader |= PEEK_FLAGN_80MHz;
				hasBandwidthFlag = true;
			}

			//since this is ac traffic, make sure that the VHT flag is set
			flagNHeader |= PEEK_FLAGN_VHT;
		} break;
		default: break;
		}

		if (hasBandwidthFlag == false)
		{
			//bandwidth still not set, default to 20Mhz
			flagNHeader |= PEEK_FLAGN_20MHZ_LOWER;
			hasBandwidthFlag = true;
		}

		size_t mcsIndex = GetMCSIndex(band, dataRate, flagNHeader);
		UInt32 streamCount = GetSpatialStreams(band, dataRate, flagNHeader);

		try
		{
			DataRateKey_t rateKey = GetDataRateKey(flagNHeader);

			//ideally the VHT flag will be set for 802.11ac traffic
			//but in case it is not, check the band
			if ( ((flagNHeader & PEEK_FLAGN_VHT) != 0) )
			{
				rate = g_MCSTable.GetVHTRow(mcsIndex, static_cast<UInt8>(streamCount)).DataRate(rateKey.first, rateKey.second);
			}
			else
			{
				//not 802.11ac traffic, must be 802.11n
				rate = g_MCSTable.GetHTRow(mcsIndex).DataRate(rateKey.first, rateKey.second);
			}
		}
		catch (const std::exception& e)
		{
			errorMsg = e.what();
		}
		
		return rate;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetMCSIndex
	//
	//	input:	
	//		band:	unused
	//		dataRate:	may contain a dataRate or an MCS index depending on the bits set in the flagNHeader
	//		flagNHeader:	bitset indicating various 802.11 options that were enabled when receiving the packet
	//
	//	output: a valid index in the MCSTable or -1 on failure
	//  throws: none
	//
	//	Uses the dataRate and flagNHeader fields to generate an index into the MCSTable.
	//
	//////////////////////////////////////////////////////////////////////////
	UInt16 GetMCSIndex(UInt32 /*band*/, UInt16 dataRate, UInt32 flagNHeader)
	{
		UInt16 mcsIndex = static_cast<UInt16>(-1);

		//make sure an mcs index is present
		if ((flagNHeader & PEEK_FLAGN_MCS) == 0)
			return mcsIndex;

		mcsIndex = GET_RATE_MCS(dataRate);

		return mcsIndex;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetSpatialStreams
	//
	//	input:	
	//		band:		a value indicating which 802.11 spec was used to receive the packet
	//		dataRate:	may contain a dataRate or an MCS index depending on the bits set in the flagNHeader
	//		flagNHeader:	bitset indicating various 802.11 options that were enabled when receiving the packet
	//
	//	output: the number of spatial streams
	//  throws: none
	//
	//	Uses the band, dataRate and flagNHeader arguments to determine the number of spatial streams.
	//	Errors are silently discarded.
	//
	//////////////////////////////////////////////////////////////////////////
	UInt32 GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader)
	{
		std::string errorMsg;
		return GetSpatialStreams(band, dataRate, flagNHeader, errorMsg);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	GetSpatialStreams
	//
	//	input:	
	//		band:		a value indicating which 802.11 spec was used to receive the packet
	//		dataRate:	may contain a dataRate or an MCS index depending on the bits set in the flagNHeader
	//		flagNHeader:	bitset indicating various 802.11 options that were enabled when receiving the packet
	//		errorMsg:	empty on success, contains a description of the error on failure.
	//
	//	output: the number of spatial streams
	//  throws: none
	//
	//	Uses the band, dataRate and flagNHeader arguments to determine the number of spatial streams.
	//	If an error occurs a description of the error is stored in errorMsg.
	//
	//////////////////////////////////////////////////////////////////////////
	UInt32 GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader, std::string& errorMsg)
	{
		UInt8 streamCount = 1;

		errorMsg.clear();

		//make sure an mcs index is present
		if ((flagNHeader & PEEK_FLAGN_MCS) == 0)
			return streamCount;

		if ((flagNHeader & PEEK_FLAGN_NSS) != 0)
		{
			streamCount = GET_RATE_NSS(dataRate);
		}
		else if ((band == PEEK_802_11_AC_NSS_2_BAND_0) || (band == PEEK_802_11_AC_NSS_2_BAND_1))
		{
			//why only band_0 and band_1, what about band_2 and band_3?
			if ((flagNHeader & PEEK_FLAGN_CISCO_AC_3) != 0)
			{
				streamCount = 3;
			}
			else if ((flagNHeader & PEEK_FLAGN_CISCO_AC_2) != 0)
			{
				streamCount = 2;
			}
		}
		else if ( ((flagNHeader & PEEK_FLAGN_VHT) == 0) && (band != PEEK_802_11_AC_BW80_PRIMARY_0_BAND) &&
			(band != PEEK_802_11_AC_BW80_PRIMARY_1_BAND) &&
			(band != PEEK_802_11_AC_BW80_PRIMARY_2_BAND) &&
			(band != PEEK_802_11_AC_BW80_PRIMARY_3_BAND) &&
			(band != PEEK_802_11_AC_NSS_2_BAND_2) &&
			(band != PEEK_802_11_AC_NSS_2_BAND_3) )
		{
			//not ac, must be n
			//use the mcsindex to determine how many streams there are
			size_t mcsIndex = GET_RATE_MCS(dataRate);

			try
			{
				streamCount = g_MCSTable.GetHTRow(mcsIndex).StreamCount();
			}
			catch (const std::exception& e)
			{
				errorMsg = e.what();
			}
		}

		return streamCount;
	}
}

