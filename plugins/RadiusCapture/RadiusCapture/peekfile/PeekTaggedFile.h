// =============================================================================
//	PeekTaggedFile.h
// =============================================================================
//	Copyright (c) 2003-2008 WildPackets, Inc. All rights reserved.

#ifndef PEEKTAGGEDFILE_H
#define PEEKTAGGEDFILE_H

#include <time.h>
#include "MediaTypes.h"
#include "PeekPacket.h"
#include "ByteStream.h"
#include "TagFile.h"
#include "MemUtil.h"
#include "ArrayEx.h"
#include "Environment.h"
#include "StreamOnCByteStream.h"
#include "ChannelMgr.h"

#ifdef OPT_USE_ZLIB
#define ZLIB_DLL
#include "zlib.h"
#endif /* OPT_USE_ZLIB */

#define OPT_OUTPUTFILESTREAM
class COutputFileStream;


// =============================================================================
//	CCaptureFileWriter
// =============================================================================

class CCaptureFileWriter
{
public:
					CCaptureFileWriter( LPCTSTR lpszFile = NULL, bool bCompressed = false);
	virtual			~CCaptureFileWriter();

	bool			Open( LPCTSTR lpszFile, bool bCompressed = false );
	bool			Close();
	bool			Delete();

	bool			IsOpen() const;
	UInt32			GetSize() const;
	CString			GetFileName() const;
	HANDLE			GetFileHandle() const;
	DWORD			GetLastError() const { return m_nResult; }

	virtual bool	WriteBytes( const void* pData, UInt32 nLength );	// virtual for OPT_INTEGRITY
	bool			WriteHeader( const TaggedFile::BLOCK_HEADER &blockHeader );
	bool			WriteHeader( TaggedFile::BlockType nType, UInt32 nLength, UInt32 nFlags );

	DWORD			GetFilePosition() const;
	bool			SetFilePosition( DWORD dwPos, DWORD positionFlags ) const;

	bool			Flush();

protected:
#ifdef OPT_USE_ZLIB
	gzFile				m_hCompressedFile;
#endif /* OPT_USE_ZLIB */
#ifdef OPT_OUTPUTFILESTREAM
	COutputFileStream*	m_pOutputFileStream;
#else
	HANDLE				m_hUncompressedFile;
#endif /* OPT_OUTPUTFILESTREAM */
	CString				m_strFileName;
	DWORD				m_nResult;
};


// =============================================================================
//	CCaptureFileReader
// =============================================================================

class CCaptureFileReader
{
public:
					CCaptureFileReader( LPCTSTR lpszFile = NULL, bool bIsCompressed = false);
	virtual			~CCaptureFileReader();

	bool			Open( LPCTSTR lpszFile, bool bIsCompressed );
#ifdef OPT_USE_ZLIB
	bool			AttachCompressed( gzFile hCompressedFile, bool bOwn = false );
#endif
	bool			AttachFile( HANDLE hFile, bool bOwn = false );
	bool			AttachMemMap( const UInt8* pMemMappedFile, UInt32 nLength, bool bOwn = false );
	const UInt8*	GetMemoryMap() const {	return m_pMemMappedFile;	}
	void			OwnFile( bool bOwn = true );
	bool			Close();

	bool			IsTaggedFile() const;
#ifdef OPT_USE_ZLIB
	bool			IsCompressed() const { return (m_hCompressedFile != NULL); }
#endif /* OPT_USE_ZLIB */

	bool			IsOpen() const;
	DWORD			GetLastError() const { return m_nResult; }
	UInt32			GetSize() const { return m_nLength; }

	bool			IsEndOfFile() const;
	bool			IsEndOfBlock() const;

	bool			Rewind();

	UInt32					GetCurBlockPosition() const;
	TaggedFile::BlockType	GetCurBlockType() const;
	UInt32					GetCurBlockLength() const;
	UInt32					GetCurBlockFlags() const;

	bool			NextBlock();

	bool			ReadHeader( TaggedFile::BLOCK_HEADER& header );
	const TaggedFile::BLOCK_HEADER& GetCurBlockHeader();

	bool			ReadBytes( void* pData, int nLength );

	DWORD			GetFilePosition() const;
	bool			SetFilePosition( DWORD dwPos, DWORD positionFlags, bool bReadHeader = false );

	bool			FindBlock( TaggedFile::BlockType blockType );

	HRESULT			ReadMSXMLDoc( IXMLDOMDocument** ppXMLDoc );

protected:
	bool			ReadInitialBlock();

protected:
	DWORD			m_nResult;
	UInt32			m_nLength;

#ifdef OPT_USE_ZLIB
	gzFile			m_hCompressedFile;
#endif
	HANDLE			m_hUncompressedFile;
	const UInt8*	m_pMemMappedFile;
	UInt32			m_nMemMappedLength;
	
	bool			m_bOwnFile;				// true if we have to close the file when we're done

	bool			m_bTaggedFile;

	TaggedFile::BLOCK_HEADER	m_curBlock;	// block header of the block we're on
	UInt32			m_nCurBlockPosition;	// position where it was located
	UInt32			m_nCurFilePosition;
};


// =============================================================================
//	CCaptureFileBlock
// =============================================================================

class CCaptureFileBlock
{
public:
					CCaptureFileBlock() {}
	virtual			~CCaptureFileBlock() {}

	static UInt32
	MAKE_BLOCK_FLAGS( 
		UInt8	compression = TaggedFile::Uncompressed, 
		UInt8	format = TaggedFile::BinaryFormat, 
		UInt8	mbz0 = 0, 
		UInt8	mbz1 = 0 )
	{
		// ia32 is little-endian, so reverse the byte field order
		return (compression) | (format << 8) | (mbz0 << 16) | (mbz1 << 24);
	}

public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL ) = 0;
	virtual bool	Read( CCaptureFileReader& theFile ) = 0;
};


// =============================================================================
//	CCaptureFileVersionBlock
// =============================================================================

class CCaptureFileVersionBlock :
	public CCaptureFileBlock
{
public:
					CCaptureFileVersionBlock();
	virtual			~CCaptureFileVersionBlock();

	static TaggedFile::BlockType GetType()
	{
		return MemUtil::Swap32((UInt32) TaggedFile::kCaptureFileVersionBlock);
	}

	// CCaptureFileBlock
public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );
	virtual bool	Read( CCaptureFileReader& theFile );

public:
	void			SetCurrentVersion( ENV_VERSION FileVersion, ENV_VERSION ProductVersion );
	ENV_VERSION		GetAppVersion( UInt32* pOutVers = NULL ) const;
	ENV_VERSION		GetProductVersion( UInt32* pOutVers = NULL ) const;
	UInt32			GetFileVersion() const;

protected:
	CString			m_strAppVers;
	CString			m_strProductVers;
	CString			m_strFileVers;
};


// =============================================================================
//	CCaptureFileSessionBlock
// =============================================================================

class CCaptureFileSessionBlock :
	public CCaptureFileBlock
{
public:
					CCaptureFileSessionBlock();
	virtual			~CCaptureFileSessionBlock();

	static TaggedFile::BlockType GetType()
	{
		return MemUtil::Swap32( (UInt32) TaggedFile::kCaptureFileSessionBlock );
	}

	// CCaptureFileBlock
public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );
	virtual bool	Read( CCaptureFileReader& theFile );

	void			Reset();

public:
	void			SetWriteTime( time_t inTime );
	time_t			GetWriteTime() const;
	void			SetTimeZoneBias( SInt32 nBias );
	SInt32			GetTimeZoneBias() const;
	void			SetMediaType( TMediaType inMediaType );
	TMediaType		GetMediaType() const;
	void			SetMediaSubType( TMediaSubType inSubType );
	TMediaSubType	GetMediaSubType() const;
	void			SetLinkSpeed( UInt64 nLinkSpeed );
	UInt64			GetLinkSpeed() const;
	void			SetPacketCount( UInt32 nPacketCount );
	UInt32			GetPacketCount() const;
	void			SetComment( const CString& strComment );
	CString			GetComment() const;
	void			SetSessionStartTime( UInt64 t );
	UInt64			GetSessionStartTime() const;
	void			SetSessionEndTime( UInt64 t );
	UInt64			GetSessionEndTime() const;
	void			SetAdapterName( const CString& strAdapterName );
	CString			GetAdapterName() const;
	void			SetAdapterAddr( const CString& strAdapterAddr );
	CString			GetAdapterAddr() const;
	void			SetCaptureName( const CString& strCaptureName );
	CString			GetCaptureName() const;
	void			SetCaptureID( const GUID& id );
	GUID			GetCaptureID() const;
	void			SetOwner( const CString& strOwner );
	CString			GetOwner() const;
	void			SetFileIndex( UInt32 nIndex );
	UInt32			GetFileIndex() const;
	bool			HasFileIndex() const { return !m_strFileIndex.IsEmpty(); }
	void			SetHost( const CString& strHost );
	CString			GetHost() const;
	void			SetEngineName( const CString& strName );
	CString			GetEngineName() const;
	void			SetEngineAddr( const CString& strAddr );
	CString			GetEngineAddr() const;

	// wireless support
	void			SetDomain( TMediaDomain inDomain );
	TMediaDomain	GetDomain() const;
	void			AddDataRate( UInt32 inDataRate );
	void			SetDataRates( const UInt32* inRates, SInt32 nRateCount );
	void			GetDataRates( UInt32* payRates, UInt32& nCount ) const;
	UInt32			GetDataRateCount() const;
	const CChannelManager*	GetChannelManager() const { return &m_channels;	}
	CChannelManager*		GetChannelManager() { return &m_channels;	}

protected:
	bool					m_bIncludeWirelessInfo;
	CString					m_strTimeCount;
	CString					m_strTime;
	CString					m_strMediaType;
	CString					m_strMediaSubType;
	CString					m_strLinkSpeed;
	CString					m_strPacketCount;
	CString					m_strDomain;
	CString					m_strTimezoneBias;
	CString					m_strComment;
	CString					m_strSessionStartTime;
	CString					m_strSessionEndTime;
	CString					m_strAdapterName;
	CString					m_strAdapterAddr;
	CString					m_strCaptureName;
	CString					m_strCaptureID;
	CString					m_strOwner;
	CString					m_strFileIndex;
	CString					m_strHost;
	CString					m_strEngineName;
	CString					m_strEngineAddr;
	TArrayEx<UInt32>		m_ayDataRates;
	CChannelManager			m_channels;

	// for updating the session block
	bool					m_bWroteBlock;
	DWORD					m_nBlockDataLen;
	DWORD					m_nBlockDataPos;
};


// =============================================================================
//	CCaptureFilePacketBlock
// =============================================================================

class CCaptureFilePacketBlock :
	public CCaptureFileBlock
{
public:
					CCaptureFilePacketBlock();
	virtual			~CCaptureFilePacketBlock();

	static TaggedFile::BlockType GetType()
	{
		return MemUtil::Swap32( (UInt32) TaggedFile::kCaptureFilePacketsBlock );
	}

	// CCaptureFileBlock
public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );
	virtual bool	Read( CCaptureFileReader& theFile );

public:
	bool			UpdateHeader( CCaptureFileWriter& theFile, bool bRestoreFilePos );
	bool			WriteHeader( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );

	void				SetPacket( const PeekPacket* thePacket, const UInt8* inPacketData, UInt16 inDataSize = 0 );
	const PeekPacket*	GetPacket() const { return m_pPacket; }
	const UInt8*		GetPacketDataPtr() const { return m_packetData; }
	const UInt8*		GetPacketData() const {	return m_packetData; }
	UInt8*				GetPacketData() { return (UInt8*) m_ayReadPacketData.GetData(); }
	UInt16				GetPacketDataBytes() const 
	{
		return (m_Packet.fSliceLength == 0) ? 
			(m_Packet.fPacketLength) : 
			min(m_Packet.fSliceLength, m_Packet.fPacketLength);
	}

	UInt32			GetPacketCount() const { return m_nPacketCount; }

	void			Reset();

	void			SetUseMemoryPointer( bool bUseMemoryPointer = true );

protected:
	struct SPacketHeader
	{
		TaggedFile::PacketAttribute	actualLength;
		TaggedFile::PacketAttribute	timestampLo;
		TaggedFile::PacketAttribute	timestampHi;
		TaggedFile::PacketAttribute	flagsStatus;
		TaggedFile::PacketAttribute	mediaSpecType;	// for media specific headers, is present
	};

	struct SWirelessPacketHeader
	{
		// channel, data rate, dbm and % strength/noise
		TaggedFile::PacketAttribute	channelNumber;	// renamed (internally) in v5.6.0+
		TaggedFile::PacketAttribute	dataRate;
		TaggedFile::PacketAttribute	signalStrength;
		TaggedFile::PacketAttribute	signaldBm;
		TaggedFile::PacketAttribute	signaldBm1;
		TaggedFile::PacketAttribute	signaldBm2;		// 2K7 (v6.2)
		TaggedFile::PacketAttribute	signaldBm3;		// 2K7 (v6.2)
		TaggedFile::PacketAttribute	noiseStrength;
		TaggedFile::PacketAttribute	noisedBm;		
		TaggedFile::PacketAttribute	noisedBm1;		
		TaggedFile::PacketAttribute	noisedBm2;		// 2K7 (v6.2)
		TaggedFile::PacketAttribute	noisedBm3;		// 2K7 (v6.2)
		TaggedFile::PacketAttribute	channelFreq;	// new to v5.6.0+
		TaggedFile::PacketAttribute	channelBand;	// new to v5.6.0+
		TaggedFile::PacketAttribute flagsN;			// 2K7 (v6.2)
	};

	struct SFullDuplexHeader
	{
		TaggedFile::PacketAttribute	channel;
	};

	struct SWanHeader
	{
		TaggedFile::PacketAttribute	Direction;
		TaggedFile::PacketAttribute	Protocol;
	};

	struct SPacketDataStart
	{
		// this must be the last attribute before the data
		TaggedFile::PacketAttribute	packetDataLength;
	};

	bool						m_bUseMemoryPointer;
	bool						m_bWroteHeader;
	PeekPacket					m_Packet;			// local copy when loading
	const PeekPacket*			m_pPacket;			// pointer reference when saving
	UInt16						m_nDataSize;		// if the packet should be sliced while saving, this is the length
	UInt32						m_nPacketCount;
	const UInt8*				m_packetData;
	TArrayEx<UInt8>				m_ayReadPacketData;	// when we're reading, this is where we store read packet data
	MediaSpecificHeaderAll		m_mediaInfo;		// when we're reading, this is where we store media specific info
	SPacketHeader				m_header;			// we keep one initialized to speed up writing
	MediaSpecificPrivateHeader*	m_MediaSpec;
	Wireless80211PrivateHeader*	m_Wireless;
	FullDuplexPrivateHeader*	m_FullDuplex;
	WideAreaNetworkHeader*		m_Wan;
	SWirelessPacketHeader		m_wirelessHeader;
	SFullDuplexHeader			m_fullDuplexHeader;
	SWanHeader					m_wanHeader;
	SPacketDataStart			m_packetDataStart;
	DWORD						m_nBlockHeaderPos;	// where the file was at when we wrote our header
};


// =============================================================================
//	CCaptureFileIdBlock
// =============================================================================

class CCaptureFileIdBlock :
	public CCaptureFileBlock
{
public:
					CCaptureFileIdBlock();
	virtual			~CCaptureFileIdBlock();

	static TaggedFile::BlockType GetType()
	{
		return MemUtil::Swap32( (UInt32) TaggedFile::kCaptureFileIdBlock );
	}

	// CCaptureFileBlock
public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );
	virtual bool	Read( CCaptureFileReader& theFile );

public:
	void			Enable( bool bEnabled = false ) { m_bEnabled = bEnabled; };
	void			SetId();
	void			SetId( const GUID& id );
	GUID			GetId() const;
	UInt32			GetIndex() const { return m_nIndex; };

protected:
	bool			m_bEnabled;
	CString			m_strId;
	UInt32			m_nIndex;
};


// =============================================================================
//	CCaptureFileGpsBlock
// =============================================================================

class CCaptureFileGpsBlock :
	public CCaptureFileBlock
{
public:
					CCaptureFileGpsBlock();
	virtual			~CCaptureFileGpsBlock();

	static TaggedFile::BlockType GetType()
	{
		return MemUtil::Swap32((UInt32) TaggedFile::kCaptureFileGpsBlock );
	}

	// CCaptureFileBlock
public:
	virtual bool	Write( CCaptureFileWriter& theFile, DWORD* outBytesWritten = NULL );
	virtual bool	Read( CCaptureFileReader& theFile );

public:
	void			SetByteStream( CByteStream* pBS ) { m_pByteStream = pBS; };
	CByteStream*	GetByteStream(){ return m_pByteStream; };

protected:
	CByteStream*	m_pByteStream;
};

#endif /* PEEKTAGGEDFILE_H */
