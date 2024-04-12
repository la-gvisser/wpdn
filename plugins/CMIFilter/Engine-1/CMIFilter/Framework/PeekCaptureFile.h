// ============================================================================
//	PeekCaptureFile.h
//		interface for CPeekCaptureFile class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "tagfile.h"
#include "MediaTypes.h"
#include "MemUtil.h"
#include "CaptureFile.h"
#include "GPSInfo.h"

using namespace CaptureFile;
using namespace MemUtil;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Capture File
//
//	A Peek Capture File support reading and writing of OmniPeek format files.
//	Peek files have the extension: pkt.
//
//	Peek Capture File is part of the Capture File module.
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#define	BLOCK_FLAGS(c,f)	((UInt32)(c&0xFF)|((f&0xFF)<<8))
#define BLOCK_COMP(f)		((UInt32)(f&0xFF))
#define BLOCK_FORMAT(f)		((UInt32)((f&0x0000FF00)>>8))


// ============================================================================
//		CBlock
// ============================================================================

class CBlock
{
protected:
	UInt32		m_nType;
	UInt32		m_nFlags;
	size_t		m_nLength;
	CByteVector	m_bvData;

public:
	static void		Parse( size_t inLength, const byte* inData,
		const CAttributeIdList& inIds, CBlockAttributeList& outAttribs );
	static void		Parse( const CByteVector& inData, const CAttributeIdList& inIds,
		CBlockAttributeList& outAttribs ) {
		if ( inData.size() > 0 ) {
			Parse( inData.size(), &inData[0], inIds, outAttribs );
		}
	}
	static void		ParseList( const std::string& inData, UInt16 inId, CBlockAttributeList& outAttribs );

public:
	;		CBlock() : m_nType( 0 ), m_nFlags( 0 ), m_nLength( 0 ) {}
	;		CBlock( const CBlock& inOther, bool inCopyData = false )
		:	m_nType( inOther.m_nType )
		,	m_nFlags( inOther.m_nFlags )
		,	m_nLength( inOther.m_nLength )
	{
		if ( inCopyData ) {
			copy( inOther.m_bvData.begin(), inOther.m_bvData.end(), m_bvData.begin() );
		}
	}
	virtual	~CBlock() {}

	const CByteVector&	GetData() const { return m_bvData; }
	CByteVector&		GetData() { return m_bvData; }
	UInt32				GetFlags() const { return m_nFlags; }
	size_t				GetLength() const { return m_nLength; }
	UInt32				GetType() const { return Swap32( m_nType ); }

	virtual bool	IsValid() const { return (m_nType != 0); }

	virtual size_t	Load( size_t /*inLength*/, TaggedFile::BLOCK_HEADER* /*inHeader*/ ) { return 0; }

	virtual bool	Read( COSFile& inFile );
	virtual bool	ReadData( COSFile& inFile );

	virtual size_t	Store( CByteVector& /*outData*/ ) { return 0; }
};


// ============================================================================
//		CFileBlock
// ============================================================================

class CFileBlock
	: public	CBlock
{
protected:
	UInt64	m_nOffset;

public:
	;		CFileBlock() : m_nOffset( OFFSET_EOF ) {}
	;		CFileBlock( const CBlock& inOther, bool inCopyData = false )
		:	CBlock( inOther, inCopyData )
		,	m_nOffset( OFFSET_EOF ) {}
	;		CFileBlock( const CFileBlock& inOther, bool inCopyData = false )
		:	CBlock( inOther, inCopyData )
		,	m_nOffset( inOther.m_nOffset ) {}

	UInt64	GetOffset() const { return m_nOffset; }

	void	SetOffset( UInt64 inOffset ) { m_nOffset = inOffset; }
};


// ============================================================================
//		CVersionBlock
// ============================================================================

class CVersionBlock
	:	public CFileBlock
{
public:
	class CIdList : public CAttributeIdList { public: CIdList(); };
	static CIdList	s_AttribIds;

protected:
	UInt32			m_nVersion;
	tVersion		m_AppVersion;
	tVersion		m_ProductVersion;

protected:
	void	Build( CBlockAttributeList& outAttribs );

	void	Parse( size_t inLength, const byte* inData );
	void	Parse( const CByteVector& inData ) { Parse( inData.size(), &inData[0] ); }

public:
	;		CVersionBlock();
	;		CVersionBlock( const CBlock& inBlock );

	std::wstring	FormatAppVersion() const { return m_AppVersion.Format(); }
	std::string		FormatAppVersionA() const { return m_AppVersion.FormatA(); }
	std::wstring	FormatProductVersion() const { return m_ProductVersion.Format(); }
	std::string		FormatProductVersionA() const { return m_ProductVersion.FormatA(); }

	tVersion		GetAppVersion() const { return m_AppVersion; }
	UInt32			GetFileVersion() const { return m_nVersion; }
	tVersion		GetProductVersion() const { return m_ProductVersion; }

	virtual bool	IsValid() const {
		return (GetType() == TaggedFile::kCaptureFileVersionBlock) && (m_nVersion != 0);
	}

	virtual size_t	Load( size_t inLength, const TaggedFile::BLOCK_HEADER* inHeader );

	bool			ReadData( COSFile& inFile );

	void			SetAppVersion( const tVersion& inVersion ) { m_AppVersion = inVersion; }
	void			SetFileVersion( UInt32 inVersion ) { m_nVersion = inVersion; }
	void			SetProductVersion( const tVersion& inVersion ) { m_ProductVersion = inVersion; }
	virtual size_t	Store( CByteVector& outData );

	bool			Write( COSFile& inFile );
};


// ============================================================================
//		CSessionBlock
// ============================================================================

class CSessionBlock
	:	public CFileBlock
{
public:
	class CIdList : public CAttributeIdList { public: CIdList(); };
	class CChannelIdList : public CAttributeIdList { public: CChannelIdList(); };
	static CIdList				s_AttribIds;
	static CChannelIdList		s_ChannelAttribIds;

	typedef struct {
		UInt32	fNumber;
		UInt32	fFrequency;
		UInt32	fBand;
	} tChannel;

protected:
	CFileTime	m_RawTime;
	std::string	m_Time;
	UInt32		m_TimeZoneBias;
	tMediaType	m_MediaType;
	UInt32		m_LinkSpeed;
	UInt32		m_PacketCount;
	std::string	m_Comment;
	CFileTime	m_SessionStartTime;
	CFileTime	m_SessionEndTime;
	std::string	m_AdapterName;
	std::string	m_AdapterAddr;
	std::string	m_CaptureName;
	std::string	m_CaptureId;
	std::string	m_Owner;
	UInt32		m_FileIndex;
	std::string	m_Host;
	std::string	m_EngineName;
	std::string	m_EngineAddr;
	std::string	m_MediaDomain;

	std::vector<UInt32>		m_ayRates;
	std::vector<tChannel>	m_ayChannels;

protected:
	void	Build( CBlockAttributeList& outAttribs );
	bool	BuildChannelList( std::string& outChannelList );
	bool	BuildRateList( std::string& outRateList );

	void	Parse( size_t inLength, const byte* inData );
	void	Parse( const CByteVector& inData ) {
		Parse( inData.size(), &inData[0] );
	}
	bool	ParseChannel( const std::string& inChannel, tChannel& outChannel );
	void	ParseChannelList( const std::string& inChannels );
	void	ParseRateList( const std::string& inRates );

public:
	;		CSessionBlock( tMediaType inType );
	;		CSessionBlock( const CBlock& inBlock );

	void			AddPacket( UInt64 inTimeStamp, size_t inPacketSize ) {
		m_PacketCount++;
		if ( m_SessionStartTime == 0 ) {
			m_SessionStartTime = inTimeStamp;
		}
		m_SessionEndTime = inTimeStamp + ((inPacketSize * 8) * (m_LinkSpeed * 1000));
	}

	std::string		GetAdapterAddress() const { return m_AdapterAddr; }
	std::string		GetAdapterName() const { return m_AdapterName; }
	std::string		GetCaptureId() const { return m_CaptureId; }
	std::string		GetCaptureName() const { return m_CaptureName; }
	const std::vector<tChannel>&	GetChannelList() const { return m_ayChannels; }
	std::string		GetComment() const { return m_Comment; }
	const std::vector<UInt32>&	GetDataRates() const { return m_ayRates; }
	const CFileTime&	GetEndTime() const { return m_SessionEndTime; }
	std::string		GetEngineAddress() const { return m_EngineAddr; }
	std::string		GetEngineName() const { return m_EngineName; }
	UInt32			GetFileIndex() const { return m_FileIndex; }
	std::string		GetHost() const { return m_Host; }
	UInt32			GetLinkSpeed() const { return m_LinkSpeed; }
	std::string		GetMediaDomain() const { return m_MediaDomain; }
	tMediaType		GetMediaType() const { return m_MediaType; }
	std::string		GetOwner() const { return m_Owner; }
	UInt32			GetPacketCount() const { return m_PacketCount; }
	const CFileTime&	GetRawTime() const { return m_RawTime; }
	const CFileTime&	GetStartTime() const { return m_SessionStartTime; }
	std::string		GetTime() const { return m_Time; }
	UInt32			GetTimeZoneBias() const { return m_TimeZoneBias; }

	void			Init() {
		m_PacketCount = 0;
		m_SessionStartTime = 0;
		m_SessionEndTime = 0;
	}
	virtual bool	IsValid() const {
		return (GetType() == TaggedFile::kCaptureFileSessionBlock);
	}

	virtual size_t	Load( size_t inLength, const TaggedFile::BLOCK_HEADER* inHeader );

	bool			ReadData( COSFile& inFile );

	void			SetAdapterAddress( const std::string& inAddress ) { m_AdapterAddr = inAddress; }
	void			SetAdapterName( const std::string& inName ) { m_AdapterName = inName; }
	void			SetCaptureId( const std::string& inId ) { m_CaptureId = inId; }
	void			SetCaptureName( const std::string& inName ) { m_CaptureName = inName; }
	void			SetChannelList( const std::vector<tChannel>& inChannels ) { m_ayChannels = inChannels; }
	void			SetComment( const std::string& inComment ) { m_Comment = inComment; }
	void			SetDataRates( const std::vector<UInt32>& inRates ) { m_ayRates = inRates; }
	void			SetEndTime( const CFileTime& inTime ) { m_SessionEndTime = inTime; }
	void			SetEngineAddress( const std::string& inAddress ) { m_EngineAddr = inAddress; }
	void			SetEngineName(const std::string& inName ) { m_EngineName = inName; }
	void			SetFileIndex( UInt32 inIndex ) { m_FileIndex = inIndex; }
	void			SetHost( const std::string& inHost ) { m_Host = inHost; }
	void			SetLinkSpeed( UInt32 inSpeed ) { m_LinkSpeed = inSpeed; }
	void			SetMediaDomain( const std::string& inDomain ) { m_MediaDomain = inDomain; }
	void			SetMediaType( tMediaType inType );
	void			SetOwner( const std::string& inOwner ) { m_Owner = inOwner; }
	void			SetPacketCount( UInt32 inCount ) { m_PacketCount = inCount; }
	void			SetRawTime( UInt64 inTime ) { m_RawTime = inTime; }
	void			SetStartTime( const CFileTime& inTime ) { m_SessionStartTime = inTime; }
	void			SetTime( const std::string& inTime ) { m_Time = inTime; }
	void			SetTimeZoneBias( UInt32 inBias ) { m_TimeZoneBias = inBias; }
	virtual size_t	Store( CByteVector& outData );

	bool			Write( COSFile& inFile );

};


// ============================================================================
//		CFileIdBlock
// ============================================================================

class CFileIdBlock
	:	public CFileBlock
{
	class CIdList : public CAttributeIdList { public: CIdList(); };
	static CIdList	s_AttribIds;

protected:
	std::string		m_strId;
	UInt32			m_nIndex;

protected:
	void	Build( CBlockAttributeList& outAttribs );

	void	Parse( size_t inLength, const byte* inData );
	void	Parse( const CByteVector& inData ) {
		Parse( inData.size(), &inData[0] );
	}

public:
	;		CFileIdBlock() : m_nIndex( 0 ) {}
	;		CFileIdBlock( const CBlock& inBlock );

	const std::string&	GetId() const { return m_strId; }
	UInt32				GetIndex() const { return m_nIndex; }

	virtual bool	IsValid() const {
		return (GetType() == TaggedFile::kCaptureFileIdBlock);
	}

	virtual size_t	Load( size_t inLength, const TaggedFile::BLOCK_HEADER* inHeader );

	bool			ReadData( COSFile& inFile );

	void			SetId( const std::string& inId ) { m_strId = inId; }
	void			SetIndex( UInt32 inIndex ) { m_nIndex = inIndex; }
	virtual size_t	Store( CByteVector& outData );

	bool			Write( COSFile& inFile );
};


// ============================================================================
//		CGPSBlock
// ============================================================================

class CGPSBlock
	:	public CFileBlock
{
protected:
	CGPSInfoList	m_ayGPSInfo;

protected:
	void	Build( CByteVector& outData );

public:
	;		CGPSBlock() {}
	;		CGPSBlock( const CBlock& inBlock );

	const CGPSInfoList&	GetGPSInfoList() const { return m_ayGPSInfo; }

	virtual bool	IsValid() const {
		return (GetType() == TaggedFile::kCaptureFileGpsBlock);
	}

	virtual size_t	Load( size_t inLength, const TaggedFile::BLOCK_HEADER* inHeader );

	bool			ReadData( COSFile& inFile );

	void			SetGPSInfoList( const CGPSInfoList& inInfoList ) { m_ayGPSInfo = inInfoList; }
	virtual size_t	Store( CByteVector& outData );

	bool			Write( COSFile& inFile );
};


// ============================================================================
//		CPacketsBlock
// ============================================================================

class CPacketsBlock
	:	public CFileBlock
{
#if (0)
public:
	static size_t	PacketLength( const byte* inData );
#endif

#if (0)
protected:
	size_t			LoadPackets( tMediaType inType, size_t inLength, const byte* ipData );
#endif

public:
	;		CPacketsBlock() {}
	;		CPacketsBlock( const CBlock& inBlock, bool inCopyData = true );
	;		CPacketsBlock( const CPacketsBlock& inBlock );

	virtual bool	IsValid() const {
		return (GetType() == TaggedFile::kCaptureFilePacketsBlock);
	}

	virtual size_t	Load( tMediaType inType, size_t inLength, const TaggedFile::BLOCK_HEADER* inHeader );

	bool			ReadData( COSFile& inFile );

	virtual size_t	Store( CByteVector& outData );

	bool			Write( COSFile& inFile );
};


// ============================================================================
//		CPeekCapture
// ============================================================================

class CPeekCapture
{
protected:
	CVersionBlock	m_VersionBlock;
	CSessionBlock	m_SessionBlock;
	CFileIdBlock	m_FileIdBlock;
	CGPSBlock		m_GPSBlock;
	CPacketsBlock	m_PacketsBlock;

public:
	;		CPeekCapture( tMediaType inMediaType = MT_ETHERNET )
		:	m_SessionBlock( inMediaType )
	{}
	virtual	~CPeekCapture() {}

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		if ( inOther.GetFileFormat() == CF_FORMAT_PEEK ) {
			const CPeekCapture*	pOther = dynamic_cast<const CPeekCapture*>( &inOther );
			if ( pOther ) {
				(void)pOther;
			}
		}
	}

	const CFileIdBlock&		GetFileIdBlock() const { return m_FileIdBlock; }
	const CGPSBlock&		GetGPSBlock() const { return m_GPSBlock; }
	virtual tMediaType		GetMediaType() const {
		return m_SessionBlock.GetMediaType();
	}
	const CPacketsBlock&	GetPacketsBlock() const { return m_PacketsBlock; }
	const CSessionBlock&	GetSessionBlock() const { return m_SessionBlock; }
	const CVersionBlock&	GetVersionBlock() const { return m_VersionBlock; }

	static bool	IsKind( const std::wstring& inKind ) {
		return (inKind.compare( L"pkt" ) == 0);
	}

#if (0)
	int		Load( CByteVector& outData );
	int		Store( CByteVector& outData );
#endif
};


// ============================================================================
//		CPeekCaptureReader
// ============================================================================

class CPeekCaptureReader
	:	public CCaptureReader
	,	public CPeekCapture
{
protected:
	CFileOffsets	m_ayOffsets;

protected:
	void	Parse( bool inScanPackets );

	bool	SeekToPacket( UInt32 inIndex );

public:
	;		CPeekCaptureReader()
		:	CCaptureReader( CF_FORMAT_PEEK )
	{}
	virtual	~CPeekCaptureReader() {}

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		CPeekCapture::CopyMetaData( inOther );
	}

	virtual tMediaType	GetMediaType() const {
		return CPeekCapture::GetMediaType();
	}
	UInt32				GetPacketCount() const {
		size_t	nHighest = m_ayOffsets.GetHighest() + 1;
		_ASSERTE( nHighest < kMaxUInt32 );
		if ( nHighest >= kMaxUInt32 ) return kMaxUInt32;

		return (m_SessionBlock.GetPacketCount())
			? m_SessionBlock.GetPacketCount()
			: static_cast<UInt32>( nHighest );
	}

	static bool	IsKind( const std::wstring& inKind ) {
		return CPeekCapture::IsKind( inKind );
	}

	bool	Open( const std::wstring& inFileName, bool inScanPackets );
	bool	Open( const wchar_t* inFileName, bool inScanPackets ) {
		return Open( std::wstring( inFileName ), inScanPackets );
	}
	bool	Open( const std::string& inFileName, bool inScanPackets ) {
		return Open( AtoW( inFileName ), inScanPackets );
	}
	bool	Open( const char* inFileName, bool inScanPackets ) {
		return Open( AtoW( inFileName ), inScanPackets );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32 inIndex );
	virtual bool	ReadFilePackets( CFilePacketList& outPackets, 
		UInt32 inCount, UInt32 inFirst = NEXT_PACKET );
};


// ============================================================================
//		CPeekCaptureWriter
// ============================================================================

class CPeekCaptureWriter
	:	public CCaptureWriter
	,	public CPeekCapture
{
protected:
#if (0)
	virtual void	Init();
#endif

public:
	;		CPeekCaptureWriter( int inAccess, tMediaType inType )
		:	CCaptureWriter( CF_FORMAT_PEEK, inAccess )
		,	CPeekCapture( inType )
	{}
	;		~CPeekCaptureWriter() { CCaptureWriter::Close(); }

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		CPeekCapture::CopyMetaData( inOther );
	}
	virtual void	Close();

	virtual tMediaType	GetMediaType() const {
		return CPeekCapture::GetMediaType();
	}
	UInt32				GetPacketCount() const {
		return m_SessionBlock.GetPacketCount();
	}

	virtual void	Init( const CCaptureFile* inTemplate = nullptr );
	static bool		IsKind( const std::wstring& inKind ) {
		return CPeekCapture::IsKind( inKind );
	}

	bool	Open( const std::wstring& inFileName );
	bool	Open( const wchar_t* inFileName ) {
		return Open( std::wstring( inFileName ) );
	}
	bool	Open( const char* inFileName ) {
		return Open( AtoW( inFileName ) );
	}
	bool	Open( const std::string& inFileName ) {
		return Open( AtoW( inFileName ) );
	}

	void	SetFileIdBlock( const CFileIdBlock& inFileId ) { m_FileIdBlock = inFileId; }
	void	SetPacketsBlock( const CPacketsBlock& inPackets ) { m_PacketsBlock = inPackets; }
	void	SetSessionBlock( const CSessionBlock& inSession ) {
		m_SessionBlock = inSession;
		m_SessionBlock.SetPacketCount( 0 );
	}
	void	SetVersionBlock( const CVersionBlock& inVersion ) { m_VersionBlock = inVersion; }

	virtual bool	WriteFilePacket( const CFilePacket& inPacket );
};

}	// namespace CaptureFile
