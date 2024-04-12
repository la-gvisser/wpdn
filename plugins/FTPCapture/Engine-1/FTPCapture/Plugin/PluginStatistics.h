// =============================================================================
//	PluginStatistics.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#pragma once


class CFtpProcess;

class IStatistics
{
public:
	virtual size_t	GetStatValue( size_t inIndex = 0 ) = 0;
	virtual void	ResetStatValue( size_t inIndex = 0 ) = 0;
};


class CPluginStatistics
{
public:
	typedef enum {
		kStat_ActiveUsers,
		kStat_ActiveDataChannels,
		kStat_Logons,
		kStat_Logoffs,
		kStat_Received,
		kStat_Stored, 
		kStat_TimedOutControl,
		kStat_TimedOutData,
		kStat_Maximum
	} FTP_Statistic;

protected:
	class CPluginStat
	{
	public:
		virtual void	Reset() = 0;
		virtual size_t	Get() = 0;
		virtual void	Increment( size_t inValue = 1 ) = 0;
	};

	class CPluginStatValue : public CPluginStat
	{
	protected:
		size_t	m_nValue;

	public:
		;		CPluginStatValue() : m_nValue( 0 ) {}
		void	Reset() { m_nValue = 0; }
		size_t	Get() { return m_nValue; }
		void	Increment( size_t inValue = 1 ) { m_nValue += inValue; }
	};

	class CPluginStatFunc : public CPluginStat
	{
	protected:
		size_t			m_nIndex;
		IStatistics&	m_Stat;

	public:
		;		CPluginStatFunc( IStatistics& inStat, size_t inIndex = 0 )
			: m_Stat( inStat ), m_nIndex( inIndex ) {}

		void	Reset() { m_Stat.ResetStatValue( m_nIndex ); }
		size_t	Get() { return m_Stat.GetStatValue( m_nIndex ); }
		void	Increment( size_t /* inValue = 1 */ ) {}
	};

	CAtlArray<CPluginStat*>	m_Stats;
	CPeekString				m_strLabels[kStat_Maximum];

public:
	;		CPluginStatistics();
	;		~CPluginStatistics();

	void	Init( CFtpProcess* inContext );
	void	Reset();
	void	Increment( size_t inIndex, size_t inValue = 1 );
	bool	Get( size_t inIndex, size_t& outStat, CPeekString& outLabel );
};
