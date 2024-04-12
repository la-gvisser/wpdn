// =============================================================================
//	PacketLoader.h
// =============================================================================
//	Copyright (c) 2002-2013 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekFile.h"
#include "PeekPacket.h"
#include "memutil.h"
#include "MediaTypes.h"
#include "Product.h"
#include "PacketHeaders.h"
#include "PeekTaggedFile.h"
#include "TimeConv.h"

#define USE_WINDOWS_FIO	1

#if USE_WINDOWS_FIO
#define FILE_HANDLE		HANDLE
#define INVALID_FILE	INVALID_HANDLE_VALUE
#define IS_HANDLE_GOOD(x) (x != INVALID_HANDLE_VALUE)
#define READ_BYTES_FROM_FILE(x, y, c, n) ::ReadFile( x, &y, c, &n, NULL );
#define READ_FROM_FILE(x, y, n) ::ReadFile( x, &y, sizeof(y), &n, NULL );
#define OPEN_FILE(x)	::CreateFile( x, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )
#define CLOSE_FILE(x)	::CloseHandle(x)
#define GET_POSITION(x) SetFilePointer(x, 0, NULL, FILE_CURRENT)
#define SET_POSITION(x,y)	SetFilePointer(x, y, 0, FILE_BEGIN )
#define RESET_POSITION(x)	(SetFilePointer( x, 0, 0, FILE_BEGIN ) != -1)
#else
#define FILE_HANDLE		FILE*
#define INVALID_FILE	NULL
#define IS_HANDLE_GOOD(x) (x != NULL)
#define READ_BYTES_FROM_FILE(x, y, c, n) n = (DWORD) fread( &y, 1, c, x );
#define READ_FROM_FILE(x, y, n) n = (DWORD) fread( &y, 1, sizeof(y), x );
#define OPEN_FILE(x)	_tfopen( x, _T("rb") );
#define CLOSE_FILE(x)	fclose(x)
#define GET_POSITION(x) ftell(x)
#define SET_POSITION(x,y)	fseek(x, y, SEEK_SET)
#define RESET_POSITION(x)	(fseek( x, 0, SEEK_SET ) == 0)
#endif

class CTaggedPacketLoader
{
public:
	CTaggedPacketLoader()	{}
	~CTaggedPacketLoader()
	{	Close();	}
	
	bool Open( LPCTSTR lpszPacketFile )
	{
		return m_reader.Open( lpszPacketFile, false );
	}

	bool Close()
	{
		return m_reader.Close();
	}

	void Detach()
	{
		m_reader.AttachFile( NULL, true );
	}

	bool IsTaggedFile() const { return m_reader.IsTaggedFile(); }

	bool ReadFileHeader( PeekFileHeader *pFileHeader = NULL)
	{
		if (!m_reader.FindBlock( CCaptureFileVersionBlock::GetType() ))
			return false;

		m_version.Read( m_reader );

		if (!m_reader.FindBlock( CCaptureFileSessionBlock::GetType() ))
			return false;

		m_session.Read( m_reader );
		memset(&m_peekFileHeader, 0, sizeof(m_peekFileHeader));
		m_version.GetAppVersion( &m_peekFileHeader.fAppVers );
		m_peekFileHeader.fPacketCount = m_session.GetPacketCount();
		m_peekFileHeader.fStatus = 0;
		m_peekFileHeader.fLinkSpeed = (UInt32) m_session.GetLinkSpeed();	// TODO: fix this for > 4G speed files?
		m_peekFileHeader.fMediaSubType = m_session.GetMediaSubType();
		m_peekFileHeader.fMediaType = m_session.GetMediaType();
		m_peekFileHeader.fTimeDate = (UInt32) m_session.GetWriteTime();
		m_peekFileHeader.fLength = 0;	// too bad for you
		if (pFileHeader)
			*pFileHeader = m_peekFileHeader;

		return m_reader.FindBlock( CCaptureFilePacketBlock::GetType() );
	}

	UInt32 GetProductType() const
	{
		switch (m_peekFileHeader.fMediaType)
		{
		case kMediaType_802_3:
			switch (m_peekFileHeader.fMediaSubType)
			{
			case kMediaSubType_802_11_b:
			case kMediaSubType_802_11_a:
			case kMediaSubType_802_11_gen:
				return WIRELESS;
			case kMediaSubType_Native:
				return ETHERNET;
			}
		case kMediaType_802_5:
			return TOKENRING;
		case kMediaType_CoWan:
			return WAN;
		default:
			return ETHERNET;
		}
	}

	bool GetNextPacket( UInt8* pData, UInt16& nLength )
	{
		if (m_reader.IsEndOfBlock())
			return false;

		if (!m_packet.Read( m_reader ))
			return false;
		
		// get the right packet stuff
		m_peekPacket = *m_packet.GetPacket();
		// now copy the data
		nLength = m_packet.GetPacketDataBytes();
		memcpy( pData, m_packet.GetPacketData(), nLength );
		
		return true;
	}

	const PeekPacket* GetPacketHeader() const 
	{
		return &m_peekPacket;
	}

	const Wireless80211PrivateHeader* GetWirelessHeader() const
	{
		if (m_peekPacket.fMediaSpecInfoBlock)
		{
			const MediaSpecificPrivateHeader* pMedia =
				reinterpret_cast<const MediaSpecificPrivateHeader*>(m_peekPacket.fMediaSpecInfoBlock);
			if (pMedia->nType == kMediaSpecificHdrType_Wireless3)
				return reinterpret_cast<const Wireless80211PrivateHeader*>(m_peekPacket.fMediaSpecInfoBlock + sizeof(*pMedia));
		}
		return NULL;
	}

	UInt32 GetPosition() const		
	{	
		return m_reader.GetFilePosition();
	}

	bool SetPosition( UInt32 nPos )	
	{	
		return m_reader.SetFilePosition( nPos, SEEK_SET, false );
	}

	TMediaDomain GetDomain() const
	{
		return m_session.GetDomain();
	}

	SInt32 GetTimeZoneBias() const
	{
		return m_session.GetTimeZoneBias();
	}

	const CCaptureFileVersionBlock& GetVersion() const
	{
		return m_version;
	}

	const CCaptureFileSessionBlock& GetSession() const
	{
		return m_session;
	}

	UInt64 GetPacketTimeStamp() const
	{
		return m_peekPacket.fTimeStamp;
	}

protected:
	CCaptureFileReader	m_reader;
	CCaptureFileVersionBlock m_version;
	CCaptureFileSessionBlock m_session;
	CCaptureFilePacketBlock	 m_packet;

	PeekFileHeader		m_peekFileHeader;
	PeekPacket			m_peekPacket;

	// common storage for any media specific stuff in the file
	MediaSpecificHeaderAll	m_mediaSpecInfo;	
};

class CPacketLoader
{
public:
	CPacketLoader() : 
		m_hPacketFile(INVALID_FILE),
		m_nType(0)
		{}

	~CPacketLoader() 
	{	Close();	}

	bool Open(LPCTSTR lpszPacketFile)
	{	// query for access
		m_strError.Empty();	
		m_ayDataRates.RemoveAllItems();
		m_Channels.SetChannelType(CChannelManager::kMaxType);

		m_bIsTaggedFile = m_taggedLoader.Open( lpszPacketFile );

		if (m_bIsTaggedFile)
		{
			m_hPacketFile = INVALID_FILE;	// let the tagged loader manage it
			m_taggedLoader.ReadFileHeader( &m_peekHeader );
			m_nType = m_taggedLoader.GetProductType();
			
			// get any channel info
			const CChannelManager* pChannels = m_taggedLoader.GetSession().GetChannelManager();
			m_Channels = *pChannels;

			// for wireless, get data rates
			if (IsWireless())
			{
				UInt32 nCount = m_taggedLoader.GetSession().GetDataRateCount();
				m_ayDataRates.SetCount( nCount );
				m_taggedLoader.GetSession().GetDataRates( (UInt32*) m_ayDataRates.GetData(), nCount );
			}

			return true;
		}
		else
		{
			m_taggedLoader.Close();
			m_hPacketFile = OPEN_FILE( lpszPacketFile );
		}

		size_t nLen = _tcslen(lpszPacketFile);
		if (nLen > 4 && _tcsicmp(lpszPacketFile + (nLen - 4), _T(".apc")) == 0)
			m_nType = WIRELESS;
		if (nLen > 4 && _tcsicmp(lpszPacketFile + (nLen - 4), _T(".tpc")) == 0)
			m_nType = TOKENRING;
		if (nLen > 4 && _tcsicmp(lpszPacketFile + (nLen - 4), _T(".pkt")) == 0)
			m_nType = ETHERNET;
		if (nLen > 4 && _tcsicmp(lpszPacketFile + (nLen - 4), _T(".wpc")) == 0)
			m_nType = WAN;

		if (IS_HANDLE_GOOD(m_hPacketFile))
		{
			if (ReadFileHeader( m_peekHeader ))
			{	// make sure that the media is set correctly
				if (IsTokenRing() && m_peekHeader.fMediaType == 0)
				{
					m_peekHeader.fMediaType = kMediaType_802_5;
				}

				if ( IsWireless() )
				{	
					UInt8* pData = (UInt8*) &m_peekHeader.fReserved;
					m_Domain = *((TMediaDomain*) pData);
					pData++;
					// get any data rate info out of the reserved area
					for (int i = 0; i < sizeof( m_peekHeader.fReserved ) && pData[i] != 0; i++)
						m_ayDataRates.AddItem( (UInt32) pData[i] );
				}

				return true;
			}
			else
			{
				m_strError.Format(_T("ReadFileHeader failed for %s"), lpszPacketFile );
#ifdef TRACE
				TRACE(_T("%s\n"), m_strError);
#endif
				return false;
			}
		}
		else
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			// Process any inserts in lpMsgBuf.
			// ...
			// Display the string.
			m_strError = (LPCTSTR) lpMsgBuf;
			// strip out CRLFs
			m_strError.Remove(_T('\n'));
			m_strError.Remove(_T('\r'));
#ifdef TRACE
			TRACE(_T("%s: Error in CPacketLoader: %s\n"), lpszPacketFile, m_strError);
#endif
			_tprintf(_T("%s: Error in CPacketLoader: %s\n"), lpszPacketFile, m_strError.GetString() );
//			MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );
			return false;
		}
	}

	void Close()
	{
		if (IS_HANDLE_GOOD(m_hPacketFile))
			CLOSE_FILE(m_hPacketFile);
		m_hPacketFile = INVALID_FILE;
		// close the tagged reader too
		m_taggedLoader.Close();
	}

	PeekPacket* GetPacketHeader()	{	return &m_packetHeader;				}
	const FullDuplexPrivateHeader* GetFullDuplexHeader() const
	{	
		const MediaSpecificPrivateHeader* pMedia = GetMediaSpecificHeader();
		if (pMedia && pMedia->nType == kMediaSpecificHdrType_FullDuplex)
			return reinterpret_cast<const FullDuplexPrivateHeader*>(((UInt8*) pMedia)+ sizeof(*pMedia));
		return NULL;
	}
	const Wireless80211PrivateHeader* GetWirelessHeader() const
	{	
		const MediaSpecificPrivateHeader* pMedia = GetMediaSpecificHeader();
		if (pMedia && pMedia->nType == kMediaSpecificHdrType_Wireless3)
			return reinterpret_cast<const Wireless80211PrivateHeader*>(((UInt8*) pMedia)+ sizeof(*pMedia));
		return NULL;
	}
	const MediaSpecificPrivateHeader* GetMediaSpecificHeader() const
	{
		if (m_packetHeader.fMediaSpecInfoBlock)
			return reinterpret_cast<const MediaSpecificPrivateHeader*>(m_packetHeader.fMediaSpecInfoBlock);
		return NULL;
	}
	PeekFileHeader* GetFileHeader()	{	return &m_peekHeader;				}
	UInt32 GetPacketCount() const	{	return m_peekHeader.fPacketCount;	}
	UInt8 GetPacketFileVersion() const	{	return m_peekHeader.fVersion;	}
	UInt32 GetChannelCount() const
	{
		return m_Channels.GetCount();
	}
	const CChannelManager* GetChannels() const
	{
		return &m_Channels;
	}
	UInt32 GetDataRateCount() const
	{
		return m_ayDataRates.GetCount();
	}
	const UInt32* GetDataRates() const
	{
		return (UInt32*) m_ayDataRates.GetData();
	}
	bool GetNextPacket( UInt8* pData, UInt16& nSize )
	{
		if (m_bIsTaggedFile)
		{
			if (m_taggedLoader.GetNextPacket( pData, nSize ))
			{
				m_packetHeader = *m_taggedLoader.GetPacketHeader();
				return true;
			}
			
			return false;
		}

		if (ReadPacketHeader())
		{
			if (m_packetHeader.fSliceLength != 0)
				nSize = min(m_packetHeader.fSliceLength, m_packetHeader.fPacketLength);
			else
				nSize = m_packetHeader.fPacketLength;

			// fix the timestamp
			m_packetHeader.fTimeStamp = TimeConv::MicroPeekToPeek( m_packetHeader.fTimeStamp );

//			ASSERT( nSize != 0 );
			if (nSize == 0)
				return false;
#ifdef DEBUG
			memset( pData, 0xcc, 2048 );
#endif
			// if this is a wireless file, read the obsolete wireless struct next and put into our media specific info
			if (IsWireless())
			{
				m_packetHeader.fMediaSpecInfoBlock = (UInt8*) &m_mediaSpecHeader;
				m_mediaSpecHeader.StdHeader.nSize = sizeof(MediaSpecificPrivateHeader) + sizeof(Wireless80211PrivateHeader);
				m_mediaSpecHeader.StdHeader.nType = kMediaSpecificHdrType_Wireless3;
				// now read the obsolete header
				Wireless80211PrivateHeaderObs theOldHeader;
				if (!ReadPacketData( sizeof(theOldHeader), (UInt8*) &theOldHeader ))
					return false;	// WTF??
				WirelessHeaderUtil::InitMediaSpecInfo( &m_mediaSpecHeader.MediaInfo.wireless );
				m_mediaSpecHeader.MediaInfo.wireless.Channel.Channel = theOldHeader.Channel;
				m_mediaSpecHeader.MediaInfo.wireless.DataRate = theOldHeader.DataRate;
				m_mediaSpecHeader.MediaInfo.wireless.SignalPercent = theOldHeader.SignalStrength;
				// apply defaults to the rest
				// also adjust the size
				m_packetHeader.fPacketLength -= 4;
				if (m_packetHeader.fSliceLength != 0)
					m_packetHeader.fSliceLength -= 4;
				nSize -= 4;
			}
			else // point the media specific info to NULL
				m_packetHeader.fMediaSpecInfoBlock = NULL;

			if (ReadPacketData(nSize, pData))
			{	// it went OK, so read another empty byte if the length is odd
				if (nSize & 0x01)
				{
					UInt8 theByte = 0x00;
					ReadPacketData(1, &theByte);	// if it fails we went off the end of the file, but that's cool for now
				}

				return true;
			}
		}

		return false;
	}

	int GetFileType() const	{	return m_nType;	}

	bool IsWireless() const	{	return (m_nType & WIRELESS) != 0;	}
	bool IsTokenRing() const {	return (m_nType & TOKENRING) != 0;	}
	bool IsEthernet() const	{	return (m_nType & ETHERNET) != 0;	}
	bool IsWan() const {	return (m_nType & WAN) != 0;	}
	bool IsTagged() const { return m_bIsTaggedFile; }

	DWORD GetPosition() const			
	{	
		if (m_bIsTaggedFile) return m_taggedLoader.GetPosition();
		return GET_POSITION(m_hPacketFile);	
	}

	void SetPosition(DWORD dwPos)
	{	
		if (m_bIsTaggedFile) 
			m_taggedLoader.SetPosition( dwPos );
		else
			SET_POSITION(m_hPacketFile, dwPos);				
	}

	LPCTSTR GetError() const { return m_strError;	}

	ENV_VERSION GetAppVersion() const
	{	// current verison is 2.0.0.0
		if (m_bIsTaggedFile)
			return m_taggedLoader.GetVersion().GetAppVersion();

		ENV_VERSION appVers;
		UInt8* pPtr = (UInt8*) &m_peekHeader.fAppVers;
		appVers.MajorVer = pPtr[3];
		appVers.MinorVer = pPtr[2];
		appVers.MajorRev = pPtr[1];
		appVers.MinorRev = pPtr[0];
		return appVers;
	}

	ENV_VERSION GetProductVersion() const
	{	// current verison is 5.1.0.0
		if (m_bIsTaggedFile)
			return m_taggedLoader.GetVersion().GetProductVersion();

		ENV_VERSION appVers;
		appVers.MajorVer = 5;
		appVers.MinorVer = 1;
		appVers.MajorRev = 0;
		appVers.MinorRev = 0;
		return appVers;
	}
	
	TMediaDomain GetDomain() const
	{
		if (m_bIsTaggedFile)
			return m_taggedLoader.GetDomain();
		else
			return m_Domain;
	}

	SInt32 GetTimeZoneBias()
	{
		if (m_bIsTaggedFile)
			return m_taggedLoader.GetTimeZoneBias();
		else
		{	// figure it out based on our timezone
			TIME_ZONE_INFORMATION theTimeZone;
			::GetTimeZoneInformation( &theTimeZone );
			return theTimeZone.Bias;
		}
	}

	// this should return time in nanoseconds; old version was always microseconds
	UInt64 GetPacketTimeStamp() const
	{
		if (m_bIsTaggedFile)
		{
			return m_taggedLoader.GetPacketTimeStamp();
		}
		else
		{	// converted from ms to ns
			return m_packetHeader.fTimeStamp;
		}
	}

protected:
	bool ReadFileHeader( 
		PeekFileHeader&	outHeader )
	{
		if ( !RESET_POSITION( m_hPacketFile ) )
		{
			return false;
		}

		DWORD	nBytesRead = 0;
		READ_FROM_FILE( m_hPacketFile, outHeader, nBytesRead );
		if ( nBytesRead != sizeof(PeekFileHeader) )
		{
			return false;
		}

		SwapFileHeader( outHeader );

		return true;
	}

	bool ReadPacketHeader()
	{
		DWORD dwOut = 0;
		switch (m_peekHeader.fVersion & 0x7f)
		{
		case 7:
		{
			PeekPacket7 theOldPacket;
			READ_FROM_FILE(m_hPacketFile, theOldPacket, dwOut);
			if (dwOut != sizeof(theOldPacket))
				return false;
			SwapPacketHeader( theOldPacket );
			// copy to new packet format
			m_packetHeader.fFlags = theOldPacket.fFlags;
			m_packetHeader.fPacketLength = theOldPacket.fPacketLength;
			m_packetHeader.fProtoSpec = (UInt32) theOldPacket.fProtoSpec;
			m_packetHeader.fSliceLength = theOldPacket.fSliceLength;
			m_packetHeader.fStatus = theOldPacket.fStatus;
			m_packetHeader.fTimeStamp = theOldPacket.fTimeStamp;
			return true;
		}
		case 8:
		{	
			PeekPacket8 theOldPacket;
			READ_FROM_FILE(m_hPacketFile, theOldPacket, dwOut);
			SwapPacketHeader( theOldPacket );
			if (dwOut != sizeof(theOldPacket))
				return false;
			// copy to new packet format
			m_packetHeader.fFlags = theOldPacket.fFlags;
			m_packetHeader.fPacketLength = theOldPacket.fPacketLength;
			m_packetHeader.fProtoSpec = (UInt32) theOldPacket.fProtoSpec;
			m_packetHeader.fSliceLength = theOldPacket.fSliceLength;
			m_packetHeader.fStatus = theOldPacket.fStatus;
			m_packetHeader.fTimeStamp = theOldPacket.fTimeStamp;
			return true;
		}
		default:
			return false;
		}
	}

	bool ReadPacketData(
		UInt16 nLength,
		UInt8* pData )
	{
		DWORD dwOut = 0;
		READ_BYTES_FROM_FILE(m_hPacketFile, *pData, nLength, dwOut);
		return (dwOut == nLength);
	}

	PeekFileHeader	m_peekHeader;
	// things in the wireless file headers but not defined in PeekFileHeader
	TMediaDomain	m_Domain;
	FILE_HANDLE		m_hPacketFile;
	PeekPacket		m_packetHeader;
	MediaSpecificHeaderAll	m_mediaSpecHeader;	// this isn't really in the file, but we can use it
	UInt32			m_nType;
	CString			m_strError;
	CTaggedPacketLoader	m_taggedLoader;
	bool			m_bIsTaggedFile;
	TArrayEx<UInt32> m_ayDataRates;
	CChannelManager m_Channels;

public:
	static void SwapFileHeader( PeekFileHeader& ioHeader )
	{
		ioHeader.fLength = MemUtil::Swap32( ioHeader.fLength );
		ioHeader.fPacketCount = MemUtil::Swap32( ioHeader.fPacketCount );
		ioHeader.fTimeDate = MemUtil::Swap32( ioHeader.fTimeDate );
		ioHeader.fTimeStart = MemUtil::Swap32( ioHeader.fTimeStart );
		ioHeader.fTimeStop = MemUtil::Swap32( ioHeader.fTimeStop );
		ioHeader.fMediaType = MemUtil::Swap32( ioHeader.fMediaType );
		ioHeader.fMediaSubType = MemUtil::Swap32( ioHeader.fMediaSubType );
		ioHeader.fLinkSpeed = MemUtil::Swap32( ioHeader.fLinkSpeed );
	}

	static void SwapPacketHeader( PeekPacket8& packet )
	{
		packet.fPacketLength = MemUtil::Swap16(packet.fPacketLength);
		packet.fProtoSpec = MemUtil::Swap32(packet.fProtoSpec);
		packet.fSliceLength = MemUtil::Swap16(packet.fSliceLength);
		packet.fTimeStamp = MemUtil::Swap64(packet.fTimeStamp);
	}

	static void SwapPacketHeader( PeekPacket7& packet )
	{
		packet.fPacketLength = MemUtil::Swap16(packet.fPacketLength);
		packet.fProtoSpec = MemUtil::Swap16(packet.fProtoSpec);
		packet.fSliceLength = MemUtil::Swap16(packet.fSliceLength);
		packet.fTimeStamp = MemUtil::Swap64(packet.fTimeStamp);
	}
};

