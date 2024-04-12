// ============================================================================
//	MsgStream.h
//		interface for the CMsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "MsgAddress.h"
#include "MsgPacket.h"
#include "PenInfo.h"
#include "PeekFile.h"
#include "FileMap.h"
#include "ProtocolType.h"
#include "LockedPtr.h"
#include "PeekStream.h"
#include "PeekTime.h"
#include <vector>
#include <map>

#ifdef HE_LINUX
#include <stdio.h>
#include <sys/stat.h>
#endif

class CCase;
class COptions;
class CReportMode;
class CMsgStreamList;
class COmniEngineContext;


// ============================================================================
//		CMsgStream
// ============================================================================

class CMsgStream
{
public:
	typedef enum {
		kMsgState_Open,
		kMsgState_Sender,
		kMsgState_Recipients,
		kMsgState_Headers,
		kMsgState_Text,
		kMsgState_BDAT,
		kMsgState_Xexch50Post,
		kMsgState_Xexch50,
		kMsgState_Closed
	} kMsgStates;

	typedef enum {
		kMsgFormat_Unknown,
		kMsgFormat_Text,
		kMsgFormat_Binary
	} kMsgFormats;

	typedef enum {
		kMsgBlock_Unknown,
		kMsgBlock_Text,
		kMsgBlock_Binary,
		kMsgBlock_Xexch50
	} kMsgBlock;

	class CBinaryBlock
	{
	protected:
		bool	m_bLast;
		UInt32	m_nSequenceStart;
		UInt32	m_nSize;
		UInt32	m_nReceived;

	public:
		;		CBinaryBlock() : m_bLast( false ), m_nSequenceStart( 0 ), m_nSize( 0 ), m_nReceived( 0 ) {}
		void	Cancel() {
			m_nSize = m_nReceived;
		}
		UInt32	GetSequenceStart() const { return m_nSequenceStart; }
		UInt32	GetSequenceEnd() const { return m_nSequenceStart + m_nSize; }
		UInt32	GetSize() const { return m_nSize; }
		bool	IsLast() const { return m_bLast; }
		void	Start( UInt32 inLength, UInt32 inStart, UInt32 inSize, bool inLast ) {
			m_nSequenceStart = inStart;
			m_nSize = inLength + inSize;
			m_bLast = inLast;
			m_nReceived = inLength;
		}
		bool	Update( UInt32 inSize, UInt32 inSequence ) {
			if ( inSequence < GetSequenceEnd() ) {
				m_nReceived += inSize;
				return true;
			}
			return false;
		}
	};

	class CStreamFileItem
	{
	protected:
		tFileNameType	m_nType;
		CPeekString		m_strFileName;
		CFileEx			m_hFile;

	public:
		;	CStreamFileItem( tFileNameType inType ) : m_nType( inType ) {};
		CFileEx&		GetFile() { return m_hFile; }
		CPeekString		GetName() const { return m_strFileName; }
		tFileNameType	GetType() { return m_nType; }
		bool			IsNull() { return (GetType() == kFileTypeMax); }
		BOOL			Open( int inFlags ) { return m_hFile.Open( m_strFileName, inFlags ); }
		void			SetName( const CPeekString& inName ) { m_strFileName = inName; }
	};
	static CStreamFileItem	s_NullFileItem;

	class CStreamFileGroup
		:	public std::vector<CStreamFileItem>
	{
	public:
		;		CStreamFileGroup() {}
		void	DeleteOpen() {
			for ( size_t i = 0; i < size(); i++ ) {
				CStreamFileItem&	item( at( i ) );
				CFileEx&	hFile( item.GetFile() );
				if ( hFile.IsOpen() ) {
#if (TOVERIFY)
					UInt64	nSize = hFile.GetLength();
					hFile.Close();
					if ( nSize == 0 ) {
#ifdef HE_LINUX
						CPeekStringA	strName( item.GetName() );
						std::remove( strName );
#else
						::DeleteFile( item.GetName() );
#endif
					}
#endif // TOVERIFY
				}
			}
		}
		bool Exists() const {
			for ( size_t i = 0; i < size(); i++ ) {
#ifdef HE_LINUX
				CPeekStringA	strName( at( i ).GetName() );
				struct stat buffer;   
				return (stat( strName.c_str(), &buffer) == 0);
#else
				if ( ::PathFileExists( at( i ).GetName() ) ) {
					return true;
				}
#endif
			}
			return false;
		}
		CStreamFileItem&	Find( tFileNameType inType ) {
			for ( size_t i = 0; i < size(); i++ ) {
				if ( at( i ).GetType() == inType ) {
					return at( i );
				}
			}
			return s_NullFileItem;
		}
	};

protected:
	tProtocolTypes		m_ProtocolType;
	CMsgStreamList*		m_pMsgStreamList;
	COmniEngineContext&	m_Context;
	size_t				m_nTargetIndex;
	CPeekStringA		m_strIdA;

	kMsgStates			m_MsgState;
	kMsgFormats			m_MsgFormat;
	kMsgBlock			m_MsgBlock;
	CBinaryBlock		m_Xexch50Block;
	CBinaryBlock		m_BdatBlock;
	bool				m_bOpen;
	bool				m_bEOMReceived;
	CIpAddressPortPair	m_IpPortPair;
	CPeekTime			m_LastPacketTime;
	CMsgText			m_LastBody;

	bool				m_bDirty;
	bool				m_bVerified;
	CMsgPacketList		m_MsgPacketList;
	CPeekStringA		m_strMsg;
	CPeekStream			m_bsMsg[2];
	CMsgText			m_strPenMsg;

	CPenInfoPtr			m_PenInfoPtr;

	// A Message Stream has 3 Parts (first 2 are optional)
	// 1. Responses: +ok
	// 2. Commands: MAIL, RCPT, DATA (ends with DATA)
	// 3. Message
	//    a. Headers (ends with \r\n\r\n)
	//    b. Body    (ends with \r\n.\r\n)

	size_t				m_nCommandsIndex;	// Commands are saved in Full captures
	size_t				m_nMessageIndex;
	size_t				m_nBodyIndex;
	size_t				m_nEndIndex;

	CPeekStringA		m_strBoundary;	// MIME multi-part boundary string

	virtual CPeekStringA&	GetMsgStringA() = 0;
	virtual bool			VerifyMsg() = 0;
	CPenInfo*				GetPenInfo();

	bool				OpenCaseFiles( const CPeekString& inPrefix, const CCase& inCase, CStreamFileGroup& ioFiles );
	bool				OpenCaseFiles( const CPeekString& inPrefix, const CPeekString& inEmailAddress,
		const CCase& inCase, CStreamFileGroup& ioFiles )
	{
		CPeekString	strPrefix( inPrefix );
		strPrefix.Append( inEmailAddress );
		strPrefix.Append( L"_" );
		return OpenCaseFiles( strPrefix, inCase, ioFiles );
	}

	const CPeekStringA&	GetBoundaryStringA();
	size_t				NextBoundaryOffset( size_t inPosition, size_t* outBoundaryBegins );
	bool				WriteString( const CPeekString& inFileName );
	bool				WriteString( CFileEx& inFile );

public:
	;		CMsgStream( tProtocolTypes inType, const CIpAddressPortPair& inIpPortPair,
				COmniEngineContext& inContext, size_t inIndex, const CMsgAddress& inTarget, kMsgStates inState );
	;		CMsgStream( tProtocolTypes inType, const CIpAddressPortPair& inIpPortPair,
				COmniEngineContext& inContext, size_t inIndex, kMsgStates inState );
	virtual	~CMsgStream() {}

	bool					operator<( const CMsgStream& inOther ) {
		return (m_IpPortPair < inOther.m_IpPortPair);
	}

	virtual void			Analyze( const CCase& inCase ) = 0;
	virtual void			Analyze( const CCase& inCase, const CTargetAddress& inTargetAddress ) = 0;

	size_t					CalculateMaxLength( MsgDirection inDirection );

	CPeekString				ErrorFileName( const CPeekString& inType, const CPeekString& inOutputDirectory );

	bool					GetAttachmentText( size_t& ioPosition, CPeekStringA& outAttachment );
	const CPeekStringA&		GetIdA() const { return m_strIdA; }
	void					GetIpAndPorts( CIpAddressPortPair& outIpPortPair ) const { outIpPortPair = m_IpPortPair; }
	CPeekTime				GetLastPacketTime() { return m_LastPacketTime; }
	const CMsgPacketList&	GetMsgList() { return m_MsgPacketList; }
	CPeekStream&			GetByteStream( MsgDirection inDirection );
	CPeekStringA			GetPayload();
	const CPeekStringA&		GetPenMsg();
	bool					GetMsgSizeInfo( tMsgSizeInfo& outSizeInfo );
	virtual tProtocolTypes	GetProtocolType() { return m_ProtocolType; }
	virtual CPeekString		GetProtocol() { return CProtocolType::Format( m_ProtocolType ); }
	kMsgStates				GetState() const { return m_MsgState; }

	virtual bool	HasTarget( const CTargetAddress& inTargetAddress );

	bool			IsEqual( const CIpAddressPortPair& inIpPortPair ) const { return (m_IpPortPair == inIpPortPair); }
	//bool			IsInverse( const CIpAddressPortPair& inIpPortPair ) const { return m_IpPortPair.IsInverse( inIpPortPair ); }
	bool			IsFormatBinary() const { return (m_MsgFormat == kMsgFormat_Binary); }
	bool			IsOpen() const { return m_bOpen; }
	bool			IsEOMReceived() const { return m_bEOMReceived; }
	virtual bool	IsValid() = 0;

	const CIpAddressPortPair& Key() const { return m_IpPortPair; }

	void	NoteFileError( const CPeekString& inFileName );
	void	NoteWriteError( const CPeekString& inFileName );

	void	SetMsgFormat( kMsgFormats inFormat ) { m_MsgFormat = inFormat; }
	void	SetMsgStreamList( CMsgStreamList* inMsgStreamList ){ m_pMsgStreamList = inMsgStreamList; }
	void	SetMsgState( kMsgStates inState ) { m_MsgState = inState; }

	CMsgPacket*	TestAndAddMsg( CMsgPacketPtr& inMsgPacket );

	bool	WritePackets( const CPeekString& inFileName );
	//bool	WritePackets( CFileEx& inFile );
	bool	WritePayload( const CPeekString& inFileName );
	bool	WritePayload( CFileEx& inFile );
};


// ============================================================================
//		TStreamList
// ============================================================================

#if (TOVERIFY)

template< typename S >
class TStreamList
{
protected:
	class CMap
		:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
		,	public std::map<CIpAddressPortPair, S*>
	{
	public:
		;		CMap( UInt32 nBins )
			:	std::map<CIpAddressPortPair, S*>()
		{ (void) nBins; }
		;		~CMap() { Reset(); }
		void	Reset() {
			auto	item( this->begin() );
			auto	last( this->end() );
			while ( item != last ) {
				delete item->second;
				item->second = NULL;
				++item;
			}
			this->clear();
		}
	};

protected:
	CMap		m_Map;

public:
	;			TStreamList();
	virtual		~TStreamList();

	bool		Add( S* inMsgStream );
	S*			Find( const CIpAddressPortPair& inIpPortPair );
	size_t		GetCount() const { return m_Map.size(); }
	void		Lock() { m_Map.Lock(); }
	void		Monitor( CPeekDuration pdTimeOut, CArrayString& outRemovedStreams );
	void		Remove( S* inMsgStream ) { m_Map.erase( inMsgStream->Key() ); }
	void		RemoveAndDelete( S* inMsgStream ) {
		Remove( inMsgStream );
		delete inMsgStream;
	}
	void		Reset() { m_Map.Reset(); }
	void		Unlock() { m_Map.Unlock(); }
};


// ----------------------------------------------------------------------------
//		TStreamList
// ----------------------------------------------------------------------------

template< typename S >
inline TStreamList< S >::TStreamList()
	:	m_Map( 8209 )
{
}

template< typename S >
inline TStreamList< S >::~TStreamList()
{
}


// ----------------------------------------------------------------------------
//		Add
// ----------------------------------------------------------------------------

template< typename S >
bool
TStreamList< S >::Add(
	S*	inMsgStream )
{
	if ( inMsgStream == NULL ) return false;

	m_Map[inMsgStream->Key()] = inMsgStream;
	inMsgStream->SetMsgStreamList( reinterpret_cast<CMsgStreamList*>( this ) );
	return true;
}


// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

template< typename S >
S*
TStreamList< S >::Find(
	const CIpAddressPortPair&	inIpPortPair )
{
	// Lock

#ifdef DEBUG
	const size_t	nCount = m_Map.size();
	if ( nCount == 0 ) return false;
	CPeekString		strSource = inIpPortPair.GetSrcAddress().Format() + L":" + inIpPortPair.GetSrcPort().Format();
	CPeekString		strDestination = inIpPortPair.GetDstAddress().Format() + L":" + inIpPortPair.GetDstPort().Format();
#endif

	auto	pItem( m_Map.find( inIpPortPair ) );
	if ( pItem != m_Map.end() ) {
		return pItem->second;
	}

	// Can't use CIpAddressPortPair.Inverse().
	CIpAddressPortPair	iappInverse( inIpPortPair.GetDestination(), inIpPortPair.GetSource() );

#ifdef DEBUG
	CPeekString	strSrcInv = iappInverse.GetSrcAddress().Format() + L":" + iappInverse.GetSrcPort().Format();
	CPeekString	strDstInv = iappInverse.GetDstAddress().Format() + L":" + iappInverse.GetDstPort().Format();
#endif

	pItem = m_Map.find( iappInverse );
	if ( pItem != m_Map.end() ) {
		return pItem->second;
	}

	return NULL;
}


// ----------------------------------------------------------------------------
//		Monitor
// ----------------------------------------------------------------------------

template< typename S >
void TStreamList< S >::Monitor(
	CPeekDuration	pdTimeOut,
	CArrayString&	outRemovedStreams )
{
	std::vector<CIpAddressPortPair>	ayMorgue;

	CPeekTime		ptCurTime( true );
	for ( auto pos = m_Map.begin(); pos != m_Map.end(); pos++ ) {
		CIpAddressPortPair	ippKey( pos->first );
		S*					pMsgStream( pos->second );

		CPeekDuration	pdDelta( ptCurTime - pMsgStream->GetLastPacketTime() );
		bool			bRemove( pdDelta > pdTimeOut );
		if ( bRemove ) {
			CPeekStringA	strRemoved( pMsgStream->GetIdA() );
			outRemovedStreams.push_back( CPeekString( strRemoved ) );
			ayMorgue.push_back( ippKey );
			delete pMsgStream;
			pos->second = NULL;
		}
	}

	const size_t	nCount( ayMorgue.size() );
	for ( size_t i = 0; i < nCount; i++ ) {
		m_Map.erase( ayMorgue[i] );
	}

	return;
}

#endif // TOVERIFY
