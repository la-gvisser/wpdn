// ============================================================================
//	EmailStats.h
//		interface for the CEmailStats class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"


// ============================================================================
//		CEmailStats
// ============================================================================

class CEmailStats
{
public:
	enum EmailStatType {
		kStat_TypeSmtp,
		kStat_TypePop3,
		kStat_TypeCount
	};

	enum EmailStat {
		kStat_SmtpEmailsDetected,
		kStat_SmtpFullEmailsCaptured,
		kStat_SmtpPenEmailsCaptured,
		kStat_SmtpEmailResets,
		kStat_SmtpEmailMalformed,
		kStat_SmtpSessionsTimeOut,

		kStat_Pop3EmailsDetected,
		kStat_Pop3FullEmailsCaptured,
		kStat_Pop3PenEmailsCaptured,
		kStat_Pop3NoTargets,
		kStat_Pop3EmailResets,
		kStat_Pop3EmailMalformed,
		kStat_Pop3SessionsTimeOut,

		kStat_FileError,
		kStat_StatCount,
		kStat_MaxStat
	};

	class CStatsLabels
		:	public CArrayString
	{
	public:
		;	CStatsLabels();
	};

protected:
	size_t			m_aStats[kStat_MaxStat];
	CStatsLabels	m_aLabels;

public:
	;				CEmailStats() : m_aLabels() { Reset(); }
	;				~CEmailStats() {};

	EmailStat		FirstIndex() const { return kStat_SmtpEmailsDetected; }
	size_t			Get( EmailStat inStat ) const { return (inStat < kStat_StatCount) ? m_aStats[inStat] : 0; } 
	void			Increment( EmailStat inStat, size_t inCount = 1 ) {
		if ( inStat < kStat_StatCount ) m_aStats[inStat] += inCount;
	}
	const CPeekString&	Label( EmailStat inStat ) const {
			return (inStat < kStat_StatCount) ? m_aLabels[inStat] : m_aLabels[kStat_StatCount];
	} 
	EmailStat		NextIndex( EmailStat pos ) const { return static_cast<EmailStat>( pos + 1 ); }
	void			Reset() {
		for ( size_t i = 0; i < kStat_MaxStat; i++ ) {
			m_aStats[i] = 0;
		}
	}
};
