// ============================================================================
//	RadiusStats.h
//		interface for the CRadiusStats class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#ifdef RADIUS_STATS
#pragma once

class CRadiusStats
{
public:
	enum EMailStatType {
		kStat_TypeSmtp,
		kStat_TypePop3,
		kStat_TypeHotmail,
		kStat_TypeCount
	};

	enum EMailStat {
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

	class CStatsLabels : public CArrayString
	{
	public:
		CStatsLabels::CStatsLabels( CPeekPlugin& inPlugin );
	};

protected:
	UInt32			m_aStats[kStat_MaxStat];
	CStatsLabels	m_aLabels;

public:
	;				CRadiusStats( CPeekPlugin& inPlugin );
	;				~CRadiusStats();

	void			Reset();
	EMailStat		FirstIndex() const { return kStat_SmtpEmailsDetected; }
	EMailStat		NextIndex( EMailStat pos ) const { return (EMailStat)(pos + 1); }
	void			Increment( EMailStat inStat, size_t inCount = 1 ) { if ( inStat < kStat_StatCount ) m_aStats[inStat] += inCount; }
	UInt32			Get( EMailStat inStat ) const { return ((inStat < kStat_StatCount) ? m_aStats[inStat] : 0); } 
	const CString&	Label( EMailStat inStat ) const {
			return ((inStat < kStat_StatCount) ? m_aLabels[inStat] : m_aLabels[kStat_StatCount]); } 
};
#endif //RADIUS_STATS