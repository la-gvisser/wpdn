// =============================================================================
//	MCSTable.h
// =============================================================================
//	Copyright (c) 2014 WildPackets, Inc. All rights reserved.

#ifndef WILDPACKETS_MCS_TABLE_H_
#define WILDPACKETS_MCS_TABLE_H_

#include "AGTypes.h"
#include <string>

namespace MCSTable
{
	namespace detail
	{
		//for backwards compatibility.
		//member functions simply forward on to the free functions
		struct MCSTableFacade
		{
		public:
			MCSTableFacade() {}
			~MCSTableFacade() {}

			double GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const;
			UInt16 GetMCSIndex(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const;
			UInt32 GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader) const;
		};
	}

	detail::MCSTableFacade* GetInstance();

	double GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader);
	double GetDataRate(UInt32 band, UInt16 dataRate, UInt32 flagNHeader, std::string& errorMsg);

	UInt16 GetMCSIndex(UInt32 band, UInt16 dataRate, UInt32 flagNHeader);
	UInt32 GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader);
	UInt32 GetSpatialStreams(UInt32 band, UInt16 dataRate, UInt32 flagNHeader, std::string& errorMsg);
}

#endif

