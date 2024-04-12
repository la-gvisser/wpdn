// =============================================================================
//	WirelessHeaderFacade.h
// =============================================================================
//	Copyright (c) 2014 WildPackets, Inc. All rights reserved.

#ifndef WILDPACKETS_WIRELESS_HEADER_FACADE_H_
#define WILDPACKETS_WIRELESS_HEADER_FACADE_H_

#include "PacketHeaders.h"

class WirelessHeaderFacade
{
public:
	WirelessHeaderFacade() {}
	virtual ~WirelessHeaderFacade() {}

public:
	virtual SWirelessChannel GetWirelessChannel() const = 0;
	virtual UInt32 GetBand() const = 0;
	virtual UInt16 GetDataRate() const = 0;
	virtual UInt32 GetFlagsN() const = 0;
	virtual UInt8 GetStreamCount() const = 0;
	virtual UInt8 GetSignalLevel() const = 0;
	virtual SInt8 GetSignaldBm() const = 0;
	virtual SInt8 GetSignaldBmN(UInt8 nStream) const = 0;
	virtual UInt8 GetNoiseLevel() const = 0;
	virtual SInt8 GetNoisedBm() const = 0;
	virtual SInt8 GetNoisedBmN(UInt8 nStream) const = 0;
};


namespace detail
{
	template<typename T>
	class WirelessHeaderFacadeImpl;

	template<>
	class WirelessHeaderFacadeImpl<Wireless80211PrivateHeader> : public WirelessHeaderFacade
	{
	public:
		typedef Wireless80211PrivateHeader header_type;

	public:
		WirelessHeaderFacadeImpl(const header_type& header)
			: m_pHeader(&header)
		{}

		virtual ~WirelessHeaderFacadeImpl() {}

	public:
		virtual SWirelessChannel GetWirelessChannel() const
		{
			SWirelessChannel temp;
			temp.Band = m_pHeader->Channel.Band;
			temp.Channel = m_pHeader->Channel.Channel;
			temp.Frequency = m_pHeader->Channel.Frequency;

			return temp;
		}

		virtual UInt32 GetBand() const { return m_pHeader->Channel.Band; }
		virtual UInt16 GetDataRate() const	{ return m_pHeader->DataRate; }
		virtual UInt32 GetFlagsN() const	{ return m_pHeader->FlagsN; }
		virtual UInt8 GetStreamCount() const	{ return MAX_STREAM_COUNT; }	//returns the total number of signal/noise dbm values available
		virtual UInt8 GetSignalLevel() const	{ return m_pHeader->SignalPercent; }
		virtual SInt8 GetSignaldBm() const	{ return m_pHeader->SignaldBm; }

		virtual SInt8 GetSignaldBmN(UInt8 nStream) const
		{
			switch (nStream)
			{
			case 0: { return m_pHeader->SignaldBm; }
			case 1: { return m_pHeader->SignaldBm1; }
			case 2: { return m_pHeader->SignaldBm2; }
			case 3: { return m_pHeader->SignaldBm3; }
			default: break;
			}

			return PEEK_NULL_DBM;
		}

		virtual UInt8 GetNoiseLevel() const	{ return m_pHeader->NoisePercent; }
		virtual SInt8 GetNoisedBm() const	{ return m_pHeader->NoisedBm; }

		virtual SInt8 GetNoisedBmN(UInt8 nStream) const
		{
			switch (nStream)
			{
			case 0: { return m_pHeader->NoisedBm; }
			case 1: { return m_pHeader->NoisedBm1; }
			case 2: { return m_pHeader->NoisedBm2; }
			case 3: { return m_pHeader->NoisedBm3; }
			default: break;
			}

			return PEEK_NULL_DBM;
		}

	private:
		const header_type* m_pHeader;

		static const UInt8 MAX_STREAM_COUNT = 4;
	};

	//derived class for extracting wireless fields from the TunneledWirelessHeader
	//implements the Packet::IWirelessHeader interface
	template<>
	class WirelessHeaderFacadeImpl<TunneledWirelessHeader> : public WirelessHeaderFacade
	{
	public:
		typedef TunneledWirelessHeader header_type;

	public:
		WirelessHeaderFacadeImpl(const header_type& header)
			: m_pHeader(&header)
		{}

		virtual ~WirelessHeaderFacadeImpl() {}

	public:
		virtual SWirelessChannel GetWirelessChannel() const
		{
			SWirelessChannel temp;
			temp.Band = m_pHeader->Wireless.Channel.Band;
			temp.Channel = m_pHeader->Wireless.Channel.Channel;
			temp.Frequency = m_pHeader->Wireless.Channel.Frequency;

			return temp;
		}

		virtual UInt32 GetBand() const { return m_pHeader->Wireless.Channel.Band; }
		virtual UInt16 GetDataRate() const	{ return m_pHeader->Wireless.DataRate; }
		virtual UInt32 GetFlagsN() const	{ return m_pHeader->Wireless.FlagsN; }
		virtual UInt8 GetStreamCount() const	{ return MAX_STREAM_COUNT; }	//returns the total number of signal/noise dbm values available
		virtual UInt8 GetSignalLevel() const	{ return m_pHeader->Wireless.SignalPercent; }
		virtual SInt8 GetSignaldBm() const	{ return m_pHeader->Wireless.SignaldBm; }

		virtual SInt8 GetSignaldBmN(UInt8 nStream) const
		{
			switch (nStream)
			{
			case 0: { return m_pHeader->Wireless.SignaldBm; }
			case 1: { return m_pHeader->Wireless.SignaldBm1; }
			case 2: { return m_pHeader->Wireless.SignaldBm2; }
			case 3: { return m_pHeader->Wireless.SignaldBm3; }
			case 4: { return m_pHeader->SignaldBm4; }
			default: break;
			}

			return PEEK_NULL_DBM;
		}

		virtual UInt8 GetNoiseLevel() const	{ return m_pHeader->Wireless.NoisePercent; }
		virtual SInt8 GetNoisedBm() const	{ return m_pHeader->Wireless.NoisedBm; }

		virtual SInt8 GetNoisedBmN(UInt8 nStream) const
		{
			switch (nStream)
			{
			case 0: { return m_pHeader->Wireless.NoisedBm; }
			case 1: { return m_pHeader->Wireless.NoisedBm1; }
			case 2: { return m_pHeader->Wireless.NoisedBm2; }
			case 3: { return m_pHeader->Wireless.NoisedBm3; }
			case 4: { return m_pHeader->NoisedBm4; }
			default: break;
			}

			return PEEK_NULL_DBM;
		}

	private:
		const header_type* m_pHeader;

		static const UInt8 MAX_STREAM_COUNT = 5;
	};
}	//namespace detail

typedef detail::WirelessHeaderFacadeImpl<Wireless80211PrivateHeader> Wireless80211PrivateHeaderFacade;
typedef detail::WirelessHeaderFacadeImpl<TunneledWirelessHeader> TunneledWirelessHeaderFacade;
#endif
