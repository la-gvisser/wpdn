// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "Version.h"
#include "PeekLock.h"
#include "LockedPtr.h"
#include "FileEx.h"
#include "Case.h"

class COmniPlugin;
class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;

#if (0)
// ============================================================================
//		COptionsView
// ============================================================================

class COptionsView
{
public:
	class CColumnProp
	{
	protected:
		int				m_nId;
		CPeekString		m_strLabel;
		CPeekString		m_strConfigTag;
		int				m_nWidth;

	public:
		;		CColumnProp() : m_nWidth( 0 ) {}
		;		CColumnProp( int inId, const CPeekString& inLabel,
					const CPeekString& inConfigTag, int inWidth )
			:	m_nId( inId )
			,	m_strLabel( inLabel )
			,	m_strConfigTag( inConfigTag )
			,	m_nWidth( inWidth )
		{}

		int					GetId() const { return m_nId; }
		const CPeekString&	GetLabel() const { return m_strLabel; }
		const CPeekString&	GetConfigTag() const { return m_strConfigTag; }
		int					GetWidth() const { return m_nWidth; }

		void		SetLabel( const CPeekString& inLabel ) { m_strLabel = inLabel; }
		void		SetConfigTag( const CPeekString& inConfigTag ) { m_strConfigTag = inConfigTag; }
		void		SetWidth( int inWidth ) { m_nWidth = inWidth; }
	};
	typedef CAtlArray<CColumnProp>	CColumnPropList;

protected:
	// Dialog size
	CSize		m_Size;
	bool		m_bIntegrity;

	// Case List Columns
	CColumnPropList	m_ColumnPropList;

public:
	;			COptionsView();
	;			COptionsView( const COptionsView& inOther ) { Copy( inOther ); }

	COptionsView&	operator=( const COptionsView& inOther ) { return Copy( inOther ); }
	COptionsView&	Copy( const COptionsView& inOther ) {
		m_Size = inOther.m_Size;
		m_bIntegrity = inOther.m_bIntegrity;
		m_ColumnPropList.Copy( inOther.m_ColumnPropList );
		return *this;
	}

	CSize		GetSize() const { return m_Size; }
	CColumnPropList&	GetColumnList() { return m_ColumnPropList; }

	bool		IsIntegrity() const { return m_bIntegrity; }

	void		SetSize( CSize inSize ) {
		if ( (inSize.cx > 200) && (inSize.cy > 200) ) {
			m_Size = inSize;
		}
	}
	void		SetIntegrity( bool inEnable ) { m_bIntegrity = inEnable; }
};
#endif


// =============================================================================
//		COptions
// =============================================================================

class COptions
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
friend class CSafeOptions;
friend class COptionsDialog;

public:
	enum CaptureOptions {
		kOptions_PenEmail,
		kOptions_PenText,
		kOptions_FullEmail,
		kOptions_FullPktFile,
		kOptions_Count
	};

	// Case List Options
	class COptionsCases {
	protected:
		CCaseList		m_CaseList;

	public:
		bool	Add( const CCase& inCase );
		void	Analyze( CMsgStream* inMsgStream ) {
			m_CaseList.Analyze( inMsgStream );
		}
		bool	Find( CCase::tCaseId inId, size_t& outIndex ) const { return m_CaseList.Find( inId, outIndex ); }

		const CCase&		GetAt( size_t inIndex ) const { return m_CaseList.GetAt( inIndex ); }
		size_t				GetCount() const { return m_CaseList.GetCount(); }
		int					GetCount32() const { return m_CaseList.GetCount32(); }
		const CCaseList&	GetList() const { return m_CaseList; }

		bool	IsAcceptPackets() const { return m_CaseList.IsAcceptPackets(); }
		bool	IsEmpty() const { return m_CaseList.IsEmpty(); }
		bool	IsSummaryStats() const { return m_CaseList.IsSummaryStats(); }
		bool	IsLogMessages() const { return m_CaseList.IsLogMessages(); }
		bool	IsProtocolOfInterest( CProtocolType inProtocolType ) const {
			return m_CaseList.IsProtocolOfInterest( inProtocolType );
		}
		bool	IsSlicePackets() const { return m_CaseList.IsSlicePackets(); }
		
		void	Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const {
			m_CaseList.Log( inPreAmble, inFile );
		}
		void	LogUpdate( const CPeekStringA& inPreAmble, const COptionsCases& inCases, CFileEx& inFile ) {
			m_CaseList.LogUpdate( inPreAmble, inCases.m_CaseList, inFile );
		}
		
		bool	Model( CPeekDataElement& ioParent ) { return m_CaseList.Model( ioParent ); }
		
		void	RemoveAt( int inIndex ) { m_CaseList.RemoveAt( inIndex ); }
		void	Reset() { m_CaseList.RemoveAll(); }
		
		bool	Update( int inIndex, const CCase& inCase );
	};

protected:
	static CPeekString	s_strDefault;

protected:
	CVersion			m_Version;
	UInt32				m_XmlVersion;

#if (0)
	COptionsView		m_View;
#endif
	COptionsCases		m_Cases;

public:
	static const CPeekString&	GetTagDefaults() { return s_strDefault; }

public:
	;			COptions() : m_XmlVersion( 1 ) {}
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions() {}

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}

	void	Analyze( CMsgStream* inMsgStream ) {
		m_Cases.Analyze( inMsgStream );
	}

	void		Copy( const COptions& inOther );

	const COptionsCases&	GetCases() const { return m_Cases; }
	COptionsCases&			GetCases() { return m_Cases; }
#if (0)
	const COptionsView&		GetView() const { return m_View; }
	COptionsView&			GetView() { return m_View; }
#endif

	bool		IsAcceptPackets() const { return m_Cases.IsAcceptPackets(); }
	bool		IsLogMessages() const { return m_Cases.IsLogMessages(); }
	bool		IsProtocolOfInterest( CProtocolType inProtocolType ) const {
		return m_Cases.IsProtocolOfInterest( inProtocolType );
	}
	bool		IsSlicePackets() const { return m_Cases.IsSlicePackets(); }
	bool		IsSummaryStats() const { return m_Cases.IsSummaryStats(); }

	bool		Log( const CPeekStringA& inLabel, const CPeekString& inChangeLogFileName );
	bool		LogUpdate( const CPeekStringA& inLabel, const COptions& inOptions,
					const CPeekString& inChangeLogFileName );

	bool		Model( CPeekDataElement& ioPrefs );
	bool		Model( CPeekDataModeler& ioPreferences );

	void		Reset() {
		m_Cases.Reset();
	}
};


// ============================================================================
//	COptionsPtr
// ============================================================================

class COptionsPtr
	:	public TLockedPtr<COptions>
{
public:
	;	COptionsPtr( COptions* inOptions ) : TLockedPtr<COptions>( inOptions ) {};
};


// ============================================================================
//	CSafeOptions
// ============================================================================

class CSafeOptions
{
protected:
	COptions	m_Options;

public:
	;		CSafeOptions() {}
	;		CSafeOptions( COptions* inOther ) {
		_ASSERTE( inOther );
		m_Options.Lock();
		m_Options.Copy( *inOther );
		m_Options.Unlock();
	}

	COptionsPtr	Get() { return COptionsPtr( &m_Options ); }

	bool		Duplicate( COptions* outOther ) const {
		m_Options.Lock();
		*outOther = m_Options;
		m_Options.Unlock();
		return true;
	};
};
